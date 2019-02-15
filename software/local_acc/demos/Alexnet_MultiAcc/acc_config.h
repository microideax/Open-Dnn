#ifndef ACC_CONFIG_H
#define ACC_CONFIG_H

// #define BIAS_MEM        	0x00005000
//#define BIAS_MEM        	0x00008000
#define BIAS_MEM        	0x0000B000
#define BIAS_OFFSET		    0x00000000

#define DATA_MEM_0_ADDR    	0x10000000				//0x01000000         
#define DATA_MEM_1_ADDR     0x11000000				//0x01400000
#define WEIGHT_MEM       	0x12000000     			 	//0x01800000
#define DATA_MEM_2_ADDR     0x13000000				//0x01C00000    

#define DATA_MEM_0_OFFSET  	  0x00400000			//0x00040000
#define DATA_MEM_1_OFFSET 	  0x00440000			//0x00050000
#define WEIGHT_OFFSET   	  0x00480000			//0x00060000
#define DATA_MEM_2_OFFSET     0x004C0000 			//0x00070000







#endif