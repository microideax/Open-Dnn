#ifndef CONV_VALIDATE_H
#define CONV_VALIDATE_H
#include "ap_fixed.h"

class conv_validate
{
public:
	int layer_num;
	int num_input;
	int num_output;
	int stride;
	int padding;
	int kernel_size;
	int inputfeature_size;
	int outputfeature_size;
	int act;
	ap_int<512> *weight;
	ap_int<512> *input_feature;
	ap_int<512> *output_feature;
	ap_int<512> *output_feature_software;
	ap_uint<32> lnum_list[16];
	ap_uint<32> config_list[16*16];
	ap_fixed<32,26>* bias;

	conv_validate(int layer_num, int num_input,int num_output,int kernel_size,int stride,int padding, int inputfeature_size);
	void print_weight(void);
	void print_feature_in(void);
	void print_bias(void);
	void print_feature_out(void);

	void software_conv_process(void);
	void print_feature_out_softeare(void);
	void test_fun(void);
};



#endif
