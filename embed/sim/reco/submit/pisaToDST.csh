#!/usr/local/bin/tcsh -f

setenv HOME /phenix/u/hachiya
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
#setenv LD_LIBRARY_PATH /phenix/subsys/vtx/hachiya/12.02/install/lib:$LD_LIBRARY_PATH

##################################

if( $#argv != 4) then
  echo "pisaToDST.csh num"
  echo "   num = job number"
  echo "   evtnum = Nevent"
  echo "   infile = inputfile"
  echo "   outfile = outputfile"
  exit -1
endif

set jobno     = $1
set evtnum    = $2
set inputfile = $3
set outfile   = $4

set scriptdir = "/phenix/hhj/hachiya/15.08/embed/sim/reco/submit"
#set outputdir = "/phenix/hhj2/hachiya/15.08/embed/sim/reco/"
set outputdir = "/phenix/hhj2/hachiya/15.08/embed/sim/reco_409471/"
set tmpdir    = "/home/tmp/hachiya_job_$jobno"

set infile = `basename $inputfile`


echo $jobno
echo $evtnum
echo $inputfile
echo $outfile
echo $scriptdir
echo $tmpdir


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

#link input file
#ln -s $inputfile .
cp $inputfile .

ls 

#generate input file
echo ".x pisaToDST.C($evtnum, "'"'$infile'"'", "'"'${outfile}'"'");" >  cmd.input
echo ".q"                                                            >> cmd.input


##run root
root -b < cmd.input

mv -f $outfile $outputdir

rm -fr $tmpdir

