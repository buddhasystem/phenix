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
setenv LD_LIBRARY_PATH /phenix/u/mxmp/install/lib:$LD_LIBRARY_PATH

# -mxp- Was:
# /phenix/hhj/hachiya/15.08/embed/real/dstmerge/install/lib:/phenix/hhj/hachiya/15.08/source/embedreco/install/lib/

##################################

if( $#argv != 3) then
  echo "reco_rawhit.csh num"
  echo "   num = job number"
  echo "   evtnum = Nevent"
  echo "   infile = inputfile"
  exit -1
endif

set jobno     = $1
set evtnum    = $2
set inputfile = $3

set scriptdir = "/direct/phenix+u/mxmp/phenix/embed/real/dstmerge/wrk/submit"

# -mxp- Was:
# set scriptdir = "/direct/phenix+hhj/hachiya/15.08/embed/real/dstmerge/wrk/submit"

set outputdir = "/direct/phenix+u/mxmp/real"
set tmpdir    = "/home/tmp/mxmp_job_$jobno"

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

#copy input file
#cp $inputfile .
${HOME}/copy_prdf.pl $infile


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

