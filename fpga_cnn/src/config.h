
#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "ap_fixed.h"
#include "ap_int.h"


//extern int layer_count = 0;
//define data type
typedef ap_fixed<16,10> data_type;
typedef ap_fixed<16,10> data_type_w;
typedef ap_fixed<16,10> data_type_o;
typedef ap_fixed<32,26> b_type;

typedef unsigned int uint;

typedef ap_uint<32> Tparam;
// counter datatype used in the entire design
typedef ap_int<512> data_type_itf;
//typedef ap_fixed<16,0> param_type;

#define READ_LEN_i uint(sizeof(ap_int<512>)/sizeof(data_type))
#define READ_LEN_w uint(sizeof(ap_int<512>)/sizeof(data_type_w))
#define READ_LEN_o uint(sizeof(ap_int<512>)/sizeof(data_type_o))

#define DATA_LEN uint(sizeof(data_type))
#define DATA_O_LEN int(sizeof(data_type_itf)/sizeof(data_type_o))
#define DATA_O int(sizeof(data_type_o))

// C++ compilation debug mode
#define _LAYER_MODE_ 1

#ifdef _LAYER_MODE_
#define IBUF (Tr-1)*S_max + K_max
//#define
#endif

// C++ compilation debug mode
//#ifdef _LAYER_MODE_
//#define _ACC_MODE_ 0
//#else
//#define _ACC_MODE_ 1
//#endif

#define _HLS_MODE_  1

//#define _BATCH_MODE_ 1
#ifdef _BATCH_MODE_
#define _KERNEL_DEBUG_ 0
#else
#define _KERNEL_DEBUG_ 1
#endif
#ifndef _HLS_MODE_
#define _C_DEBUG_MODE_ 1
#endif

#define _8CH_ 1


//network configuration PARAMETERS
int weight_bias_record = 0;
int weight_bias_count_1 = 0;
int weight_bias_count_2 = 0;
int nn_in_data_size_conv[2] = {28, 14};
int nn_in_number_conv[2] = {1, 6};
int nn_out_number_conv[2] = {6, 16};
int nn_channel_size_conv[2] = {5, 5};
int nn_padding_conv[2] = {2, 0};
int nn_group_conv[2] = {1, 1};
int nn_in_data_size_pooling[2] = {28, 10};
int nn_in_number_pooling[2] = {6, 16};
int nn_channel_size_pooling[2] = {2, 2};
int nn_in_data_size_fc[1] = {5};
int nn_in_number_fc[1] = {16};
int nn_out_number_fc[1] = {10};
int nn_channel_size_fc[1] = {5};


#endif
