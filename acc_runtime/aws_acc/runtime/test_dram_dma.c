/*
 * Amazon FPGA Hardware Development Kit
 *
 * Copyright 2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Amazon Software License (the "License"). You may not use
 * this file except in compliance with the License. A copy of the License is
 * located at
 *
 *    http://aws.amazon.com/asl/
 *
 * or in the "license" file accompanying this file. This file is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, express or
 * implied. See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>

#include "fpga_pci.h"
#include "fpga_mgmt.h"
#include "fpga_dma.h"
#include "utils/lcd.h"
//#include "acc_ctrl.h"
#include "test_dram_dma_common.h"

#define	MEM_16G              (1ULL << 34)
#define USER_INTERRUPTS_MAX  (16)

/* use the standard out logger */
static const struct logger *logger = &logger_stdout;

void usage(const char* program_name);
int dma_example(int slot_id, size_t buffer_size);

void rand_string(char *str, size_t size);

int main(int argc, char **argv) {
    int rc;
    int slot_id = 0;
    pci_bar_handle_t pci_bar_handle = PCI_BAR_HANDLE_INIT;
    uint32_t value0,value1;
    uint32_t value2;


    /* setup logging to print to stdout */
    rc = log_init("test_dram_dma");
    fail_on(rc, out, "Unable to initialize the log.");
    rc = log_attach(logger, NULL, 0);
    fail_on(rc, out, "%s", "Unable to attach to the log.");

    /* initialize the fpga_plat library */
    rc = fpga_mgmt_init();
    fail_on(rc, out, "Unable to initialize the fpga_mgmt library");
    rc = fpga_pci_init();
    fail_on(rc, out, "Unable to initialize the fpga_pci library");
    /* check that the AFI is loaded */
    log_info("Checking to see if the right AFI is loaded...");



    int i;
   // int j;
    //1 data in generation
    
    //1.1 feature in generation
    short int feature_in[1024][32];
    for(i = 0 ; i < 16*16; i++)
        feature_in[i][0]= 64;
    //1.2 weight generation
    short int weight[1024][32];
    for(i = 0 ; i < 9; i++)
        weight[i][0]=16;
    //1.3 bias generation
    int bias[512] = {0};
   // for(i = 0 ; i < 512; i++)
     //   bias[i] = 64;
     bias[0] = 0; 
     bias[256] = 0;
    //1.4 para generation
    int para[512] = {0};
    para[0] = 1;
    para[16+0] = 1;//N
    para[16+1] = 3;//K
    para[16+2] = 1;//M
    para[16+3] = 16;//Rin
    para[16+4] = 16;//Cin
    para[16+5] = 16;//R
    para[16+6] = 16;//C
    para[16+7] = 1;//S
    para[16+8] = 1;//P
    para[16+9] = 1;//act
    para[16+10] = 0;//weight_offset
    para[16+11] = 0;//bias_offset
    para[16+12] = 0;//in_offset
    para[16+13] = 0;//out_offset
    para[16+14] = 0;//inport
    para[16+15] = 0;
    
    para[256] = 1;
    para[256+16+0] = 1;//N
    para[256+16+1] = 3;//K
    para[256+16+2] = 1;//M
    para[256+16+3] = 16;//Rin
    para[256+16+4] = 16;//Cin
    para[256+16+5] = 16;//R
    para[256+16+6] = 16;//C
    para[256+16+7] = 1;//S
    para[256+16+8] = 1;//P
    para[256+16+9] = 1;//act
    para[256+16+10] = 0;//weight_offset
    para[256+16+11] = 0;//bias_offset
    para[256+16+12] = 0;//in_offset
    para[256+16+13] = 0;//out_offset
    para[256+16+14] = 0;//inport
    para[256+16+15] = 0;



    //2 transfer input data
    //2.1 open DMA channel
    int write_fd;
    write_fd = -1;
    write_fd = fpga_dma_open_queue( FPGA_DMA_XDMA, slot_id,/*channel*/ 0, /*is_read*/ false);
    fail_on((rc = (write_fd < 0) ? -1 : 0), out, "unable to open write dma queue");
    //2.2 transfer  feature in
    rc = fpga_dma_burst_write(write_fd, (uint8_t*)feature_in, 1024*64,0xD0000000);
    fail_on(rc, out, "DMA write failed");
    rc = fpga_dma_burst_write(write_fd, (uint8_t*)feature_in, 1024*64,0xDB000000);
    fail_on(rc, out, "DMA write failed");
    //2.3 transfer weight
    rc = fpga_dma_burst_write(write_fd, (uint8_t*)weight, 1024*64,0xD1000000);
    fail_on(rc, out, "DMA write failed");
    //2.4 transfer bias
    rc = fpga_dma_burst_write(write_fd, (uint8_t*)bias, 512*4,0xD2000000);
    fail_on(rc, out, "DMA write failed");
    //2.5 transfer para
    rc = fpga_dma_burst_write(write_fd, (uint8_t*)para, 512*4,0xD3000000);
    fail_on(rc, out, "DMA write failed");
    //2.6 close DMA channel
    if(write_fd >= 0) 
        close(write_fd);
    

    //3 start process
    //3.1 open AXI Lite channel
    rc = fpga_pci_attach(0,FPGA_APP_PF, 0, 0,  &pci_bar_handle);
    fail_on(rc, out, "Unable to attach to the AFI on slot id %d", slot_id);
    //3.2 config offset addr
    rc = fpga_pci_poke(pci_bar_handle, 0x0000000+0x20, 0xD1000000);   //weight
    rc = fpga_pci_poke(pci_bar_handle, 0x0000000+0x18, 0xD2000000);   //bias
    rc = fpga_pci_poke(pci_bar_handle, 0x0000000+0x10, 0xD3000000);   //para
    rc = fpga_pci_poke(pci_bar_handle, 0x0000000+0x28, 0xD0000000);   //feature in
    
    rc = fpga_pci_poke(pci_bar_handle, 0x0001000+0x20, 0xD1000000);   //weight
    rc = fpga_pci_poke(pci_bar_handle, 0x0001000+0x18, 0xD2000000);   //bias
    rc = fpga_pci_poke(pci_bar_handle, 0x0001000+0x10, 0xD3000000);   //para
//    rc = fpga_pci_poke(pci_bar_handle, 0x0001000+0x28, 0xCA000000);   //feature in


    rc = fpga_pci_poke(pci_bar_handle, 0x0002000+0x20, 0xD1000000);   //weight
    rc = fpga_pci_poke(pci_bar_handle, 0x0002000+0x18, 0xD2000000);   //bias
    rc = fpga_pci_poke(pci_bar_handle, 0x0002000+0x10, 0xD3000000);   //para
//    rc = fpga_pci_poke(pci_bar_handle, 0x0002000+0x28, 0xDB000000);   //feature in

    //3.3 config mode
 //   rc = fpga_pci_poke(pci_bar_handle, 0x0000000+0x30, 0);    //acc0 mode 0
 //   rc = fpga_pci_poke(pci_bar_handle, 0x0001000+0x30, 0);    //acc1 mode 0
  //    rc = fpga_pci_poke(pci_bar_handle, 0x0002000+0x30, 0);    //acc2 mode 0 



    //3.4 start process
    //
    //
    //==================acc0 mode0
    rc = fpga_pci_poke(pci_bar_handle, 0x0000000+0x30, 0);    //acc0 mode 0
    rc = fpga_pci_peek(pci_bar_handle, 0x0000000, &value0);
    printf("value0:%x \n\r",value0);
    rc = fpga_pci_poke(pci_bar_handle, 0x0000000, 0x00000001);
    while((value0&0x00000002) != 0x00000002)
    {
        rc = fpga_pci_peek(pci_bar_handle, 0x0000000, &value0);
        fail_on(rc, out, "Unable to read read from the fpga !");
    }
    //==================acc0 mode 1

    rc = fpga_pci_poke(pci_bar_handle, 0x0000000+0x30, 1);    //acc0 mode 1
    rc = fpga_pci_peek(pci_bar_handle, 0x0000000, &value0);
    printf("value0:%x \n\r",value0);
    rc = fpga_pci_poke(pci_bar_handle, 0x0000000, 0x00000001);
    while((value0&0x00000002) != 0x00000002)
    {
        rc = fpga_pci_peek(pci_bar_handle, 0x0000000, &value0);
        fail_on(rc, out, "Unable to read read from the fpga !");
    }
    //==================acc1 mode0

    rc = fpga_pci_poke(pci_bar_handle, 0x0001000+0x30, 0);    //acc1 mode 0
    rc = fpga_pci_peek(pci_bar_handle, 0x0001000, &value1);
    printf("value1:%x \n\r",value1);
    rc = fpga_pci_poke(pci_bar_handle, 0x0001000, 0x00000001);
    while((value1&0x00000002) != 0x00000002)
    {
        rc = fpga_pci_peek(pci_bar_handle, 0x0001000, &value1);
        fail_on(rc, out, "Unable to read read from the fpga !");
    }
 
    //==================acc1 mode1
    rc = fpga_pci_poke(pci_bar_handle, 0x0001000+0x30, 1);    //acc1 mode 1
    rc = fpga_pci_peek(pci_bar_handle, 0x0001000, &value1);
    printf("value1:%x \n\r",value1);
    rc = fpga_pci_poke(pci_bar_handle, 0x0001000, 0x00000001);
    while((value1&0x00000002) != 0x00000002)
    {
        rc = fpga_pci_peek(pci_bar_handle, 0x0001000, &value1);
        fail_on(rc, out, "Unable to read read from the fpga !");
    }
    //==================acc2 mode0
    rc = fpga_pci_poke(pci_bar_handle, 0x0002000+0x30, 0);    //acc0 mode 0
    rc = fpga_pci_peek(pci_bar_handle, 0x0002000, &value2);
    printf("value2:%x \n\r",value2);
    rc = fpga_pci_poke(pci_bar_handle, 0x0002000, 0x00000001);
    while((value2&0x00000002) != 0x00000002)
    {
        rc = fpga_pci_peek(pci_bar_handle, 0x0002000, &value2);
        fail_on(rc, out, "Unable to read read from the fpga !");
    }
    //==================acc2 mode 1

    rc = fpga_pci_poke(pci_bar_handle, 0x0002000+0x30, 1);    //acc0 mode 1
    rc = fpga_pci_peek(pci_bar_handle, 0x0002000, &value2);
    printf("value2:%x \n\r",value2);
    rc = fpga_pci_poke(pci_bar_handle, 0x0002000, 0x00000001);
    while((value2&0x00000002) != 0x00000002)
    {
        rc = fpga_pci_peek(pci_bar_handle, 0x0002000, &value2);
        fail_on(rc, out, "Unable to read read from the fpga !");
    }
  //  rc = fpga_pci_peek(pci_bar_handle, 0x0001000, &value1);
  //    rc = fpga_pci_peek(pci_bar_handle, 0x0002000, &value2);
    //  printf("================1.1==================\n\r");
   // printf("value0:%x \n\r",value0);
   // printf("value1:%x \n\r",value1);
  //    printf("value2:%x \n\r",value2);
   // rc = fpga_pci_poke(pci_bar_handle, 0x0000000, 0x00000001);
   // rc = fpga_pci_poke(pci_bar_handle, 0x0001000, 0x00000001);
  //    rc = fpga_pci_poke(pci_bar_handle, 0x0002000, 0x00000001);

  //  rc = fpga_pci_peek(pci_bar_handle, 0x0000000, &value0);
  //  rc = fpga_pci_peek(pci_bar_handle, 0x0001000, &value1);
  //    rc = fpga_pci_peek(pci_bar_handle, 0x0002000, &value2);
     // printf("================1.2==================\n\r");
  //  printf("value0:%x \n\r",value0);
  //  printf("value1:%x \n\r",value1);
  //    printf("value2:%x \n\r",value2);
  //  rc = fpga_pci_peek(pci_bar_handle, 0x0000000, &value0);
  //  rc = fpga_pci_peek(pci_bar_handle, 0x0001000, &value1);
     // rc = fpga_pci_peek(pci_bar_handle, 0x0002000, &value2);
     // printf("================1.2==================\n\r");
  //  printf("value0:%x \n\r",value0);
  //  printf("value1:%x \n\r",value1);
     // printf("value2:%x \n\r",value2);
  //  rc = fpga_pci_peek(pci_bar_handle, 0x0000000, &value0);
  //  rc = fpga_pci_peek(pci_bar_handle, 0x0001000, &value1);
     // rc = fpga_pci_peek(pci_bar_handle, 0x0002000, &value2);
   // printf("================1.2==================\n\r");
  //  printf("value0:%x \n\r",value0);
  //  printf("value1:%x \n\r",value1);
    //  printf("value2:%x \n\r",value2);


  /*  while((value0&0x00000002) != 0x00000002)
    {
        rc = fpga_pci_peek(pci_bar_handle, 0x0000000, &value0);
        fail_on(rc, out, "Unable to read read from the fpga !");
    }*/ 

   /* while ((value1&0x00000002) != 0x00000002) 
    {
        rc = fpga_pci_peek(pci_bar_handle, 0x0001000, &value1);
        fail_on(rc, out, "Unable to read read from the fpga !");
    }*/ 
/*   while(((value0&0x00000002) != 0x00000002) || ((value1&0x00000002) != 0x00000002) )
    {
        rc = fpga_pci_peek(pci_bar_handle, 0x0000000, &value0);
        fail_on(rc, out, "Unable to read read from the fpga !");
        rc = fpga_pci_peek(pci_bar_handle, 0x0001000, &value1);
        fail_on(rc, out, "Unable to read read from the fpga !");
    } 
    rc = fpga_pci_peek(pci_bar_handle, 0x0000000, &value0);
    rc = fpga_pci_peek(pci_bar_handle, 0x0001000, &value1);
   // rc = fpga_pci_peek(pci_bar_handle, 0x0002000, &value2);
    printf("value0:%x \n\r",value0);
    printf("value1:%x \n\r",value1);*/
  //  printf("value2:%x \n\r",value2);
    //3.5 change mode
  //  rc = fpga_pci_poke(pci_bar_handle, 0x0000000+0x30, 1);    //acc0 mode 1
  //  rc = fpga_pci_poke(pci_bar_handle, 0x0001000+0x30, 1);    //acc1 mode 1
  //  rc = fpga_pci_poke(pci_bar_handle, 0x0002000+0x30, 1);    //acc2 mode 1 
    //3.6 start process
    
  //  rc = fpga_pci_peek(pci_bar_handle, 0x0000000, &value0);
  //  rc = fpga_pci_peek(pci_bar_handle, 0x0001000, &value1);
   // rc = fpga_pci_peek(pci_bar_handle, 0x0002000, &value2);
 //   printf("value0:%x \n\r",value0);
 //   printf("value1:%x \n\r",value1);
 //   printf("value2:%x \n\r",value2);
 //   rc = fpga_pci_poke(pci_bar_handle, 0x0000000, 0x00000001);
 //   rc = fpga_pci_poke(pci_bar_handle, 0x0001000, 0x00000001);
   // rc = fpga_pci_poke(pci_bar_handle, 0x0002000, 0x00000001);

   // rc = fpga_pci_peek(pci_bar_handle, 0x0000000, &value0);
   // rc = fpga_pci_peek(pci_bar_handle, 0x0001000, &value1);
  //  rc = fpga_pci_peek(pci_bar_handle, 0x0002000, &value2);
   // printf("value0:%x \n\r",value0);
  //  printf("value1:%x \n\r",value1);
  //  printf("value2:%x \n\r",value2);
   /* while(((value0&0x00000002) != 0x00000002) && ((value1&0x00000002) != 0x00000002) && ((value2&0x00000002) != 0x00000002))
    {
        rc = fpga_pci_peek(pci_bar_handle, 0x0000000, &value0);
        fail_on(rc, out, "Unable to read read from the fpga !");
        rc = fpga_pci_peek(pci_bar_handle, 0x0001000, &value1);
        fail_on(rc, out, "Unable to read read from the fpga !");
        rc = fpga_pci_peek(pci_bar_handle, 0x0002000, &value2);
        fail_on(rc, out, "Unable to read read from the fpga !");
    }*/ 

   // rc = fpga_pci_peek(pci_bar_handle, 0x0000000, &value0);
   // rc = fpga_pci_peek(pci_bar_handle, 0x0001000, &value1);
   // rc = fpga_pci_peek(pci_bar_handle, 0x0002000, &value2);
   // printf("value0:%x \n\r",value0);
   // printf("value1:%x \n\r",value1);
   // printf("value2:%x \n\r",value2);
   /* 
    //3.7 change mode

    rc = fpga_pci_poke(pci_bar_handle, 0x0000000+0x30, 0);    //acc0 mode 0
    rc = fpga_pci_poke(pci_bar_handle, 0x0001000+0x30, 0);    //acc1 mode 0
    rc = fpga_pci_poke(pci_bar_handle, 0x0002000+0x30, 0);    //acc2 mode 0 

    //3.8 start process
    rc = fpga_pci_peek(pci_bar_handle, 0x0000000, &value0);
    rc = fpga_pci_peek(pci_bar_handle, 0x0001000, &value1);
    rc = fpga_pci_peek(pci_bar_handle, 0x0002000, &value2);
    printf("value0:%x \n\r",value0);
    printf("value1:%x \n\r",value1);
    printf("value2:%x \n\r",value2);
    rc = fpga_pci_poke(pci_bar_handle, 0x0000000, 0x00000001);
    rc = fpga_pci_poke(pci_bar_handle, 0x0001000, 0x00000001);
    rc = fpga_pci_poke(pci_bar_handle, 0x0002000, 0x00000001);

    rc = fpga_pci_peek(pci_bar_handle, 0x0000000, &value0);
    rc = fpga_pci_peek(pci_bar_handle, 0x0001000, &value1);
    rc = fpga_pci_peek(pci_bar_handle, 0x0002000, &value2);
    printf("value0:%x \n\r",value0);
    printf("value1:%x \n\r",value1);
    printf("value2:%x \n\r",value2);
    while(((value0&0x00000002) != 0x00000002) && ((value1&0x00000002) != 0x00000002) && ((value2&0x00000002) != 0x00000002))
    {
        rc = fpga_pci_peek(pci_bar_handle, 0x0000000, &value0);
        fail_on(rc, out, "Unable to read read from the fpga !");
        rc = fpga_pci_peek(pci_bar_handle, 0x0001000, &value1);
        fail_on(rc, out, "Unable to read read from the fpga !");
        rc = fpga_pci_peek(pci_bar_handle, 0x0002000, &value2);
        fail_on(rc, out, "Unable to read read from the fpga !");
    } 

    rc = fpga_pci_peek(pci_bar_handle, 0x0000000, &value0);
    rc = fpga_pci_peek(pci_bar_handle, 0x0001000, &value1);
    rc = fpga_pci_peek(pci_bar_handle, 0x0002000, &value2);
    printf("value0:%x \n\r",value0);
    printf("value1:%x \n\r",value1);
    printf("value2:%x \n\r",value2);
    
    
    
    */
   
    
//3.5 close channel
    if (pci_bar_handle >= 0) 
    {
        rc = fpga_pci_detach(pci_bar_handle);
	if (rc) 
	{
            printf("Failure while detaching from the fpga.\n");
        }
    }


    //4 read feature out
    int read_fd;
    int j;
    read_fd = -1;
    short int feature_out[1024][32];
    //4.1 open DMA channel
    read_fd = fpga_dma_open_queue(FPGA_DMA_XDMA, slot_id, /*channel*/ 0, /*is_read*/ true);
    fail_on((rc = (read_fd < 0) ? -1 : 0), out, "unable to open read dma queue");
    //4.2 read feature out
    rc = fpga_dma_burst_read(read_fd, (uint8_t*)feature_out,16*16*64, 0xC0000000);
    fail_on(rc, out, "DMA read failed");
    //4.3 display
    for(i=0;i<16;i++)
    {
        for(j=0;j<16;j++)
           printf("%f ",feature_out[i*16+j][0]/64.0);
        printf("\n\r");
    }    
    //4.4 close DMA channel
    if (read_fd >= 0)
    {
        close(read_fd);
    }


out:
    log_info("TEST %s", (rc == 0) ? "PASSED" : "FAILED");
    return rc;
}

