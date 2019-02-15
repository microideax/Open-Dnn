#include "layer.h"
#include "acc_ctrl.h"
#include <iostream>

using namespace std;

void conv_layer_construct(uint32_t 	input_num,
						 uint32_t 	output_num,
						 uint32_t 	kernel_size,
						 uint32_t 	feature_in_size,
						 uint32_t 	feature_out_size,
						 uint32_t 	stride,
						 uint32_t 	padding,
						 uint32_t 	act,	
						 uint32_t   weight_offset,
						 uint32_t   bias_offset,
						 uint32_t   data_in_offset,
						 uint32_t   data_out_offset,
						 uint32_t*  para_list
						 )
{
	para_list[0] = input_num;
	para_list[1] = kernel_size;
	para_list[2] = output_num;
	para_list[3] = feature_in_size;
	para_list[4] = feature_in_size;
	para_list[5] = feature_out_size;
	para_list[6] = feature_out_size;
	para_list[7] = stride;
	para_list[8] = padding;
	para_list[9] = act;
	para_list[10] = weight_offset;
	para_list[11] = bias_offset;
	para_list[12] = data_in_offset;
	para_list[13] = data_out_offset;
	para_list[14] = 0;
	para_list[15] = 0;
}





void pooling_layer_construct(uint32_t feature_in_size,
							uint32_t  input_num,
							uint32_t kernel_size,
							uint32_t feature_out_size,
							uint32_t stride,
							uint32_t padding,
							uint32_t act,
						 	uint32_t   data_in_offset,
						 	uint32_t   data_out_offset,
						 	uint32_t*  para_list
							)
{		
	para_list[0] = feature_in_size;
	para_list[1] = feature_in_size;
	para_list[2] = input_num;
	para_list[3] = kernel_size;
	para_list[4] = feature_out_size;
	para_list[5] = feature_out_size;
	para_list[6] = stride;
	para_list[7] = padding;
	para_list[8] = act;
	para_list[9] = data_in_offset;
	para_list[10] = data_out_offset;
	para_list[11] = 0;
	para_list[12] = 0;
	para_list[13] = 0;
	para_list[14] = 0;
	para_list[15] = 1;
}

void layer_construct(
					MyLayer* layer,
					int id,
					int type,     //0:conv  1:pool
					uint32_t input_num,
					uint32_t output_num,
					uint32_t kernel_size,
					uint32_t feature_in_size,
					uint32_t feature_out_size,
					uint32_t stride,
					uint32_t padding,
					uint32_t act,
					uint32_t weight_offset_host,
					uint32_t bias_offset_host
					 	)
{
	layer->id 					= id;
	layer->type     			= type;					//0:conv  1:pool
	layer->input_num			= input_num;
	layer->output_num            = output_num;
	layer->kernel_size           = kernel_size;
	layer->feature_in_size       = feature_in_size;
	layer->feature_out_size      = feature_out_size;
	layer->stride                = stride;
	layer->padding               = padding;
	layer->act                   = act;
	layer->weight_offset_host    = weight_offset_host;
	layer->bias_offset_host      = bias_offset_host;
	//cout <<"print in function:" << layer.id << endl;
}