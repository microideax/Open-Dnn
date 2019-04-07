#include <caffe/caffe.hpp>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <algorithm>
#include <iosfwd>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "caffe/common.hpp"
#include "caffe/proto/caffe.pb.h"
#include "caffe/util/io.hpp"

#include "layer_factory_impl.h"
#include "../../api_lib/inc/layer.h"
#include "../../api_lib/inc/acc_ctrl.h"
#include "../../api_lib/inc/cl_tsc.h"
#include "acc_config.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <math.h>
using namespace caffe;
using namespace cv;
using namespace std;

void load_fc(const LayerParameter& src,int num_input,int num_output,float* weight,float* bias);
void fc_software(float* feature_in,float* feature_out,float* weight,float* bias,int num_input,int num_output);

void software_validate_conv(int num_input,
                            int num_output,
                            int kernel_size,
                            int stride,
                            int padding,
                            int feature_in_size,
                            int feature_out_size,
                            short int feature_in[][32],
                            short int feature_out[][32],
                            short int weight[][32],
                            int* bias
                            );
void software_validate_pooling(int num_input,
                               int feature_in_size,
                               int feature_out_size,
                               short int feature_in[][32],
                               short int feature_out[][32]);

void input_feature_ready(short int input_feature[][32]);
void weight_ready(short int weight[][32]);
//void bias_ready(int* bias);
void para_ready(int para[][512]);

int main()
{
  short int input_feature[16*16][32] = {0};
  short int output_feature[16*16][32] = {0};
  short int weight[200][32] = {0};
  int bias[512] = {0};
  int para[3][512] = {0};
  short int temp_array[144][32] = {0};


  //1.input feature ready
  input_feature_ready(input_feature);

  //2.weight ready
  weight_ready(weight);

  //3.bias ready
  //bias_ready(bias);  

  //4.para ready
  para_ready(para);
  // for(int i = 0 ; i < 32; i++)
  // {
  //   for(int j = 0 ; j < 16; j++)
  //   {
  //     cout << para[i*16+j] <<" ";
  //   }
  //   cout << endl;
  // }




  acc_ctrl sub_net0(0x10000000,  //para_offset_addr
                    0x11000000,  //bias_offset_addr
                    0x12000000,  //weight_offset_addr
                    0x13000000,  //data_in_offset_addr
                    0x00000000  //ctrl_addr
                    );

  // acc_ctrl sub_net1(0x14000000,  //para_offset_addr
  //                   0x15000000,  //bias_offset_addr
  //                   0x16000000,  //weight_offset_addr
  //                   0x00000000,  //data_in_offset_addr
  //                   0x00001000  //ctrl_addr
  //                   );

  // acc_ctrl sub_net2(0x17000000,  //para_offset_addr
  //                   0x18000000,  //bias_offset_addr
  //                   0x19000000,  //weight_offset_addr
  //                   0x00000000,  //data_in_offset_addr
  //                   0x00002000  //ctrl_addr
  //                   );

  sub_net0.write_para(para[0],512*4);
  // sub_net1.write_para(para[1],512*4);
  // sub_net2.write_para(para[2],512*4);


  //sub_net0.write_bias(bias,512*4);
  // sub_net1.write_bias(bias,512*4);
  // sub_net2.write_bias(bias,512*4);

  

  sub_net0.write_weight(weight,200*64);
  // sub_net1.write_weight(weight,200*64);
  // sub_net2.write_weight(weight,200*64);


  sub_net0.write_data(input_feature,16*16*64);


  sub_net0.start_process(0);
  sub_net0.start_process(1);
  
  // sub_net1.start_process(0);
  // sub_net1.start_process(1);

  // sub_net2.start_process(0);
  // sub_net2.start_process(1);

  // sub_net0.start_process(0);
  // sub_net0.start_process(1);
  sub_net0.read_data(output_feature,16*16*64,0);

  int i,j,k;
  for(i=0;i<16;i++)
  {
    for(j=0;j<16;j++)
        cout << setw(10)<< output_feature[i*16+j][0]/64.0 <<" ";
    cout << endl;
  }
  cout <<"test finish"<< endl;



  // int fpga_fd;
  // int rc;
  // off_t off;
  // fpga_fd = open( "/dev/xdma0_c2h_1", O_RDWR | O_NONBLOCK);
  // assert(fpga_fd >= 0);

  // off = lseek(fpga_fd,0xC0000000,SEEK_SET);

  // rc = read(fpga_fd, temp_array, 12*12*64);
  // assert(rc ==  12*12*64);
  // close(fpga_fd); 
  // cout << "read finish" << endl;
  // for(int i = 0 ; i < 12; i++)
  // {
  //   for(int j = 0 ; j < 12; j++)
  //     cout << temp_array[i*12+j][0]<<" ";
  //   cout << endl;
  // }     
    return 0;
}



