#!/bin/bash

for i in 0 1 2
do
    rm -rf sub_net_$i vivado_hls.log
done

echo "Cleaned existing projects!!!"

echo "Start generating sub-net IPs ..."

vivado_hls -f hls_script.tcl
#vivado_hls -f hls_script.tcl
