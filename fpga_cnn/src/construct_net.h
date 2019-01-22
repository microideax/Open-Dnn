// param_conv[] = { N, K, M, Rin, Cin, R, C, S, P, act, w_off, b_off, i_off, o_off, 0, 0}
#ifndef _CONSTRUCT_NET_H_
#define _CONSTRUCT_NET_H_

//#include "/opt/Xilinx/Vivado/2017.4/include/mpfr.h"
//#include "/opt/Xilinx/Vivado/2017.4/include/gmp.h"

#include "config.h"
#include <iostream>
#include <ap_fixed.h>
#include "acc_instance.h"


using namespace std;

void inf_net_1(
    Tparam layer_num[16],
    Tparam param_conv[16*16],
    Tparam param_pool[16*16],
	ap_fixed<32,26> bias_in[1024],
    data_type_itf weight_in[2048],
    data_type_itf data_in[1024],
    data_type_itf data_out[1024]
   )
{

//5242880
#pragma HLS INTERFACE s_axilite port=return bundle=CRTL_BUS

#pragma HLS INTERFACE bram  port=layer_num
#pragma HLS INTERFACE bram  port=param_conv
#pragma HLS INTERFACE bram  port=param_pool

#pragma HLS INTERFACE bram  port=bias_in depth=1024

#pragma HLS INTERFACE m_axi port=weight_in offset=slave depth=2048 bundle=WEIGHT_IN
#pragma HLS INTERFACE m_axi port=data_in   offset=slave depth=1024 bundle=DATA_IN
#pragma HLS INTERFACE m_axi port=data_out  offset=slave depth=1024 bundle=DATA_OUT

#pragma HLS INTERFACE s_axilite port=weight_in bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=data_in bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=data_out bundle=CRTL_BUS

    Tparam layer_num_local[16];
    Tparam param_conv_local[16];
    Tparam param_pool_local[16];

#pragma HLS ARRAY_PARTITION variable = param_conv_local complete
#pragma HLS ARRAY_PARTITION variable = param_pool_local complete

    for (unsigned int ll = 0; ll < 16; ll++)
    {
#pragma HLS PIPELINE
        layer_num_local[ll] = layer_num[ll];
    }
    cout << "LAYER ACC: CONV Loading layer number for current accelerator ..." << endl;
    cout << "LAYRE ACC: CONV will process "<< layer_num_local[0] << " layers" <<endl;
    for (unsigned int l = 0; l < layer_num_local[0]; l++) {
    	cout << "LAYER ACC: CONV Processing " << l << "th layer ..." << endl;
        for (unsigned int i = 0; i < 16; i++)
        {
#pragma HLS PIPELINE
            param_conv_local[i] = param_conv[l*16 + i];
            param_pool_local[i] = param_pool[l*16 + i];
        }
        if (param_pool_local[15] == 0)
        {
            cout << "LAYER ACC: Execute conv layer without pool, enable_pool =  " << param_pool_local[15] << endl;
            conv_layer_acc_1(param_conv_local[0], // N
                         param_conv_local[1], // K
                         param_conv_local[2], // M
                         param_conv_local[3], // Rin
                         param_conv_local[4], // Cin
                         param_conv_local[5], // R
                         param_conv_local[6], // C
                         param_conv_local[7], // S
                         param_conv_local[8], // P
                         param_conv_local[9], // act
                         param_conv_local[10], // w_offset
                         param_conv_local[11], // b_offset
                         param_conv_local[12], // in_offset
                         param_conv_local[13], // out_offset
                         bias_in,
                         weight_in,
                         data_in,
                         data_out);
        }
        else
        {
            cout << "LAYER ACC: POOL Execute pool layer with pooling enable: " << param_pool_local[15] << endl;
            max_pool_layer_new(param_pool_local[0], // R_in,
                           param_pool_local[1], // C_in
                           param_pool_local[2], // N
                           param_pool_local[3], // K
                           param_pool_local[4], // R
                           param_pool_local[5], // C
                           param_pool_local[6], // S
                           param_pool_local[7], // P
                           param_pool_local[8], // act
                           param_pool_local[9], // in_offset
                           param_pool_local[10], // out_offset
                           data_in,
                           data_out);
        }
    }
};


void inf_net_2(
    Tparam layer_num[16],
    Tparam param_conv[16*16],
    Tparam param_pool[16*16],
	data_type_itf bias_in[32*4],
    data_type_itf weight_in[128*4096],
    data_type_itf data_in[128],
    data_type_itf data_out[128]
   )
{

#pragma HLS INTERFACE s_axilite port=return bundle=CRTL_BUS

#pragma HLS INTERFACE bram  port=layer_num
#pragma HLS INTERFACE bram  port=param_conv
#pragma HLS INTERFACE bram  port=param_pool

#pragma HLS INTERFACE bram  port=bias_in depth=32

#pragma HLS INTERFACE m_axi port=weight_in offset=slave depth=1024 bundle=WEIGHT_IN
#pragma HLS INTERFACE m_axi port=data_in   offset=slave depth=1024 bundle=DATA_IN
#pragma HLS INTERFACE m_axi port=data_out  offset=slave depth=1024 bundle=DATA_OUT

#pragma HLS INTERFACE s_axilite port=weight_in bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=data_in bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=data_out bundle=CRTL_BUS

    Tparam layer_num_local[16];
    Tparam param_conv_local[16];
    Tparam param_pool_local[16];

#pragma HLS ARRAY_PARTITION variable = param_conv_local complete
#pragma HLS ARRAY_PARTITION variable = param_pool_local complete

    for (unsigned int ll = 0; ll < 16; ll++)
    {
#pragma HLS PIPELINE
        layer_num_local[ll] = layer_num[ll];
    }
    cout << "LAYER ACC: FC Loaded layer number "<< layer_num_local[0]<<" for current accelerator ..." << endl;
    for (unsigned int l = 0; l < layer_num_local[0]; l++) {
    	cout << "LAYER ACC: FC Processing " << l << "th layer ..." << endl;
        for (unsigned int i = 0; i < 16; i++)
        {
#pragma HLS PIPELINE
            param_conv_local[i] = param_conv[l*16 + i];
            param_pool_local[i] = param_pool[l*16 + i];
        }
        fc_layer_acc_1(param_conv_local[0], // N
                         param_conv_local[1], // K
                         param_conv_local[2], // M
                         param_conv_local[3], // Rin
                         param_conv_local[4], // Cin
                         param_conv_local[5], // R
                         param_conv_local[6], // C
                         param_conv_local[7], // S
                         param_conv_local[8], // P
                         param_conv_local[9], // act
                         param_conv_local[10], // w_offset
                         param_conv_local[11], // b_offset
                         param_conv_local[12], // in_offset
                         param_conv_local[13], // out_offset
                         bias_in,
                         weight_in,
                         data_in,
                         data_out);
    }
};


#endif //_CONSTRUCT_NET_H_
