#ifndef POOLING_VALIDATE_H
#define POOLING_VALIDATE_H

#include "/opt/Xilinx/Vivado/2018.1/include/gmp.h"
//#include "/opt/Xilinx/Vivado/2018.1/include/mpfr.h"
#include "ap_fixed.h"

class pooling_validate
{
public:
	int num_input;
	int stride;
	int kernel_size;
	int inputfeature_size;
	int outputfeature_size;
	int act;
	
	ap_int<512> *input_feature;
	ap_int<512> *output_feature;
	ap_int<512> *output_feature_software;
	ap_uint<32> config_list[16];
	
	pooling_validate(int num_input,int stride,int kernel_size,int inputfeature_size,int act);
	void print_feature_in(void);
};

#endif
