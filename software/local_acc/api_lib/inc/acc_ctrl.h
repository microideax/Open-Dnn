#ifndef ACC_CTRL_H
#define ACC_CTRL_H

#include "cl_tsc.h"
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <fcntl.h>
#include <assert.h>
#include <stdint.h>
//using namespace std;
//==============board level interface============//
#define DEVICE_H2C      "/dev/xdma0_h2c_1"
#define DEVICE_C2H      "/dev/xdma0_c2h_1"
#define DEVICE_CTRL		"/dev/xdma0_user"

#define MAP_SIZE (8*1024UL)




//================ctrl port addr=================

#define ACC0_CTRL_PORT_ADDR    0x0000      
#define ACC1_CTRL_PORT_ADDR    0x1000  
#define ACC2_CTRL_PORT_ADDR    0x2000  

//=================bram address================

#define ACC0_LAYER_NUM_MEM_ADDR	 	   	0x3000
#define ACC0_CONV_PARA_MEM_ADDR       	0x4000
#define ACC0_POOL_PARA_MEM_ADDR         0x5000
#define ACC0_BIAS_IN_MEM_ADDR        	0x6000

#define ACC1_LAYER_NUM_MEM_ADDR	 	   	0x7000
#define ACC1_CONV_PARA_MEM_ADDR       	0x8000
#define ACC1_POOL_PARA_MEM_ADDR         0x9000
#define ACC1_BIAS_IN_MEM_ADDR        	0xA000

#define ACC2_LAYER_NUM_MEM_ADDR	 	   	0xB000
#define ACC2_CONV_PARA_MEM_ADDR       	0xC000
#define ACC2_POOL_PARA_MEM_ADDR         0xD000
#define ACC2_BIAS_IN_MEM_ADDR        	0xE000

//=================ddr address=======================

#define DATA_MEM_ADDR_0       			0x10000000
#define DATA_MEM_ADDR_1       			0x11000000
#define DATA_MEM_ADDR_2       			0x12000000


#define WEIGHT_MEM_ADDR               0x1C000000


//==============address==================//
class ACC_ctrl
{
private:
	uint32_t layer_num_addr;
	uint32_t conv_para_addr;
	uint32_t pool_para_addr;
	off_t ctrl_addr;
	uint32_t bias_mem_addr;
public:
	ACC_ctrl(
			 uint32_t layer_num_addr,
			 uint32_t conv_para_addr,
			 uint32_t pool_para_addr,
			 uint32_t bias_mem_addr,
			 off_t    ctrl_addr
			 );
	void write_bias(int* bias,int bias_length);
	static void write_weight(short int weight[][32],int weight_length,uint32_t weight_mem);
	
	void write_data(short int feature[][32],int feature_length,int memId);
	void read_data(short int feature[][32],int feature_length,int memId,int isOffset);
	void start_process(int layer_num,uint32_t para_conv_array[][16],uint32_t para_pool_array[][16],int memId);
	void checkEnd(void);
};



struct MyLayer
{
	int id;
	int type;     //0:conv  1:pool
	uint32_t input_num;
	uint32_t output_num;
	uint32_t kernel_size;
	uint32_t feature_in_size;
	uint32_t feature_out_size;
	uint32_t stride;
	uint32_t padding;
	uint32_t act;
	uint32_t weight_offset_host;
	uint32_t bias_offset_host;
	uint32_t data_in_offset;
	uint32_t data_out_offset;
};

#endif
