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

./clean.sh

echo $INFILE
mkdir -p ../gen_proj
mkdir -p ../gen_proj/hls_proj/src
mkdir -p ../gen_proj/hls_proj/testbench
mkdir -p ../gen_proj/impl_proj/

echo "script executed!!!"
#--------------1.param extract-----------------------
python3.5 paramExtractor/extract.py --model $INFILE
mv net_config_params.txt dse/
echo "Finished network parameter extraction."
echo " "
#--------------2.design space exploration------------
python3.5 dse/tm_tn_multiAcc.py dse/net_config_params.txt
mv acc_ins_params.txt netGen/
echo "Finished accelerator design space exploration."
echo " "
#--------------3.code generation---------------------
python3.5 netGen/generate_accInst.py --params netGen/acc_ins_params.txt
python3.5 netGen/generate_consNet.py --params netGen/acc_ins_params.txt
echo "Finished accelerators and sub-nets generation."
echo "Constructing the testing and implementation folder..."
#TODO: move all the files into the correct positions, src/testbench/
cp ../fpga_cnn/src/* ../gen_proj/hls_proj/src/
cp ../fpga_cnn/testbench/* ../gen_proj/hls_proj/testbench/
mv *.h ../gen_proj/hls_proj/src/

cp ../scripts/hls_impl/hls_script.tcl ../gen_proj/hls_proj/
cp ../scripts/sys_gen/impl_script.tcl ../gen_proj/impl_proj/

echo "Files copied"
echo "Generation done!!!"

exit