void input_feature_ready(short int input_feature[][32])
{
  int i,j;
  for(i=0;i<16*16;i++)
  {
    for(j=0;j<1;j++)
      input_feature[i][j] = 1<<6;
    for(j=1;j<32;j++)
      input_feature[i][j] = 0<<6;
  }
}



void weight_ready(short int weight[][32])
{
  int i;
  for(i=0;i<20;i++)
  {
    weight[i][0] = 64;
  }
  weight[20][0] = 0;
  weight[21][0] = 0;

  // for(i=18;i<36;i++) 
  // {
  //   weight[i][0] = 32;
  // }
}
// void bias_ready(int* bias)
// {
//   int i;

// }
void para_ready(int para[][512])
{
    int i;

    //===========================subnet0
    //0-1.layer_num
    para[0][0] = 1;
    //0-2.conv para
    para[0][16+0] = 1;//N
    para[0][16+1] = 3;//K
    para[0][16+2] = 1;//M
    para[0][16+3] = 16;//Rin
    para[0][16+4] = 16;//Cin
    para[0][16+5] = 16;//R
    para[0][16+6] = 16;//C
    para[0][16+7] = 1;//S
    para[0][16+8] = 1;//P
    para[0][16+9] = 1;//act
    para[0][16+10] = 0;//weight_offset
    para[0][16+11] = 20;//bias_offset
    para[0][16+12] = 0;//in_offset
    para[0][16+13] = 0;//out_offset
    para[0][16+14] = 0;//inport
    para[0][16+15] = 0;

    //1-1.layer_num
    para[0][256+0] = 1;
    //1-2.conv_para
    para[0][256+16+0] = 1; //N
    para[0][256+16+1] = 3; //K
    para[0][256+16+2] = 1; //M
    para[0][256+16+3] = 16; //Rin
    para[0][256+16+4] = 16; //Cin
    para[0][256+16+5] = 16;  //R
    para[0][256+16+6] = 16;  //C
    para[0][256+16+7] = 1;  //S
    para[0][256+16+8] = 1;  //P
    para[0][256+16+9] = 1;  //act
    para[0][256+16+10] = 10; //weight_offset
    para[0][256+16+11] = 21; //bias_offset
    para[0][256+16+12] = 0; //in_offset
    para[0][256+16+13] = 0; //out_offset
    para[0][256+16+14] = 0; //inport
    para[0][256+16+15] = 0;


    //===========================subnet1
    //0-1.layer_num
    para[1][0] = 1;
    //0-2.conv para
    para[1][16+0] = 1;//N
    para[1][16+1] = 3;//K
    para[1][16+2] = 1;//M
    para[1][16+3] = 16;//Rin
    para[1][16+4] = 16;//Cin
    para[1][16+5] = 16;//R
    para[1][16+6] = 16;//C
    para[1][16+7] = 1;//S
    para[1][16+8] = 1;//P
    para[1][16+9] = 1;//act
    para[1][16+10] = 0;//weight_offset
    para[1][16+11] = 0;//bias_offset
    para[1][16+12] = 0;//in_offset
    para[1][16+13] = 0;//out_offset
    para[1][16+14] = 0;//inport
    para[1][16+15] = 0;

    //1-1.layer_num
    para[1][256+0] = 1;
    //1-2.conv_para
    para[1][256+16+0] = 1; //N
    para[1][256+16+1] = 3; //K
    para[1][256+16+2] = 1; //M
    para[1][256+16+3] = 16; //Rin
    para[1][256+16+4] = 16; //Cin
    para[1][256+16+5] = 16;  //R
    para[1][256+16+6] = 16;  //C
    para[1][256+16+7] = 1;  //S
    para[1][256+16+8] = 1;  //P
    para[1][256+16+9] = 1;  //act
    para[1][256+16+10] = 9; //weight_offset
    para[1][256+16+11] = 0; //bias_offset
    para[1][256+16+12] = 0; //in_offset
    para[1][256+16+13] = 0; //out_offset
    para[1][256+16+14] = 0; //inport
    para[1][256+16+15] = 0;




    //===========================subnet2
    //0-1.layer_num
    para[2][0] = 1;
    //0-2.conv para
    para[2][16+0] = 1;//N
    para[2][16+1] = 3;//K
    para[2][16+2] = 1;//M
    para[2][16+3] = 16;//Rin
    para[2][16+4] = 16;//Cin
    para[2][16+5] = 16;//R
    para[2][16+6] = 16;//C
    para[2][16+7] = 1;//S
    para[2][16+8] = 1;//P
    para[2][16+9] = 1;//act
    para[2][16+10] = 0;//weight_offset
    para[2][16+11] = 0;//bias_offset
    para[2][16+12] = 0;//in_offset
    para[2][16+13] = 0;//out_offset
    para[2][16+14] = 0;//inport
    para[2][16+15] = 0;

    //1-1.layer_num
    para[2][256+0] = 1;
    //1-2.conv_para
    para[2][256+16+0] = 1; //N
    para[2][256+16+1] = 3; //K
    para[2][256+16+2] = 1; //M
    para[2][256+16+3] = 16; //Rin
    para[2][256+16+4] = 16; //Cin
    para[2][256+16+5] = 16;  //R
    para[2][256+16+6] = 16;  //C
    para[2][256+16+7] = 1;  //S
    para[2][256+16+8] = 1;  //P
    para[2][256+16+9] = 1;  //act
    para[2][256+16+10] = 9; //weight_offset
    para[2][256+16+11] = 0; //bias_offset
    para[2][256+16+12] = 0; //in_offset
    para[2][256+16+13] = 0; //out_offset
    para[2][256+16+14] = 0; //inport
    para[2][256+16+15] = 0;
}

