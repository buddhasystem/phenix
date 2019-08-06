#!/usr/local/bin/tcsh -f

################################################################
# This is based on the original logic by Takashi.
#
# The idea is to set up the environment in a uniform way to facilitate
# accessing data among the various stages of the embedding process.
#
# For that reason the script embed/setup/embedding_setup template
# needs to be used (after modification appropriate for the user)

# --Maxim Potekhin /mxp/--

if (! $?EMBEDDING_HOME) then       
echo "Environment not set, exiting..."
exit -1   
endif

setenv prompt 1
source /etc/csh.login
foreach i (/etc/profile.d/*.csh)
  source $i
end
source $HOME/.login
unsetenv OFFLINE_MAIN
unsetenv ONLINE_MAIN
unsetenv ROOTSYS

source /opt/phenix/bin/phenix_setup.csh new

##################################

if( $#argv != 3) then
  echo "pisaToDST.csh num"
  echo "   num = job number"
  echo "   evtnum = Nevent"
  echo "   infile = inputfile"
#  echo "   outfile = outputfile"
  exit -1
endif

set jobno     = $1
set evtnum    = $2
set inputfile = $3
set outfile   = "pisatodst_${evtnum}_${jobno}.root"

set scriptdir = $EMBEDDING_HOME/sim/reco/submit
set outputdir = $DATADIR
set tmpdir    = "/home/tmp/${USER}_job_$jobno"

set infile = `basename $inputfile`


echo Job number $jobno
echo Number of events $evtnum
echo Input file $inputfile
echo Output file $outfile
echo Script directory $scriptdir
echo Tmp directory $tmpdir

#move to wrk directory
if( ! -d $tmpdir ) then
  mkdir -p $tmpdir
endif
echo "cd $tmpdir"
cd       $tmpdir


##pisaToDSTLinker with new library
#/afs/rhic/phenix/software/simulation/run15/pisaToDSTLinker.csh
#${scriptdir}/pisaToDSTLinker.csh
${scriptdir}/pisaToDSTLinker_run14.csh

rm -f pisaToDST.C
cp ${scriptdir}/pisaToDST.C .

rm -f pisaToDST_IOManager.C
cp ${scriptdir}/pisaToDST_IOManager.C .

cp ${scriptdir}/svxPISA.par .
cp ${scriptdir}/svx_threshold.dat .

cp $inputfile .

ls 

#generate input file
echo ".x pisaToDST.C($evtnum, "'"'$infile'"'", "'"'${outfile}'"'");" >  cmd.input
echo ".q"                                                            >> cmd.input


##run root
root -b < cmd.input

mv -f $outfile $outputdir

rm -fr $tmpdir

