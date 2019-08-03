#!/usr/local/bin/tcsh -f

################################################################
# DISCLAIMER: DEPRECATED VERSION
#
# This is modeled after Takashi's script (see the commented section
# on the bottom. Now a renamed copy will be created for cleaner
# set up.
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


# Historical settings:, already commented out by TakashiL
# set pname = "em" #set pid   = "11" #set pnamel = ("em" "pip" "pim") #set pidl   = ("11" "211" "-211")



# -- below is Takashi's original script:
# set max = 97
# set pnamel = ("pip")
# set pidl   = ("211")

# @ idx = 1
# foreach pname ($pnamel)
#   set pid = $pidl[$idx]
#   echo "$pname, $pid"
  
#   @ i = 96
#   while ( ${i} < ${max} )
  
#   #  set outname = `printf "gen_ep_%06d.root" $i`
#     set outname = `printf "gen_%s_xyvtx_%06d.root" $pname $i`
  
#   #  root -b -q 'phparticlegen_pip.C(1000, "'"$outname"'")'
#   echo  'phparticlegen_pip.C(10000, '$pid', "'"$outname"'")'
#   root -b -q  'phparticlegen_pip.C(10000, '$pid', "'"$outname"'")'
  
#     @ i ++
#   end

#   @ idx ++
# end