void software_validate_conv(int num_input,
                            int num_output,
                            int kernel_size,
                            int stride,
                            int padding,
                            int feature_in_size,
                            int feature_out_size,
                            short int feature_in[][32],
                            short int feature_out[][32],
                            short int weight[][32],
                            int* bias
                            )
{
    int i,j,k,x,y,z;
    int temp;
    short int* temp_array = new short int[(feature_in_size+2*padding) * (feature_in_size+2*padding) * num_input];

    for(k = 0 ; k < num_input; k++)
        for(i = 0 ; i < feature_in_size+2*padding; i++)
            for(j = 0 ; j < feature_in_size+2*padding ; j++)
                temp_array[ k * (feature_in_size+2*padding ) * (feature_in_size+2*padding ) + i * (feature_in_size+2*padding ) + j] = 0;

    for(j = 0 ; j < num_input; j++)
        for(x = padding; x < feature_in_size + padding; x++)
            for(y = padding; y < feature_in_size + padding; y++)
                temp_array[ j * (feature_in_size+2*padding ) * (feature_in_size+2*padding ) + x * (feature_in_size+2*padding ) + y]
                            = feature_in[(j/32) * feature_in_size * feature_in_size +(x-padding)*feature_in_size + (y-padding)][j%32];
                            //input_feature[(j/32) * feature_in_size * feature_in_size +(x-padding)*feature_in_size + (y-padding)].range(16*(j%32)+15,16*(j%32));

    cout <<"software processing..." << endl;
    for(i = 0 ; i < num_output; i++)
    {
        for(x = 0 ; x < feature_in_size-kernel_size+1+2*padding; x+=stride)
            for(y = 0; y < feature_in_size-kernel_size+1+2*padding;y+=stride)
            {
                //temp = bias[i].range(15,0);
                //temp = 0;
                temp = bias[i];
                for(j = 0 ; j < num_input; j++)
                {
                    for(k = 0 ; k < kernel_size;k++)
                    {
                        for(z = 0 ; z < kernel_size; z++)
                        {

                            temp += (int)(temp_array[j * (feature_in_size+2*padding ) * (feature_in_size+2*padding ) + x * (feature_in_size+2*padding) + y + (k*(feature_in_size+2*padding) + z)])
                                    *(int)(weight[(i/32)*num_input*kernel_size*kernel_size +j*kernel_size*kernel_size + k*kernel_size + z][i%32])/64;
                        }
                    }
                }
                feature_out[(i/32) *feature_out_size * feature_out_size + (x/stride) * feature_out_size + y/stride][i%32]= (temp < 0) ? 0 : temp;
                // if( feature_out[(i/32) *feature_out_size * feature_out_size + (x/stride) * feature_out_size + y/stride][i%32] <0)
                // {
                //     feature_out[(i/32) *feature_out_size * feature_out_size + (x/stride) * feature_out_size + y/stride][i%32] = 0;
                //     //cout <<"!!"<< i << ":" <<x << y;
                // } 
            }
    }
}


void software_validate_pooling(int num_input,
                               int feature_in_size,
                               int feature_out_size,
                               short int feature_in[][32],
                               short int feature_out[][32])     //for padding = 0   stride = 2   kernel_size = 3
{

    int i,j,k,x,y,z;
    int temp_array[9] = {0};
    int temp_max = 0;
    for(i = 0 ; i < num_input; i++)
    {
        for(j = 0 ; j < feature_in_size - 2; j+=2)
        {
            for(k = 0 ; k <  feature_in_size - 2; k+=2)
            {
                for(x = 0 ; x < 3; x++)
                    for(y = 0 ; y < 3; y++)
                        temp_array[x*3+y] = feature_in[i/32*feature_in_size*feature_in_size + j*feature_in_size + k + (x*feature_in_size+y)][i%32];

                    
                temp_max = temp_array[0];

                for(z = 1; z < 9; z++)
                    if(temp_array[z] > temp_max)
                        temp_max = temp_array[z];
                
                feature_out[i/32*feature_out_size*feature_out_size + (j/2) * feature_out_size + k/2][i%32] = temp_max;
            }
        }
    }
}