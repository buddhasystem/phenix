#!/usr/local/bin/tcsh -f


# Make the data DST with raw detector hits

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
setenv LD_LIBRARY_PATH $MYINSTALL/lib:$LD_LIBRARY_PATH

# -mxp- Was:
# /phenix/hhj/hachiya/15.08/embed/real/dstmerge/install/lib:/phenix/hhj/hachiya/15.08/source/embedreco/install/lib/

##################################

if( $#argv != 3) then
  echo "reco_rawhit_updated.csh num"
  echo "   num = job number"
  echo "   evtnum = Nevent"
  echo "   infile = inputfile"
  exit -1
endif

set jobno     = $1
set evtnum    = $2
set inputfile = $3

set scriptdir = "$EMBEDDING_HOME/real/dstmerge/wrk/submit"

# -mxp- Was:
# set scriptdir = "/direct/phenix+hhj/hachiya/15.08/embed/real/dstmerge/wrk/submit"

set outputdir = $DATADIR
set tmpdir    = "/home/tmp/${USER}_job_$jobno"

set infile = `basename $inputfile`
set runnum = `echo $infile | awk -F'-' '{printf "%d", $2}'`


echo $jobno
echo $evtnum
echo $inputfile
echo $runnum
echo $scriptdir
echo $tmpdir


#move to wrk directory
if( ! -d $tmpdir ) then
  mkdir -p $tmpdir
endif
echo "cd $tmpdir"
cd       $tmpdir


##pisaToDSTLinker with new library
/opt/phenix/core/bin/LuxorLinker.pl -1 $runnum
# -mxp- Was:
# /afs/rhic/phenix/software/calibration/data/LuxorLinker.pl -1 $runnum

cp ${scriptdir}/Fun4All_CA_merge.C .
cp ${scriptdir}/OutputManager.C    .
cp ${scriptdir}/QA.C               .
cp ${scriptdir}/TrigSelect.C.run14auau200  TrigSelect.C
cp ${scriptdir}/rawdatacheck.C     .
cp ${scriptdir}/copy_prdf.pl .
#copy input file
cp $inputfile .
#./copy_prdf.pl $infile

set copyErrCode = $?
echo '==========================ERROR CODE=================================' $copyErrCode

if ( $copyErrCode != 0 ) then
echo Could not get the input file $inputfile, exiting
cd $HOME
rm -fr $tmpdir
echo "removed $tmpdir"
exit $copyErrCode
endif



#generate input file
echo ".x Fun4All_CA_merge.C($evtnum, "'"'$infile'"'");" >  cmd.input
echo ".q"                                               >> cmd.input


##run root
root -b < cmd.input

#move
echo "mv -f CNTmerge_*.root $outputdir"
mv -f CNTmerge_*.root $outputdir

#remove tmp dir
cd $HOME
rm -fr $tmpdir
echo "removed $tmpdir"

