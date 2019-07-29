#!/usr/local/bin/tcsh -f

source /opt/phenix/bin/phenix_setup.csh
setenv LD_LIBRARY_PATH /phenix/hhj/hachiya/15.08/source/PHParticleGen/install/lib/:$LD_LIBRARY_PATH

echo $LD_LIBRARY_PATH

set max = 97

#set pname = "em"
#set pid   = "11"

#set pnamel = ("em" "pip" "pim")
#set pidl   = ("11" "211" "-211")
set pnamel = ("pip")
set pidl   = ("211")

@ idx = 1
foreach pname ($pnamel)
  set pid = $pidl[$idx]
  echo "$pname, $pid"
  
  @ i = 96
  while ( ${i} < ${max} )
  
  #  set outname = `printf "gen_ep_%06d.root" $i`
    set outname = `printf "gen_%s_xyvtx_%06d.root" $pname $i`
  
  #  root -b -q 'phparticlegen_pip.C(1000, "'"$outname"'")'
  echo  'phparticlegen_pip.C(10000, '$pid', "'"$outname"'")'
  root -b -q  'phparticlegen_pip.C(10000, '$pid', "'"$outname"'")'
  
    @ i ++
  end

  @ idx ++
end
