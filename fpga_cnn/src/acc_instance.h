#ifndef _ACC_INSTANCE_H_
#define _ACC_INSTANCE_H_

#include "config.h"
#include "conv_acc_innerpp.h"
#include "fc_acc_innerpp.h"
#include "max_pool_acc_innerpp.h"

using namespace std;

conv_acc<data_type_itf, Tparam, data_type, data_type_w, data_type_o, 32, 8,   8,   8,    5,     5, 32, 32, 32> convAcc1;
//													                 Tm   Tn    Tr   Tc   S_max  K_max
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
    convAcc1.conv_layer_acc_mbuf(N, K, M, R_IN, C_IN, C_OUT, R_OUT, S, P, act,
                                 weight_offset, bias_offset, in_offset, out_offset,
                                 layer_bias,
                                 i_weight,
                                 i_data,
                                 out_data);
};

// fc layer accelerator initialization
fc_acc<data_type_itf, Tparam, data_type, data_type_w, data_type_o, 32, 32,  5,  5> fcAcc1;

void fc_layer_acc_1(
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
	fcAcc1.fc_layer_acc_mbuf(N, M, R_IN, C_IN, K, act,
			i_weight, layer_bias,
			weight_offset, bias_offset, in_offset, out_offset,
			i_data, out_data);
};

//													                 Tm   Tn    Tr   Tc   S_max  K_max
//void conv_layer_acc_2(
//        Tparam N,
//        Tparam K,
//        Tparam M,
//        Tparam R_IN,
//        Tparam C_IN,
//        Tparam C_OUT,
//        Tparam R_OUT,
//        Tparam S,
//        Tparam P,
//        Tparam act,
//        Tparam weight_offset,
//        Tparam bias_offset,
//        Tparam in_offset,
//        Tparam out_offset,
//		ap_fixed<32,26>* layer_bias,
//        data_type_itf* i_weight,
//        data_type_itf* i_data,
//        data_type_itf* out_data
//)
//{
//    convAcc1.conv_layer_acc_mbuf(N, K, M, R_IN, C_IN, C_OUT, R_OUT, S, P, act,
//                                 weight_offset, bias_offset, in_offset, out_offset,
//                                 layer_bias,
//                                 i_weight,
//                                 i_data,
//                                 out_data);
//};


max_pool_acc<data_type_itf, data_type, data_type_w, data_type_o, 32, 16, 16, 2, 3> maxPoolAcc1;

void max_pool_layer_new(
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
        data_type_itf* o_data)
{
    maxPoolAcc1.max_pool_layer_mbuf(R_in, C_in, N, K, R, C, S, P, act, i_offset, o_offset, i_data, o_data);
};
#endif
