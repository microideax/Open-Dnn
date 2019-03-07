#ifndef _ACC_INSTANCE_H_
#define _ACC_INSTANCE_H_
#include "config.h"
#include "conv_acc_2ibuf.h"
#include "fc_acc_innerpp.h"
#include "max_pool_acc_innerpp.h"

using namespace std;
conv_acc< data_type_itf, Tparam, data_type, data_type_w, data_type_o, 32, 4, 8, 8,   5,    11,     32,        32,          32> convAcc0;
conv_acc< data_type_itf, Tparam, data_type, data_type_w, data_type_o, 32, 4, 8, 8,   5,    11,     32,        32,          32> convAcc1;
//																	  Tm, Tn,Tr,Tc,S_max,K_max, int IBUF_t, int WBUF_t, int OBUF_t>

void conv_layer_acc_0(
					Tparam N,
					Tparam K,
					Tparam M,
					Tparam R_IN,
					Tparam C_IN,
					Tparam C_OUT,
					Tparam R_OUT,
					Tparam S,
					Tparam P,
					Tparam act,
					Tparam inport,
					Tparam weight_offset,
					Tparam bias_offset,
					Tparam in_offset,
					Tparam out_offset,
					ap_fixed<32,26>* layer_bias,
					data_type_itf* i_weight,
					data_type_itf* i_data,
					data_type_itf* out_data
				)
{
#pragma HLS INTERFACE m_axi  port=i_data
#pragma HLS INTERFACE bram port=out_data
	convAcc0.conv_layer_acc_mbuf(N, K, M, R_IN, C_IN, C_OUT, R_OUT, S, P, act, inport, weight_offset, bias_offset, in_offset, out_offset,layer_bias,i_weight,i_data,out_data);
}



void conv_layer_acc_1(
					Tparam N,
					Tparam K,
					Tparam M,
					Tparam R_IN,
					Tparam C_IN,
					Tparam C_OUT,
					Tparam R_OUT,
					Tparam S,
					Tparam P,
					Tparam act,
					Tparam inport,
					Tparam weight_offset,
					Tparam bias_offset,
					Tparam in_offset,
					Tparam out_offset,
					ap_fixed<32,26>* layer_bias,
					data_type_itf* i_weight,
					data_type_itf* i_data,
					data_type_itf* out_data
				)
{
#pragma HLS INTERFACE bram port=i_data
#pragma HLS INTERFACE bram port=out_data
	convAcc1.conv_layer_acc_mbuf(N, K, M, R_IN, C_IN, C_OUT, R_OUT, S, P, act, inport, weight_offset, bias_offset, in_offset, out_offset,layer_bias,i_weight,i_data,out_data);
}

max_pool_acc< data_type_itf, data_type, data_type_w, data_type_o, 32, 16, 16, 2, 3> maxPoolAcc0;

void max_pool_layer_acc_0(
		Tparam R_in,
		Tparam C_in,
		Tparam N,
		Tparam K,
		Tparam R,
		Tparam C,
		Tparam S,
		Tparam P,
		Tparam act,
		Tparam i_offset,
		Tparam o_offset,
		data_type_itf* i_data,
		data_type_itf* o_data){
		    maxPoolAcc0.max_pool_layer_mbuf(R_in, C_in, N, K, R, C, S, P, act, i_offset, o_offset, i_data, o_data);
		};

max_pool_acc< data_type_itf, data_type, data_type_w, data_type_o, 16, 16, 16, 2, 3> maxPoolAcc1;

void max_pool_layer_acc_1(
		Tparam R_in,
		Tparam C_in,
		Tparam N,
		Tparam K,
		Tparam R,
		Tparam C,
		Tparam S,
		Tparam P,
		Tparam act,
		Tparam i_offset,
		Tparam o_offset,
		data_type_itf* i_data,
		data_type_itf* o_data){
		    maxPoolAcc1.max_pool_layer_mbuf(R_in, C_in, N, K, R, C, S, P, act, i_offset, o_offset, i_data, o_data);
		};

fc_acc< data_type_itf, Tparam, data_type, data_type_w, data_type_o, 32, 32,  5,  5> fcAcc0;

