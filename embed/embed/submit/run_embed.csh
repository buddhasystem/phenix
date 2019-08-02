#!/usr/local/bin/tcsh -f

#setenv HOME /phenix/u/hachiya
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
#setenv LD_LIBRARY_PATH /phenix/hhj/hachiya/15.08/source/cgl/install/lib/:/phenix/hhj/hachiya/15.08/source/embedreco/install/lib/:/phenix/hhj/hachiya/15.08/source/svx_cent_ana/install/lib/:$LD_LIBRARY_PATH

# make sure the path is set elsewhere (for now)
#setenv LD_LIBRARY_PATH /phenix/hhj/hachiya/15.08/source/svx_cent_ana/install/lib/:$LD_LIBRARY_PATH

##################################

if( $#argv != 7) then
  echo "reco_rawhit.csh num"
  echo "   num       = job number"
  echo "   evtnum    = Nevent"
  echo "   inputreal = inputfile"
  echo "   inputsim  = inputfile"
  echo "   outdst    = outdst"
  echo "   outntuple = outdst"
  echo "   outntana  = outdst"
  exit -1
endif

echo "Setting internal variables..."

#set runnum    = "409471"
set jobno     = $1
set evtnum    = $2
set inputsim  = $3
set inputreal = $4
set outdst    = $5
set outntuple = $6
set outntana  = $7

set scriptdir   = "/direct/phenix+u/mxmp/phenix/embed/embed/submit"
set outdstdir   = "/direct/phenix+u/mxmp/outdstdir"
set outntdir    = "/direct/phenix+u/mxmp/outntdir"
set outntanadir = "/direct/phenix+u/mxmp/outanadir"
set tmpdir      = "/home/tmp/mxmp_job_$jobno"

set inreal = `basename $inputreal`
set insim  = `basename $inputsim`

set runnum = `echo $inreal | awk -F'-' '{printf "%d", $2}'`



echo "runnum       $runnum     "
echo "jobno        $jobno      "
echo "evtnum       $evtnum     "
echo "inputreal    $inputreal  "
echo "inputsim     $inputsim   "
echo "outdst       $outdst     "
echo "outntuple    $outntuple  "
echo "outntana     $outntana   "
echo "scriptdir    $scriptdir  "
echo "outdstdir    $outdstdir  "
echo "outntdir     $outntdir   "
echo "outntanadir  $outntanadir"
echo "tmpdir       $tmpdir     "


#move to wrk directory
if( ! -d $tmpdir ) then
  mkdir -p $tmpdir
endif
echo "cd $tmpdir"
cd       $tmpdir


##pisaToDSTLinker with new library
/opt/phenix/core/bin/LuxorLinker.pl -1 $runnum

cp ${scriptdir}/Fun4All_embedeval.C .
#cp ${scriptdir}/Fun4All_embedeval_svx.C .
cp ${scriptdir}/embed_IOManager.C    .
cp ${scriptdir}/svxPISA.par         .
cp ${scriptdir}/svx_threshold.dat   .

#copy input file
cp $inputreal .
cp $inputsim .

echo "pwd"
pwd
ls


#generate input file
#echo ".x Fun4All_embedeval_svx.C($evtnum, "'"'$insim'"'", "'"'$inreal'"'", "'"'$outdst'"'", "'"'$outntuple'"'", "'"'$outntana'"'");" >  cmd.input
echo ".x Fun4All_embedeval.C($evtnum, "'"'$insim'"'", "'"'$inreal'"'", "'"'$outdst'"'", "'"'$outntuple'"'", "'"'$outntana'"'");" >  cmd.input
echo ".q"                                               >> cmd.input

##run root
root -b < cmd.input

#move
#mv -f $outdst    $outdstdir
#mv -f $outntuple $outntdir
#mv -f $outntana  $outntanadir

#remove tmp dir
#cd $HOME
#rm -fr $tmpdir
#echo "removed $tmpdir"

