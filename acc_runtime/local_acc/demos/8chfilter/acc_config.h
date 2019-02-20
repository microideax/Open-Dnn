#ifndef ACC_CONFIG_H
#define ACC_CONFIG_H

#define BIAS_MEM              0xC0000000

#define DATA_IN_MEM    	      0x10000000				//0x01000000         
#define DATA_OUT_MEM          0x11000000				//0x01400000
#define WEIGHT_MEM            0x12000000     			 	//0x01800000
#define POOLING_OUT_MEM       0x13000000				//0x01C00000    

#define BIAS_OFFSET	      0x00000000
#define DATA_IN_OFFSET        0x00400000			//0x00040000
#define DATA_OUT_OFFSET       0x00440000			//0x00050000
#define WEIGHT_OFFSET         0x00480000			//0x00060000
#define POOLING_OUT_OFFSET    0x004C0000 			//0x00070000

#endif
