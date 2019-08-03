#!/usr/local/bin/tcsh -f

################################################################
# This is based on the original logic in phparticlegen_pip.csh
# by Takashi.
#
# The idea is to set up the environment in a uniform way to facilitate
# accessing data among the various stages of the embedding process.
# --Maxim Potekhin /mxp/--

echo 'location of libraries:' $HOME/install/lib
# ls $HOME/install/lib

source /opt/phenix/bin/phenix_setup.csh
setenv LD_LIBRARY_PATH $HOME/install/lib/:$LD_LIBRARY_PATH

echo $LD_LIBRARY_PATH

# Start simplified script:
set pname = "pip"
set pid = "211"
set i = 1

set outname = `printf "gen_%s_xyvtx_%06d.root" $pname $i`

echo $outname
root -b -q  'phparticlegen_pip.C(10, '$pid', "'"$outname"'")'

exit 0