void usage(const char* program_name) {
    printf("usage: %s [--slot <slot>]\n", program_name);
}

/**
 * This example fills a buffer with random data and then uses DMA to copy that
 * buffer into each of the 4 DDR DIMMS.
 */
int dma_example(int slot_id, size_t buffer_size) {
    int write_fd, read_fd, dimm, rc;

    write_fd = -1;
    read_fd = -1;

    uint8_t *write_buffer = malloc(buffer_size);
    uint8_t *read_buffer = malloc(buffer_size);
    if (write_buffer == NULL || read_buffer == NULL) {
        rc = -ENOMEM;
        goto out;
    }

    read_fd = fpga_dma_open_queue(FPGA_DMA_XDMA, slot_id,
        /*channel*/ 0, /*is_read*/ true);
    fail_on((rc = (read_fd < 0) ? -1 : 0), out, "unable to open read dma queue");

    write_fd = fpga_dma_open_queue(FPGA_DMA_XDMA, slot_id,
        /*channel*/ 0, /*is_read*/ false);
    fail_on((rc = (write_fd < 0) ? -1 : 0), out, "unable to open write dma queue");

    rc = fill_buffer_urandom(write_buffer, buffer_size);
    fail_on(rc, out, "unabled to initialize buffer");

    for (dimm = 0; dimm < 4; dimm++) {
        rc = fpga_dma_burst_write(write_fd, write_buffer, buffer_size,
            dimm * MEM_16G);
        fail_on(rc, out, "DMA write failed on DIMM: %d", dimm);
    }

    bool passed = true;
    for (dimm = 0; dimm < 4; dimm++) {
        rc = fpga_dma_burst_read(read_fd, read_buffer, buffer_size,
            dimm * MEM_16G);
        fail_on(rc, out, "DMA read failed on DIMM: %d", dimm);

        uint64_t differ = buffer_compare(read_buffer, write_buffer, buffer_size);
        if (differ != 0) {
            log_error("DIMM %d failed with %lu bytes which differ", dimm, differ);
            passed = false;
        } else {
            log_info("DIMM %d passed!", dimm);
        }
    }
    rc = (passed) ? 0 : 1;

out:
    if (write_buffer != NULL) {
        free(write_buffer);
    }
    if (read_buffer != NULL) {
        free(read_buffer);
    }
    if (write_fd >= 0) {
        close(write_fd);
    }
    if (read_fd >= 0) {
        close(read_fd);
    }
    /* if there is an error code, exit with status 1 */
    return (rc != 0 ? 1 : 0);
}



