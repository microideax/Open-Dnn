## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 2015 Xilinx Inc. All rights reserved.
############################################################
open_project inf_net_0_sim

set_top inf_net_0

#add_files ../inference_net/activation_functions.h
add_files ../inference_net/config.h
add_files ../inference_net/construct_net.h
add_files ../inference_net/conv_acc_innerpp.h
add_files ../inference_net/max_pool_acc_innerpp.h
add_files ../inference_net/data_type.h
add_files ../inference_net/acc_instance.h
add_files ../inference_net/data_type.h

add_files -tb ../inference_net/image_converter.h
add_files -tb ../inference_net/resize_image.h
add_files -tb ../inference_net/accuracy_one_dim.h
add_files -tb ../inference_net/predict_one_dim.h
add_files -tb ../inference_net/softmax_one_dim.h
add_files -tb ../inference_net/weight_bias_one_dim.h
add_files -tb ../inference_net/stb_image/stb_image.h
add_files -tb ../inference_net/stb_image/stb_image_resize.h
add_files -tb ../inference_net/stb_image/stb_image_write.h

add_files ../ff_test.cpp
add_files -tb ../net_inputs/test_imgs/3.bmp
add_files -tb ../net_inputs/net_weights.txt
add_files -tb ../net_inputs/val.txt
add_files -tb ../net_inputs/net_mean.txt

add_files -cflags "-I/nfs/app/Xilinx/Vivado_HLS/2017.1/include -std=c++0x -fpermissive -pedantic -Wall -Wextra" -tb ../ff_test.cpp


open_solution -reset "inference_IP"
#vc709
#set_part {xc7vx690tffg1761-2}
#zynq-7000 board
#set_part {xc7z045ffg900-1} 
# zedboard
#set_part {xc7z020clg484-1}  
# UltraScale+ 
set_part {xcvu9p-flgb2104-2-i}
create_clock -period 5 -name default
config_interface -m_axi_addr64 -m_axi_offset off -register_io off

csim_design -clean -argv {net_weights.txt, 3.bmp, val.txt, net_mean.txt}

csynth_design

#export_design -flow syn -rtl verilog -format ip_catalog

cosim_design -argv {net_weights.txt, 3.bmp, val.txt, net_mean.txt} -trace_level none -rtl verilog -tool xsim

exit
