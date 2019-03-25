#include <iostream>
#include <math.h>
#include <stdlib.h>
#include "conv_validate.h"
using namespace std;


conv_validate::conv_validate(ap_uint<32>* param_list)
{
	int i,j,k;
	int input_num;
	int input_feature_size;
	int output_num;

	this->param_list = param_list;
	input_num = param_list[16+0];
	input_feature_size = param_list[16+3];

	layer_num = param_list[0];
	for(i = 0 ; i < 4096; i++)
		input_feature[i] = 0;
	for(i = 0 ; i < 2048; i++)
		weight[i] = 0;
	for(i = 0 ; i < 4096; i++)
		output_feature[i] = 0;
	for(i = 0 ; i < 1024; i++)
		bias[i] = 0;


	for(i = 0 ; i < 1;i++)
	{
		for(j = 0 ; j < 16 * 16;j++)
			input_feature[j].range(15+16*i,16*i) = 64;
	}




	for(i = 0 ; i < 9;i++)
	{
		weight[i].range(15,0) = 16;
	}


	for(i = 0 ; i < 9;i++)
	{
		weight[i].range(16+15,16+0) = 0;
	}

	for(i = 0 ; i < 9;i++)
	{
		weight[i].range(16*2+15,16*2+0) = 0;
	}




//	for(j = 0 ; j < 8; j++)
//		for(i = 0 ; i < 72;i++)
//		{
//			weight[i].range(15+16*j,0+16*j) = 64;
//		}



//	for(i = 18 ; i < 27;i++)
//	{
//		weight[i].range(15,0) = 64;
//	}
}




void conv_validate :: print_feature_in(void)
{
	int i,j;
	cout << "input feature:" << endl;
	for(i = 0 ; i <12*12; i++)
	{
		cout <<i<<":";
		for(j = 0 ; j < 32; j++)
			cout <<(short int)(input_feature[i].range(15+16*j,16*j)) <<" ";
		cout << endl;
	}
}
//
//
//void conv_validate :: print_weight(void)
//{
//	int i,j,k;
//	cout << "weight:"<<endl;
//	for(i = 0 ; i < (int)(ceil((double)num_input/32))*num_output * kernel_size*kernel_size; i++)
//	{
//		cout << i << ":";
//		for(j = 0 ; j < 32; j++)
//			cout << weight[i].range(15+16*j,16*j) <<" ";
//		cout << endl;
//	}
//}
//
//void conv_validate :: print_bias(void)
//{
//	int i;
//	cout <<"bias:"<<endl;
//	for(i = 0 ; i < num_output; i++)
//		cout << i << ":" << bias[i] <<endl;
//}
//
//
//void conv_validate :: print_feature_out(void)
//{
//	int i,j;
//	cout <<"feature out:"<<endl;
//	for(i = 0 ; i < outputfeature_size * outputfeature_size * (int)(ceil(double(num_output)/32)); i++)
//	{
//		cout <<i<<":";
//		for(j = 0 ; j < 32; j++)
//			cout <<output_feature[i].range(15+16*j,16*j) <<" ";
//		cout << endl;
//	}
//}
//
//void conv_validate :: software_conv_process(void)       //assume padding = 2
//{
//	int i,j,k,x,y,z;
//	int temp;
//	short int* temp_array = new short int[(inputfeature_size+2*padding) * (inputfeature_size+2*padding) * num_input];
//
//	for(k = 0 ; k < num_input; k++)
//		for(i = 0 ; i < inputfeature_size+2*padding; i++)
//			for(j = 0 ; j < inputfeature_size+2*padding ; j++)
//				temp_array[ k * (inputfeature_size+2*padding ) * (inputfeature_size+2*padding ) + i * (inputfeature_size+2*padding ) + j] = 0;
//
//	for(j = 0 ; j < num_input; j++)
//		for(x = padding; x < inputfeature_size + padding; x++)
//			for(y = padding; y < inputfeature_size + padding; y++)
//				temp_array[ j * (inputfeature_size+2*padding ) * (inputfeature_size+2*padding ) + x * (inputfeature_size+2*padding ) + y]
//							= input_feature[(j/32) * inputfeature_size * inputfeature_size +(x-padding)*inputfeature_size + (y-padding)].range(16*(j%32)+15,16*(j%32));
//
//	cout <<"software processing..." << endl;
//	for(i = 0 ; i < num_output; i++)
//	{
//		for(x = 0 ; x < inputfeature_size-kernel_size+1+2*padding; x+=stride)
//			for(y = 0; y < inputfeature_size-kernel_size+1+2*padding;y+=stride)
//			{
//				temp = bias[i].range(15,0);
//				for(j = 0 ; j < num_input; j++)
//				{
//
//
//					for(k = 0 ; k < kernel_size;k++)
//					{
//						for(z = 0 ; z < kernel_size; z++)
//						{
//
//							temp += (int)(temp_array[j * (inputfeature_size+2*padding ) * (inputfeature_size+2*padding ) + x * (inputfeature_size+2*padding) + y + (k*(inputfeature_size+2*padding) + z)])
//									//*(int)(weight[(j/32)*num_output*kernel_size*kernel_size +i*kernel_size*kernel_size + k*kernel_size + z].range(15+16*(j%32),16*(j%32)))/64;
//								   *(int)(weight[(i/32)*num_input*kernel_size*kernel_size +j*kernel_size*kernel_size + k*kernel_size + z].range(15+16*(i%32),16*(i%32)))/64;
//
//						}
//					}
//				}
//
//				output_feature_software[(i/32) *outputfeature_size * outputfeature_size + (x/stride) * outputfeature_size + y/stride].range(16*(i%32)+15,16*(i%32)) = (temp < 0) ? 0 : temp;
//			}
//	}
//}
//
//
//void conv_validate :: print_feature_out_softeare(void)
//{
//	int i,j;
//	cout <<"software feature out:"<<endl;
//	for(i = 0 ; i < outputfeature_size * outputfeature_size * (int)(ceil(double(num_output)/32)); i++)
//	{
//		cout <<i<<":";
//		for(j = 0 ; j < 32; j++)
//			cout <<output_feature_software[i].range(15+16*j,16*j) <<" ";
//		cout << endl;
//	}
//}
//
//void conv_validate :: test_fun(void)
//{
//	cout << "class test : "<< outputfeature_size << endl;
//	cout << (int)(ceil((double)num_input/32)) << endl;
//}
