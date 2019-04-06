#!/bin/bash

while getopts i:k:h: option
do
    case "${option}"
        in
        i) INFILE=${OPTARG};;
        h) HELP=1;;
    esac
done 

#if HELP==1
#    echo "./run_generator.sh -i input.prototxt to execute the generation!"
#fi

echo $INFILE
#mkdir -p ../gen_proj
#mkdir -p ../gen_proj/hls_proj/src
#mkdir -p ../gen_proj/hls_proj/testbench
#mkdir -p ../gen_proj/impl_proj/

echo "script executed!!!"
#--------------1.param extract-----------------------
python3.5 paramExtractor/extract.py --model $INFILE
mv net_config_params.txt dse/
#--------------2.design space exploration------------
python3.5 dse/tm_tn_multiAcc.py dse/net_config_params.txt
mv acc_ins_params.txt netGen/
#--------------3.code generation---------------------
python3.5 netGen/generate_accInst.py acc_ins_param.txt
python3.5 netGen/generate_consNet.py acc_ins_param.txt


#cp hls_script.tcl to hls_proj/
#cp impl_script.tcl to impl_proj/

exit
