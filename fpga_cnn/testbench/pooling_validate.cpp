#include <iostream>
#include <math.h>
#include <stdlib.h>
#include "pooling_validate.h"
using namespace std;

pooling_validate::pooling_validate(int num_input,int stride,int kernel_size,int inputfeature_size,int act)
{
	
	int i,j,k;
	this->num_input = num_input;
	this->stride = stride;
	this->kernel_size = kernel_size;
	this->inputfeature_size = inputfeature_size;
	this->act = act;
	outputfeature_size = (inputfeature_size-kernel_size)/stride + 1;
	
	config_list[0] = inputfeature_size;
	config_list[1] = inputfeature_size;
    config_list[2] = num_input;
	config_list[3] = kernel_size;
	config_list[4] = outputfeature_size;
	config_list[5] = outputfeature_size;
	config_list[6] = stride;
	config_list[7] = 0;
	config_list[8] = act;
	config_list[9] = 0;
	config_list[10] = 0;
	config_list[11] = 0;
	config_list[12] = 0;
	config_list[13] = 0;
	config_list[14] = 0;
	config_list[15] = 0;
	
	input_feature = new ap_int<512>[inputfeature_size * inputfeature_size * (int)(ceil(((double)num_input)/32))];
	output_feature = new ap_int<512>[outputfeature_size * outputfeature_size * (int)(ceil(((double)num_input)/32))];
	output_feature_software = new ap_int<512>[outputfeature_size * outputfeature_size * (int)(ceil(((double)num_input)/32))];
	
	
	for(i = 0 ; i < num_input; i++)
		for(j = 0 ; j < inputfeature_size ;j++)
			for(k = 0 ; k < inputfeature_size; k++)
//				input_feature[i/32*inputfeature_size*inputfeature_size + j*inputfeature_size + k].range(i%32) = rand()%10;
				;
}


//void pooling_validate::print_feature_in(void)
//{
//	int i,j,k;
//	for(i = 0 ; i < num_input; i++)
//		for(j = 0 ; j < inputfeature_size ; j++)
//			for(k = 0 ; k < inputfeature_size; k++)
//				;
//
//}







