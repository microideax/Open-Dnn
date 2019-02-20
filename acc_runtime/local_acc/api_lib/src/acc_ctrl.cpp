#include "acc_ctrl.h"

using namespace std;

const char *device_h2c = DEVICE_H2C;
const char *device_c2h = DEVICE_C2H;
const char *device_ctrl = DEVICE_CTRL;

ACC_ctrl::ACC_ctrl(
		 uint32_t layer_num_addr,
		 uint32_t conv_para_addr,
		 uint32_t pool_para_addr,
		 uint32_t bias_mem_addr,
		 off_t    ctrl_addr
		 )
{
	this->layer_num_addr = layer_num_addr;
	this->conv_para_addr = conv_para_addr;
	this->pool_para_addr = pool_para_addr;
	this->ctrl_addr = ctrl_addr;
	this->bias_mem_addr = bias_mem_addr;
}
void ACC_ctrl::write_bias(int* bias,int bias_length)
{
	int i = 0;
	int fpga_fd;
	void *map_base, *virt_addr;
	fpga_fd = open(device_ctrl,O_RDWR|O_SYNC);
	assert(fpga_fd >= 0);
	map_base = mmap(0,MAP_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,fpga_fd,bias_mem_addr);
	for(i=0;i<bias_length;i++)
	{
		virt_addr = (int*)map_base + i;
		(*(int*)virt_addr)=bias[i];
	}
	close(fpga_fd); 
}

void ACC_ctrl::write_data(short int feature[][32],int feature_length,int memId)
{
	cout <<"write start"<<endl;
	int fpga_fd;
	int rc;
	off_t off;
	fpga_fd= open(device_h2c,O_RDWR);
	assert(fpga_fd >= 0);
	if(memId==0)
	{
		cout <<"write to mem0" << endl;
		off = lseek(fpga_fd,DATA_MEM_ADDR_0,SEEK_SET);
	}
	else if(memId==1)
	{
		cout <<"write to mem1" << endl;
		off = lseek(fpga_fd,DATA_MEM_ADDR_1,SEEK_SET);
	}
	else
	{
		cout <<"write to mem2" << endl;
		off = lseek(fpga_fd,DATA_MEM_ADDR_2,SEEK_SET);
	}
	rc = write(fpga_fd, feature, feature_length);
	assert(feature_length);
	close(fpga_fd);
	cout <<"write end"<<endl; 
}

void ACC_ctrl::read_data(short int feature[][32],int feature_length,int memId,int isOffset)
{
	cout << "read start"  << endl;
	int fpga_fd;
	int rc;
	off_t off;
	fpga_fd = open(device_c2h, O_RDWR | O_NONBLOCK);
	assert(fpga_fd >= 0);
	if(memId == 0)
	{
		cout <<"read from mem0" << endl;
		if(isOffset)
			off = lseek(fpga_fd,(0x400000+DATA_MEM_ADDR_0),SEEK_SET);
		else
			off = lseek(fpga_fd,DATA_MEM_ADDR_0,SEEK_SET);
	}
	else if(memId == 1)
	{
		cout <<"read from mem1" << endl;
		if(isOffset)
			off = lseek(fpga_fd,(0x400000+DATA_MEM_ADDR_1),SEEK_SET);
		else
			off = lseek(fpga_fd,DATA_MEM_ADDR_1,SEEK_SET);
	}
	else
	{
		cout <<"read from mem2"<< endl;
		if(isOffset)
			off = lseek(fpga_fd,(0x400000+DATA_MEM_ADDR_2),SEEK_SET);
		else
			off = lseek(fpga_fd,DATA_MEM_ADDR_2,SEEK_SET);
	}

	rc = read(fpga_fd, feature, feature_length);
	assert(rc == feature_length);
	close(fpga_fd); 
	cout << "read end" << endl;
}

