//#include "/opt/Xilinx/Vivado/2018.1/include/gmp.h"
#include <iostream>
#include "ap_fixed.h"
#include "construct_net.h"
#include "../testbench/conv_validate.h"
#include "../testbench/pooling_validate.h"
#include "../testbench/fc_validate.h"
using namespace std;
void construct_para(ap_uint<32>* para);
void print_para_list(ap_uint<32>* para);

int main()
{
	int i,j;
	cout <<"Test Begin..."<<endl;

	ap_uint<32> para_list[512];
	ap_int<512> data_temp[2048];

	for(i = 0 ; i < 512; i++)
		para_list[i] = 0;
	for(i = 0 ; i < 2048; i++)
		data_temp[i] = 0;

	construct_para(para_list);
	print_para_list(para_list);
	conv_validate conv_test(para_list);


	sub_net_1(
					para_list,						//ap_uint<32> param_port[528],
					conv_test.bias,					//ap_fixed<32,26> bias_in[1024],
					conv_test.weight,				//data_type_itf weight_in[2048],
					conv_test.input_feature,	    //data_type_itf data_in_0[2048],
					data_temp,						//data_type_itf data_out_0[2048],
					data_temp,						//data_type_itf data_in_1[2048],
					conv_test.output_feature,	    //data_type_itf data_out_1[2048],
					0								//int select
				);
	//conv_test.print_feature_in();
//	for(i = 0 ; i < 16; i++)
//	{
//		for(j = 0 ; j < 16; j++)
//			cout << setw(12) <<  float(data_temp[i*16+j].range(15,0))/64 <<" ";
//		cout << endl;
//	}
	/*
	sub_net_0	(
					para_list,						//ap_uint<32> param_port[528],
					conv_test.bias,					//ap_fixed<32,26> bias_in[1024],
					conv_test.weight,				//data_type_itf weight_in[2048],
					conv_test.input_feature,	    //data_type_itf data_in_0[2048],
					data_temp,						//data_type_itf data_out_0[2048],
					data_temp,						//data_type_itf data_in_1[2048],
					conv_test.output_feature,	    //data_type_itf data_out_1[2048],
					1								//int select
				);
	*/
//	for(i = 0 ; i < 16; i++)
//	{
//		for(j = 0 ; j < 16; j++)
//			cout << setw(12) << float(conv_test.output_feature[i*16+j].range(15,0))/64 <<" ";
//		cout << endl;
//	}
	cout <<"Test Finish"<<endl;
	return 0;
}


void construct_para(ap_uint<32>* para)
{
	  int i;
	  //0-1.layer_num
	  para[0] = 1;
	  //0-2.conv para
	  para[16+0] = 1;//N
	  para[16+1] = 3;//K
	  para[16+2] = 1;//M
	  para[16+3] = 16;//Rin
	  para[16+4] = 16;//Cin
	  para[16+5] = 16;//R
	  para[16+6] = 16;//C
	  para[16+7] = 1;//S
	  para[16+8] = 1;//P
	  para[16+9] = 1;//act
	  para[16+10] = 0;//weight_offset
	  para[16+11] = 0;//bias_offset
	  para[16+12] = 0;//in_offset
	  para[16+13] = 0;//out_offset
	  para[16+14] = 0;//inport
	  para[16+15] = 0;
	  //0-3.conv para
	  para[32+0] = 1;//N
	  para[32+1] = 3;//K
	  para[32+2] = 1;//M
	  para[32+3] = 16;//Rin
	  para[32+4] = 16;//Cin
	  para[32+5] = 16;//R
	  para[32+6] = 16;//C
	  para[32+7] = 1;//S
	  para[32+8] = 1;//P
	  para[32+9] = 1;//act
	  para[32+10] = 0;//weight_offset
	  para[32+11] = 0;//bias_offset
	  para[32+12] = 0;//in_offset
	  para[32+13] = 512;//out_offset
	  para[32+14] = 1;//inport
	  para[32+15] = 0;
	  //0-4.conv para
	  para[48+0] = 1;//N
	  para[48+1] = 3;//K
	  para[48+2] = 1;//M
	  para[48+3] = 16;//Rin
	  para[48+4] = 16;//Cin
	  para[48+5] = 16;//R
	  para[48+6] = 16;//C
	  para[48+7] = 1;//S
	  para[48+8] = 1;//P
	  para[48+9] = 1;//act
	  para[48+10] = 0;//weight_offset
	  para[48+11] = 0;//bias_offset
	  para[48+12] = 512;//in_offset
	  para[48+13] = 0;//out_offset
	  para[48+14] = 1;//inport
	  para[48+15] = 0;





	  //1-1.layer_num
	  para[256+0] = 3;
	  //1-2.conv_para
	  para[256+16+0] = 1; //N
	  para[256+16+1] = 3; //K
	  para[256+16+2] = 1; //M
	  para[256+16+3] = 16; //Rin
	  para[256+16+4] = 16; //Cin
	  para[256+16+5] = 16;  //R
	  para[256+16+6] = 16;  //C
	  para[256+16+7] = 1;  //S
	  para[256+16+8] = 1;  //P
	  para[256+16+9] = 1;  //act
	  para[256+16+10] = 0; //weight_offset
	  para[256+16+11] = 0; //bias_offset
	  para[256+16+12] = 0; //in_offset
	  para[256+16+13] = 0; //out_offset
	  para[256+16+14] = 0; //inport
	  para[256+16+15] = 0;

	  //1-3.conv_para
	  para[256+32+0] = 1; //N
	  para[256+32+1] = 3; //K
	  para[256+32+2] = 1; //M
	  para[256+32+3] = 16; //Rin
	  para[256+32+4] = 16; //Cin
	  para[256+32+5] = 16;  //R
	  para[256+32+6] = 16;  //C
	  para[256+32+7] = 1;  //S
	  para[256+32+8] = 1;  //P
	  para[256+32+9] = 1;  //act
	  para[256+32+10] = 0; //weight_offset
	  para[256+32+11] = 0; //bias_offset
	  para[256+32+12] = 0; //in_offset
	  para[256+32+13] = 512; //out_offset
	  para[256+32+14] = 1; //inport
	  para[256+32+15] = 0;

	  //1-4.conv_para
	  para[256+48+0] = 1; //N
	  para[256+48+1] = 3; //K
	  para[256+48+2] = 1; //M
	  para[256+48+3] = 16; //Rin
	  para[256+48+4] = 16; //Cin
	  para[256+48+5] = 16;  //R
	  para[256+48+6] = 16;  //C
	  para[256+48+7] = 1;  //S
	  para[256+48+8] = 1;  //P
	  para[256+48+9] = 1;  //act
	  para[256+48+10] = 0; //weight_offset
	  para[256+48+11] = 0; //bias_offset
	  para[256+48+12] = 512; //in_offset
	  para[256+48+13] = 0; //out_offset
	  para[256+48+14] = 1; //inport
	  para[256+48+15] = 0;






}
void print_para_list(ap_uint<32>* para)
{
	int i,j;

	for(i = 0 ; i < 32; i++)
	{
		for(j = 0 ; j < 16; j++)
			cout << para[i*16+j] <<" ";
		cout << endl;
	}
}
