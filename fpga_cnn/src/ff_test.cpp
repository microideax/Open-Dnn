

#include <iostream>
#include <stdlib.h>
//#include "/opt/Xilinx/Vivado/2017.4/include/gmp.h"

#include "ap_fixed.h"
#include "construct_net.h"

#include "conv_validate.h"
#include "fc_validate.h"


using namespace std;


int main()
{

	cout <<"Test Begin..."<<endl;
//    						       N,  K,  M,  Rin, Cin,  R,  C,  S, P, act
    //ap_uint<32> param_conv[16] = { 34,  5,  3,  27,  27,  27,  27, 1, 2,  1, 0, 0, 0, 0, 0, 0};
//								  Rin,Cin, N , K ,  R,  C,  S, P ,act,in_off,out_off
    ap_uint<32> param_pool[16*16] = { 28, 28, 6,  2, 14, 14, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0};

//
//    ap_int<512> feature_in[3000] = {0};
//    ap_int<512> feature_out[1000]= {0};
//    ap_int<512> weight[4096]={0};
//    ap_fixed<32,26> bias[100] = {0};


    conv_validate conv(1,1,6,5,1,2,28);
    fc_validate fc(1, 4096, 4096, 1);

//    conv.print_weight();
//    conv.print_feature_in();
//    conv.print_bias();
//
    ap_uint<32> l_list[16] = {1, 5, 6, 28, 28, 28, 28, 1, 2, 1, 0, 0, 0, 0, 0, 0};
    ap_uint<32> list[16*16] = {1024, 5, 32, 28, 28, 28, 28, 1, 2, 1, 0, 0, 0, 0, 0, 0};
    ap_int<512> weight[2048];
    ap_fixed<32, 26> bias[1024];
    ap_int<512> fc_bias[32];
    ap_int<512> feature_in[1024];
    ap_int<512> feature_out[1024];

//    inf_net_1(
//    	conv.lnum_list,
//    	conv.config_list,
//		param_pool,
//        conv.bias,
//		conv.weight,
//		conv.input_feature,
//		conv.output_feature
//    );

//    inf_net_1(
//    	l_list,
//    	list,
//		param_pool,
//        bias,
//		weight,
//		feature_in,
//		feature_out
//    );

//    conv.print_feature_in();
//   conv.print_feature_out();
//   conv.software_conv_process();
//   conv.print_feature_out_softeare();


    inf_net_2(
    	fc.lnum_list,
		fc.config_list,
		param_pool,
		fc.bias,
		fc.weight,
		fc.in_feature,
		fc.out_feature
    );

//    inf_net_2(
//    	l_list,
//		list,
//		list,
//		fc_bias,
//		weight,
//		feature_in,
//		feature_out
//    );
////
//    fc.print_feature_in();
//    fc.print_weight();
//    fc.print_bias();
//    fc.print_feature_out();
//    fc.software_fc_process();
//    fc.print_software_out();

   return 0;

}










//void software_validate(ap_int<512> weight[11616],ap_int<512> feature_in[9],ap_int<512> feature_out[1024])
//{
//	float temp;
//	int i,j,k,x,y;
//
//
//	for(k = 0 ; k < 96; k++)
//		for(x = 0 ; x < 6; x++)
//			for(y = 0 ; y < 6; y++)
//			{
//				temp = 0;
//				for(i = 0 ; i < 11; i++)
//					for(j = 0 ; j < 11; j++)
//					{
//						temp = temp + ((float)(feature_in[(x * 16 + y)+(i*16+j)].range(15,0)) / 64) * ((float)(weight[k * 11 * 11 + i * 11 + j].range(15,0)) / 64);
//	//					cout <<"i = " << i << "        j = "<< j <<"        feature = " << ((float)(feature_in[i*16+j].range(15,0)) / 64)
//	//						 <<"weight = " << ((float)(weight[i*11+j].range(15,0)) / 64) <<"        temp = " <<temp << endl;
//					}
//
//				feature_out[k / 32 * 6 * 6 + x * 6 + y].range(15+16*(k%32),16*(k%32)) = temp * 64;
//
//				//cout <<"x = " << x <<"        y = "<< y <<"        temp = " << temp << endl;
//			}
//
//
//
//}


//void software_validate_pooling(ap_int<512> pooling_in[9999],ap_int<512> pooling_out[4096])
//{
//	int i,j,k,x,y,z;
//	short int temp_array[9];
//	short int temp_max;
//	for(k = 0 ; k < 50; k++)
//	{
//		for(i = 0 ; i < 55 - 2; i+=2)
//		{
//			for(j = 0 ; j < 55 - 2; j+=2)
//			{
//				for(x = 0 ; x < 3; x++)
//					for(y = 0 ; y < 3; y++)
//						temp_array[x*3+y] = pooling_in[(k/32)*55*55+i*55+j+(x*55+y)].range((k%32)*16+15,(k%32)*16);
//				temp_max = temp_array[0];
//				for(z = 1 ; z < 9; z++)
//					if(temp_array[z] > temp_max)
//						temp_max = temp_array[z];
//				pooling_out[(k/32) * 27 * 27 + (i/2) * 27 + (j/2)].range((k%32)*16+15,(k%32)*16)= (temp_max>>6);
//			}
//		}
//	}
//}


