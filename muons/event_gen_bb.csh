#!/bin/csh
echo "before"
env
source /opt/phenix/bin/phenix_setup.csh 
setenv LD_LIBRARY_PATH /gpfs02/phenix/fvtx/subsys/fvtx/ajeeta/install/lib:${LD_LIBRARY_PATH}
setenv LD_LIBRARY_PATH /gpfs02/phenix/fvtx/subsys/fvtx/ajeeta/PHG3toG4install/lib:${LD_LIBRARY_PATH}
setenv LD_LIBRARY_PATH /phenix/u/slash/workarea/offline/analysis/snowball/geant4_vmc/lib/tgt_linux:${LD_LIBRARY_PATH}
setenv LD_LIBRARY_PATH /phenix/u/slash/workarea/offline/analysis/snowball/vgm-4.01/vgm-build/lib:${LD_LIBRARY_PATH}
#setenv LD_LIBRARY_PATH /phenix/u/slash/hl/install/lib:${LD_LIBRARY_PATH}
#setenv LD_LIBRARY_PATH /phenix/u/slash/workarea/offline/AnalysisTrain/picoDST_object/install/lib:${LD_LIBRARY_PATH}
echo "after lib path set"
env
# -mxp-
# set localdir = /gpfs02/phenix/fvtx/subsys/fvtx/ajeeta/Simulation
set localdir = `pwd`

mkdir ${localdir}/PISA
set prefix = bb
#set nEvts = 2000
set nEvts = 10000000
set iter = 0
mkdir ${localdir}/pythia_files_${prefix}
mkdir ${localdir}/PISA/PISAEvent_${prefix}
mkdir ${localdir}/DST_sim_${prefix}
mkdir ${localdir}/DST_embed_${prefix}
mkdir ${localdir}/pDST_${prefix}
set skip = 0
set dataset = MB${skip}
set realdatafile=`head -n${1} realdataBG-run15pp_file.list | tail -n1`
#set realdatafile=`head -n${1} CorruptFiles_bb.txt | tail -n1`
echo realdatafile: ${realdatafile}
set run=`echo ${realdatafile} | cut -d_ -f3 | cut -d. -f1`
echo run: ${run}
set runnumber=`echo ${run} | cut -d- -f1 | sed s/0000//`
echo runnumber: ${runnumber}
set pythia_file=${localdir}/pythia_files_${prefix}/pythia_${prefix}-${dataset}-${run}-${iter}.root
set norm_file=${localdir}/pythia_files_${prefix}/normalization_${prefix}-${dataset}-${run}-${iter}.root
set pisafile=${localdir}/PISA/PISAEvent_${prefix}/PISAEvent_${prefix}-${dataset}-${run}-${iter}.root
set dstfile=${localdir}/DST_sim_${prefix}/dst_sim_${prefix}-${dataset}-${run}-${iter}.root
set dstembed=${localdir}/DST_embed_${prefix}/dst_embed_${prefix}-${dataset}-${run}-${iter}.root
set pdstfile=${localdir}/pDST_${prefix}/singlemuon_${prefix}_embed_pdst-${dataset}-${run}-${iter}.root
#if ( -e ${pisafile} ) exit
cd ${localdir}/PISA
mkdir ${localdir}/PISA/${1}
cd ${localdir}/PISA/${1}
ln -sf ${localdir}/g3tog4_${prefix}.C ${localdir}/PISA/${1}/
ln -sf ${localdir}/geom_run15_v2.root geom.root
ln -sf ${localdir}/Sim3D++.root ${localdir}/PISA/${1}/
ln -sf ${localdir}/g4libs.C ${localdir}/PISA/${1}/
ln -sf ${localdir}/pythia_configuration/phpythia8_${prefix}.cfg ${localdir}/PISA/${1}/
#root -b -q g3tog4_${prefix}.C\(${nEvts},${skip},${runnumber},\"${realdatafile}\",\"${pythia_file}\",\"${norm_file}\"\)
#mv -f PISAEvent.root ${pisafile}
cd ${localdir}
#root -b -q Fun4FVTX_Pisa.C\(${nEvts},\"${pisafile}\",\"${pythia_file}\",${runnumber},\"${dstfile}\",\"\",\"\",\"\",\"\"\)
root -b -q Fun4FVTX_RecoDST_sim_reass.C\(${nEvts},\"${dstfile}\",\"${realdatafile}\",\"${pythia_file}\",${runnumber},\"\",\"${dstembed}\",\"\",\"\",\"${pdstfile}\"\)
