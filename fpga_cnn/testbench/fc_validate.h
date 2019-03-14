#ifndef FC_VALIDATE_H
#define FC_VALIDATE_H
#include "/opt/Xilinx/Vivado/2018.1/include/gmp.h"
#include "/opt/Xilinx/Vivado/2018.1/include/mpfr.h"
#include "ap_fixed.h"

class fc_validate
{
public:
	int layer_num;
	int num_input;
	int num_output;
	int act;

//	ap_int<512> weight[1024];
//	ap_int<512> in_feature[1024];
//	ap_int<512> out_feature[1024];
//	ap_int<512> out_feature_software[1024];
//	ap_int<512> bias[32];

	ap_int<512> *weight;
	ap_int<512> *in_feature;
	ap_int<512> *out_feature;
	ap_int<512> *out_feature_software;
	ap_int<512> bias[4096];

    ap_uint<32> lnum_list[16];
    ap_uint<32> config_list[16*16];

    fc_validate(int layer_num, int num_input, int num_output, int act);
    void print_weight(void);
    void print_feature_in(void);
    void print_bias(void);
    void print_feature_out(void);

	void software_fc_process(void);
	void print_software_out(void);
};

#endif