void fc_layer_acc_0(
		Tparam N,
		Tparam K,
		Tparam M,
		Tparam R_IN,
		Tparam C_IN,
		Tparam C_OUT,
		Tparam R_OUT,
		Tparam S,
		Tparam P,
		Tparam act,
		Tparam weight_offset,
		Tparam bias_offset,
		Tparam in_offset,
		Tparam out_offset,
		data_type_itf* layer_bias,
		data_type_itf* i_weight,
		data_type_itf* i_data,
		data_type_itf* out_data
		){
			fcAcc0.fc_layer_acc_mbuf(N, M, R_IN, C_IN, K, act,
					i_weight, layer_bias,
					weight_offset, bias_offset, in_offset, out_offset,
					i_data, out_data);
		};



void conv_pool_acc_0(
					Tparam* param_port,
					ap_fixed<32,26>* bias_in,
					data_type_itf* weight_in,
					data_type_itf* data_in,
					data_type_itf* data_out
	   	   	   	   )
{
	Tparam layer_num_local[16];
	Tparam param_conv_local[16];
	Tparam param_pool_local[16];

	for (unsigned int ll = 0; ll < 16; ll++)
	{
		layer_num_local[ll] = param_port[ll];
	}

	cout << "LAYER ACC: CONV Loading layer number for current accelerator ..." << endl;
	cout << "LAYRE ACC: CONV will process "<< layer_num_local[0] << " layers" <<endl;
	for (unsigned int l = 0; l < layer_num_local[0]; l++)
	{
		cout << "LAYER ACC: CONV Processing " << l << "th layer ..." << endl;
		for (unsigned int i = 0; i < 16; i++)
		{
			param_conv_local[i] = param_port[16 + l*16 + i];
			param_pool_local[i] = param_port[16 + l*16 + i];
		}
		if (param_pool_local[15] == 0)
		{
			cout << "LAYER ACC: Execute conv layer without pool, enable_pool =  " << param_pool_local[15] << endl;
			conv_layer_acc_0(param_conv_local[0], // N
							 param_conv_local[1], // K
							 param_conv_local[2], // M
							 param_conv_local[3], // Rin
							 param_conv_local[4], // Cin
							 param_conv_local[5], // R
							 param_conv_local[6], // C
							 param_conv_local[7], // S
							 param_conv_local[8], // P
							 param_conv_local[9], // act
							 param_conv_local[14], //inport    0:read data_in via AXI bus     1:read data_in via BARM port
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
			max_pool_layer_acc_0(param_pool_local[0], // R_in,
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
								data_out,
								data_out);
		}
	}
}



void conv_pool_acc_1(
					Tparam* param_port,
					ap_fixed<32,26>* bias_in,
					data_type_itf* weight_in,
					data_type_itf* data_in,
					data_type_itf* data_out
	   	   	   	   	   )
{
	Tparam layer_num_local[16];
	Tparam param_conv_local[16];
	Tparam param_pool_local[16];

	for (unsigned int ll = 0; ll < 16; ll++)
	{
		layer_num_local[ll] = param_port[ll];
	}

	cout << "LAYER ACC: CONV Loading layer number for current accelerator ..." << endl;
	cout << "LAYRE ACC: CONV will process "<< layer_num_local[0] << " layers" <<endl;
	for (unsigned int l = 0; l < layer_num_local[0]; l++)
	{
		cout << "LAYER ACC: CONV Processing " << l << "th layer ..." << endl;
		for (unsigned int i = 0; i < 16; i++)
		{
			param_conv_local[i] = param_port[16 + l*16 + i];
			param_pool_local[i] = param_port[16 + l*16 + i];
		}
		if (param_pool_local[15] == 0)
		{
			cout << "LAYER ACC: Execute conv layer without pool, enable_pool =  " << param_pool_local[15] << endl;
			conv_layer_acc_1(param_conv_local[0], // N
							 param_conv_local[1], // K
							 param_conv_local[2], // M
							 param_conv_local[3], // Rin
							 param_conv_local[4], // C
							 param_conv_local[5], // R
							 param_conv_local[6], // C
							 param_conv_local[7], // S
							 param_conv_local[8], // P
							 param_conv_local[9], // act
							 param_conv_local[14], //inport
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
			max_pool_layer_acc_1(param_pool_local[0], // R_in,
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
							   data_out,
							   data_out);
		}
	}
};

#endif
