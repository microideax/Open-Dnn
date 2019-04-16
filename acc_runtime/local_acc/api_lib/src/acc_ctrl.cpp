#include "acc_ctrl.h"

using namespace std;

const char *device_h2c = DEVICE_H2C;
const char *device_c2h = DEVICE_C2H;
const char *device_ctrl = DEVICE_CTRL;

acc_ctrl::acc_ctrl( uint32_t para_offset_addr,
			 		uint32_t weight_offset_addr,
			 		uint32_t data_in_offset_addr,
			 		off_t    ctrl_addr
		 			)
{
	this->para_offset_addr = para_offset_addr;
	this->weight_offset_addr = weight_offset_addr;
	this->data_in_offset_addr = data_in_offset_addr;
	this->ctrl_addr = ctrl_addr;
}

void acc_ctrl::write_weight(	short int weight[][32],
								int weight_length)
{
	cout <<"write weight start" << endl;
	int fpga_fd;
	int rc;
	off_t off;
	fpga_fd= open(device_h2c,O_RDWR);
	assert(fpga_fd >= 0);
	off = lseek(fpga_fd,weight_offset_addr,SEEK_SET);
	rc = write(fpga_fd, weight, weight_length);
	assert(rc == weight_length);
	close(fpga_fd); 
	cout <<"write weight finish" << endl;
}


void acc_ctrl::write_para(	int* para_list,
							int para_length)
{
	cout <<"write para start" << endl;
	int fpga_fd;
	int rc;
	off_t off;
	fpga_fd= open(device_h2c,O_RDWR);
	assert(fpga_fd >= 0);
	off = lseek(fpga_fd,para_offset_addr,SEEK_SET);
	rc = write(fpga_fd, para_list, para_length);
	assert(rc == para_length);
	close(fpga_fd); 
	cout <<"write para finish" << endl;
}


void acc_ctrl::write_data(	short int feature[][32],
							int feature_length)
{
	cout <<"write data start"<<endl;
	int fpga_fd;
	int rc;
	off_t off;
	fpga_fd= open(device_h2c,O_RDWR);
	assert(fpga_fd >= 0);
	off = lseek(fpga_fd,data_in_offset_addr,SEEK_SET);
	rc = write(fpga_fd, feature, feature_length);
	assert(feature_length);
	close(fpga_fd);
	cout <<"write data finish"<<endl; 
}



void acc_ctrl::start_process(int mode)
{
	cout <<"process start"<<endl;
	timeval begin,end;
	int i,j,k;
	int fpga_fd;
	void *map_base, *virt_addr;

	fpga_fd = open(device_ctrl,O_RDWR|O_SYNC);
	assert(fpga_fd >= 0);
	cout <<"fpga_ctrl_open"<<endl;

    //config_mem_offset
   	map_base = mmap(0,MAP_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,fpga_fd,ctrl_addr);
   	cout <<"map_base:" << map_base << endl;
    //1.weight_mem_offset
   	virt_addr = (uint8_t*)map_base + 0x18;
   	(*(int*)virt_addr) = weight_offset_addr;
  	cout <<"virt_addr:" << virt_addr << endl;

   	//2.para_mem_offset
   	virt_addr = (uint8_t*)map_base + 0x10;
   	(*(int*)virt_addr) = para_offset_addr;   

    //3.data_in_mem offset
   	virt_addr = (uint8_t*)map_base + 0x20;
   	(*(int*)virt_addr) = data_in_offset_addr;

	//4.switch storage mode
	virt_addr = (uint8_t*)map_base + 0x28;
	(*(int*)virt_addr) = mode;

  	//start process
	virt_addr = (int*)map_base;

	//gettimeofday(&begin,NULL);
	(*((uint32_t *) virt_addr)) = 0x00000001;
	while(((*((uint32_t *) virt_addr)) & 0x00000002)!=0x00000002);
	//gettimeofday(&end,NULL);
	//cout <<"process time    " << (end.tv_usec - begin.tv_usec)/1000.0 <<"ms" <<endl;
	close(fpga_fd);
	cout <<"process finish"<<endl;
}




void acc_ctrl::read_data(short int feature[][32],int feature_length)
{
	cout << "read start"  << endl;
	int fpga_fd;
	int rc;
	off_t off;
	fpga_fd = open(device_c2h, O_RDWR | O_NONBLOCK);
	assert(fpga_fd >= 0);
	off = lseek(fpga_fd,0xC0000000,SEEK_SET);
	cout << "read finish00" << endl;
	rc = read(fpga_fd, feature, feature_length);
	assert(rc == feature_length);
	close(fpga_fd); 
	cout << "read finish" << endl;
}