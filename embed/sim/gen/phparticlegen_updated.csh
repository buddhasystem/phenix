#!/usr/local/bin/tcsh -f

################################################################
# This is based on the original logic in phparticlegen_pip.csh
# by Takashi.
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

set N = $1
set i = $2

echo 'Location of libraries:' $MYINSTALL/lib
echo 'Data directory:' $DATADIR

echo 'Statistics:' $N


source /opt/phenix/bin/phenix_setup.csh
setenv LD_LIBRARY_PATH $MYINSTALL/lib:$LD_LIBRARY_PATH

echo LD_LIBRARY_PATH $LD_LIBRARY_PATH

# Start simplified script:
set pname = "pip"
set pid   = "211"


set outname = `printf "phpythia_%s_%06d_%06d.root" $pname $N $i`

echo $outname
root -b -q  'phparticlegen_pip.C('$N', '$pid', "'"$outname"'")'

echo Moving the output file $outname to the directory $DATADIR
mv $outname $DATADIR

exit 0
