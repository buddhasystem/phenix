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

if (! $?EMBEDDING_SET) then       
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
set outputdir = $DATADIR
set tmpdir    = "/home/tmp/"$USER"_job_$jobno"

printf "Job number:\t\t$jobno\n"
printf "Events:\t\t\t$nevent\n"
printf "Input file:\t\t$inputfile\n"
printf "Script directory:\t$scriptdir\n"
printf "Output directory:\t$outputdir\n"
printf "Tmp directory:\t\t$tmpdir\n"

set outfile = "pisa_${nevent}_${jobno}.root"
printf "Output file:\t\t$outfile\n"
# exit 0

# make wrkdir

if( ! -d $tmpdir ) then
  mkdir -p $tmpdir
endif
echo "cd $tmpdir"
cd $tmpdir
pwd

##pisalinker with new library
/afs/rhic/phenix/software/simulation/run15/pisaLinker.csh

## get the input file  (phpythia)
set infile = `basename $inputfile`
cp $inputfile .

#prepare pisa.input
set pinname = "pisa.input"
rm -f $pinname

echo "0" >  $pinname
echo "N" >> $pinname
echo "0" >> $pinname

# prepare glogon.kumac
set kumname = "glogon.kumac"
rm -f $kumname


echo "macro glogon.kumac"	>  $kumname
echo "pisafile $outfile"	>> $kumname
echo "phpythia 100 $infile"	>> $kumname
echo "ptrig $nevent"		>> $kumname
echo "return"			>> $kumname

#exec pisa
echo "exec pisa"
pisa < pisa.input

#move the PISAEvent.root
mv $outfile $outputdir

rm -rf $tmpdir
