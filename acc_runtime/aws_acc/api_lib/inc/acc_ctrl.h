#ifndef ACC_CTRL_H
#define ACC_CTRL_H

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
#include "cl_tsc.h"

#include "fpga_pci.h"
#include "fpga_mgmt.h"
#include "fpga_dma.h"

using namespace std;
//==============board level interface============//
#define DEVICE_H2C      "/dev/xdma0_h2c_1"
#define DEVICE_C2H      "/dev/xdma0_c2h_2"
#define DEVICE_CTRL		"/dev/xdma0_user"

//================ctrl port addr=================

class acc_ctrl
{
private:
	uint32_t para_offset_addr;
	uint32_t bias_offset_addr;
	uint32_t weight_offset_addr;
	uint32_t data_in_offset_addr;
	off_t    ctrl_addr;
public:
	acc_ctrl(
			 uint32_t para_offset_addr,
			 uint32_t weight_offset_addr,
			 uint32_t data_in_offset_addr,
			 off_t    ctrl_addr
			 );

	void write_bias(int* bias,int bias_length);
	void write_weight(short int weight[][32],int weight_length);
	void write_para(int* para_list,int para_length);
	void write_data(short int feature[][32],int feature_length);
	void start_process(int mode);
	void read_data(short int feature[][32],int feature_length,int memId);
};

#endif
