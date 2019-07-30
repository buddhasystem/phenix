#!/usr/local/bin/tcsh -f

# setenv HOME /phenix/u/hachiya
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



if ( $#argv != 3 ) then
  echo "pisa_run.csh num input filename"
  echo "  jobno = ???    : jobnumber "
  echo "  nevent = ???   : nevent"
  echo "  filename = ??? : input filename"
  echo ""
  exit -1
endif

set jobno     = $1
set nevent    = $2
set inputfile = $3
set scriptdir = $HOME/phenix/embed/sim/pisa/submit
set outputdir = $HOME/phenix/embed/sim/pisa
set tmpdir    = "/home/tmp/mxmp_job_$jobno"

echo $jobno
echo $nevent
echo $inputfile 
echo $scriptdir
echo $outputdir
echo $tmpdir

# make wrkdir

if( ! -d $tmpdir ) then
  mkdir -p $tmpdir
endif
echo "cd $tmpdir"
cd       $tmpdir


##pisalinker with new library
/afs/rhic/phenix/software/simulation/run15/pisaLinker.csh


##link inputfile  (phpythia)
set infile = `basename $inputfile`
cp $inputfile .

# replace event.par
#rm -f event.par
#cp $scriptdir/event.par .

#prepare pisa.input
set pinname = "pisa.input"
rm -f $pinname

echo "0" >  $pinname
echo "N" >> $pinname
echo "0" >> $pinname

# prepare glogon.kumac
set kumname = "glogon.kumac"
rm -f $kumname

echo "macro glogon.kumac"             >  $kumname
echo "pisafile PISAEvent_$jobno.root" >> $kumname
echo "phpythia 100 $infile"           >> $kumname
echo "ptrig $nevent"                  >> $kumname
echo "return"                         >> $kumname

#exec pisa
echo "exec pisa"
pisa < pisa.input

#move the PISAEvent.root
mv "PISAEvent_$jobno.root" $outputdir

rm -rf $tmpdir
