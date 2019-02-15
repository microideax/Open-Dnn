#ifndef LAYER_H
#define LAYER_H
#include <stdint.h>
#include <iostream>
#include <stdint.h>
#include "acc_ctrl.h"
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
						 );
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
							);
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
					);
#endif