void ACC_ctrl::start_process(int layer_num,uint32_t para_conv_array[][16],uint32_t para_pool_array[][16],int memId)
{

	//uint64_t time1,time2,time3,time4,sum_time;
	timeval begin,end;
	int i,j,k;
	int fpga_fd;
	void *map_base, *virt_addr;

	fpga_fd = open(device_ctrl,O_RDWR|O_SYNC);
	assert(fpga_fd >= 0);
	cout <<"fpga_ctrl_open"<<endl;
		//config layer num
	map_base = mmap(0,MAP_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,fpga_fd,layer_num_addr);
	virt_addr = (int*)map_base;
	(*(int*)virt_addr) = layer_num;

	//config_conv_para
	map_base = mmap(0,MAP_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,fpga_fd,conv_para_addr);
	
	for(i = 0 ; i < layer_num; i++)
	{
		for(j = 0 ; j < 16; j++)
		{
			virt_addr = (int*)map_base + (i*16+j);
			(*(int*)virt_addr) = para_conv_array[i][j];
		}
    }

	//config_pool_para
	map_base = mmap(0,MAP_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,fpga_fd,pool_para_addr);
	
	for(i = 0 ; i < layer_num; i++)
	{
		for(j = 0 ; j < 16; j++)
		{
			virt_addr = (int*)map_base + (i*16+j);
			(*(int*)virt_addr) = para_pool_array[i][j];
		}
    }

    //config_mem_offset
   	map_base = mmap(0,MAP_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,fpga_fd,ctrl_addr);

    //1.weight_mem_offset
   	virt_addr = (uint8_t*)map_base + 0x10;
   	(*(int*)virt_addr) = WEIGHT_MEM_ADDR;
    //2.data_in_0_mem offset
   	virt_addr = (uint8_t*)map_base + 0x18;
   	if(memId == 0)
   		(*(int*)virt_addr) = DATA_MEM_ADDR_0;
 	else if(memId == 1)
 		(*(int*)virt_addr) = DATA_MEM_ADDR_1;
   	else 
   		(*(int*)virt_addr) = DATA_MEM_ADDR_2;


    //3.data_out_0_mem offset
   	virt_addr = (uint8_t*)map_base + 0x20;
   	if(memId == 0)
   		(*(int*)virt_addr) = DATA_MEM_ADDR_0;
 	else if(memId == 1)
 		(*(int*)virt_addr) = DATA_MEM_ADDR_1;
   	else
   		(*(int*)virt_addr) = DATA_MEM_ADDR_2; 



  	//start process
	cout << "start process"<<endl;
	virt_addr = (int*)map_base;

	//gettimeofday(&begin,NULL);
	(*((uint32_t *) virt_addr)) = 0x00000001;
	//while(((*((uint32_t *) virt_addr)) & 0x00000002)!=0x00000002);
	//gettimeofday(&end,NULL);

	//cout <<"process time    " << (end.tv_usec - begin.tv_usec)/1000.0 <<"ms" <<endl;
	close(fpga_fd);
}

void ACC_ctrl::write_weight(short int weight[][32],int weight_length,uint32_t weight_mem)
{
	cout <<"write weight start" << endl;
	int fpga_fd;
	int rc;
	off_t off;
	fpga_fd= open(device_h2c,O_RDWR);
	assert(fpga_fd >= 0);
	off = lseek(fpga_fd,weight_mem,SEEK_SET);
	rc = write(fpga_fd, weight, weight_length);
	assert(rc == weight_length);
	close(fpga_fd); 
	cout <<"write weight end" << endl;
}

void ACC_ctrl::checkEnd(void)
{
	int fpga_fd;
	void *map_base, *virt_addr;
	fpga_fd = open(device_ctrl,O_RDWR|O_SYNC);
	assert(fpga_fd >= 0);
	map_base = mmap(0,MAP_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,fpga_fd,ctrl_addr);
	virt_addr = (int*)map_base;
	while(((*((uint32_t *) virt_addr)) & 0x00000002)!=0x00000002);
	cout <<"process end" << endl;
}