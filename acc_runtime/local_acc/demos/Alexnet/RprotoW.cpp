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

/*#define CNN_OPEN_BINARY(filename) open(filename, O_RDONLY)
#define CNN_OPEN_TXT(filename) open(filename, O_RDONLY)


using google::protobuf::io::FileInputStream;
using google::protobuf::io::FileOutputStream;
using google::protobuf::io::ZeroCopyInputStream;
using google::protobuf::io::CodedInputStream;
using google::protobuf::io::ZeroCopyOutputStream;
using google::protobuf::io::CodedOutputStream;
using google::protobuf::Message;

void create_net_from_caffe_prototxt(const std::string& caffeprototxt,int input_param[]);
void reload_weight_from_caffe_protobinary(const std::string& caffebinary,int input_param[],short int weight[][32],int* bias);*/
void load_fc(const LayerParameter& src,int num_input,int num_output,float* weight,float* bias);
void fc_software(float* feature_in,float* feature_out,float* weight,float* bias,int num_input,int num_output);




short int weight[100000][32] = {0};
int bias[1500] = {0};
short int image_in[227*227][32] = {0};
short int feature_1[70000][32] = {0};
short int feature_2[70000][32] = {0};

short int feature_out_soft[70000][32]={0};
short int pooling_out_soft[3*27*27][32] = {0};
short int padding_temp_array[70000][32] = {0};
short int feature_bank_for_layer3[1500][32] = {0};
short int feature_bank_for_layer4[1500][32] = {0};
short int feature_bank_for_layer6[1200][32] = {0};
short int feature_bank_for_layer7[1200][32] = {0};
short int feature_bank_for_layer8[1200][32] = {0};
int index_list_fc[3] = {16,19,22};
int input_num_list_fc[3] = {9216,4096,4096};
int output_num_list_fc[3] = {4096,4096,1000};
float weight_fc5[37748736] = {0};
float weight_fc6[16777216] = {0};
float weight_fc7[4096000] = {0};
float bias_fc5[4096] = {0};
float bias_fc6[4096] = {0};
float bias_fc7[1000] = {0};
float feature_bank_for_fc1[9216] = {0};
float feature_fc_1[4096] = {0};
float feature_fc_2[4096] = {0};


int index_list[8] = {1,5,5,9,11,11,13,13};
int input_num_list[8] = {3,48,48,256,192,192,192,192};
int output_num_list[8] = {96,128,128,384,192,192,128,128};
int conv_stride_list[8] = {4,1,1,1,1,1,1,1};
int conv_padding_list[8] = {0,2,2,1,1,1,1,1};
int conv_kernel_size_list[8] = {11,5,5,3,3,3,3,3};
//int weight_offset_list[8] = {0,11616,18016,24416,52064,62432,72800,79712};
int weight_offset_list[8] = {0,1452,7852,14252,41900,52268,62636,69548};
//int bias_offset_list[8] = {0,96,224,352,736,928,1120,1248};
int bias_offset_list[8] = {0,128,256,384,768,960,1152,1280};
int conv_in_size_list[8] = {227,27,27,13,13,13,13,13};
int conv_out_size_list[8] = {55,27,27,13,13,13,13,13};
short int feature_out1[55*55*10][32]={0};
short int feature_out2[55*55*10][32]={0};
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
int main()
{
    int i,j,k,x,y,z;


    //1.generate input feature
    const int feature_in_size = 227;
    Mat frame = imread("lena.png");
    Mat smallImage;
    Mat splitImage[3];
    resize(frame,smallImage,Size(feature_in_size,feature_in_size));
    split(smallImage,splitImage);

    for(int i = 0 ; i < feature_in_size*feature_in_size; i++)
        for(int j = 0 ; j < 3;j++)
        {
            //image_in[i][j]=0;
           // image_in[i][j]=(short int)((splitImage[j].at<uchar>(i/feature_in_size,i%feature_in_size)) << 6);
            //image_in[i][j]=1*64;//(short int)(rand()%3-1)*64;
            image_in[i][j]=(rand()%2)*64;//(short int)(rand()%3-1)*64;
            //cout <<image_in[i][j] <<" ";
        }


   

    //2.generate random weight
        //layer1   weight 
        for(i = 0 ; i < 128; i++)
            for(j = 0 ; j < 3; j++)
                for(k = 0 ; k < 11*11; k++)
                    weight[i/32*3*11*11 + j*11*11 + k][i%32] = (rand()%2)*1;// (rand()%3-1)*16;
        //layer3   weight 
        for(i = 0 ; i < 128; i++)
            for(j = 0 ; j < 64; j++)
                for(k = 0 ; k < 5*5; k++)
                    weight[weight_offset_list[1] + i/32*64*5*5 + j*5*5 + k][i%32] =   (rand()%3 ==0 ) ? 1 : 0;//(rand()%2)*1;//(rand()%3-1)*16;

        //layer4   weight
        for(i = 0 ; i < 128; i++)
            for(j = 0 ; j < 64; j++)
                for(k = 0 ; k < 5*5; k++)
                    weight[weight_offset_list[2] + i/32*64*5*5 + j*5*5 + k][i%32] =  (rand()%3 ==0 ) ? 1 : 0;//(rand()%2)*1;//(rand()%3-1)*16;

        //layer7   weight
        for(i = 0 ; i < 384; i++)
            for(j = 0 ; j < 256; j++)
                for(k = 0 ; k < 3*3; k++)
                    weight[weight_offset_list[3] + i/32*256*3*3 + j*3*3 + k][i%32] = (rand()%8 ==0 ) ? 1 : 0;//(rand()%2)*1;//(rand()%2-1)*16;//(rand()%3-1)*16;//32;//(rand()%3-1)*8;//(rand()%3-1)*16;

        //layer8   weight
        for(i = 0 ; i < 192; i++)
            for(j = 0 ; j < 192; j++)
                for(k = 0 ; k < 3*3; k++)
                    weight[weight_offset_list[4] + i/32*192*3*3 + j*3*3 + k][i%32] = (rand()%8 ==0 ) ? 1 : 0;//(rand()%3-1)*8;//(rand()%3-1)*16;

        //layer9   weight
        for(i = 0 ; i < 192; i++)
            for(j = 0 ; j < 192; j++)
                for(k = 0 ; k < 3*3; k++)
                    weight[weight_offset_list[5] + i/32*192*3*3 + j*3*3 + k][i%32] = (rand()%8 ==0 ) ? 1 : 0;//(rand()%3-1)*8;//(rand()%3-1)*16;
        //layer10  weight
        for(i = 0 ; i < 128; i++)
            for(j = 0 ; j < 192; j++)
                for(k = 0 ; k < 3*3; k++)
                    weight[weight_offset_list[6] + i/32*192*3*3 + j*3*3 + k][i%32] = (rand()%16 ==0 ) ? 1 : 0;//(rand()%3-1)*8;//(rand()%3-1)*16;

        //layer11  weight
        for(i = 0 ; i < 128; i++)
            for(j = 0 ; j < 192; j++)
                for(k = 0 ; k < 3*3; k++)
                    weight[weight_offset_list[7] + i/32*192*3*3 + j*3*3 + k][i%32] = (rand()%16 ==0 ) ? 1 : 0;//(rand()%3-1)*8;//(rand()%3-1)*16;


    //3.generate random bias
        for(i=0;i<2000;i++)
            bias[i] = 4;// 0.5*64;//(rand()%10-5)*64;

    ACC_ctrl acc0(
                  ACC0_LAYER_NUM_MEM_ADDR,
                  ACC0_CONV_PARA_MEM_ADDR,
                  ACC0_POOL_PARA_MEM_ADDR,
                  ACC0_BIAS_IN_MEM_ADDR,
                  ACC0_CTRL_PORT_ADDR
                  ); 

    ACC_ctrl acc1(
                  ACC1_LAYER_NUM_MEM_ADDR,
                  ACC1_CONV_PARA_MEM_ADDR,
                  ACC1_POOL_PARA_MEM_ADDR,
                  ACC1_BIAS_IN_MEM_ADDR,
                  ACC1_CTRL_PORT_ADDR
                  );

    ACC_ctrl acc2(
                  ACC2_LAYER_NUM_MEM_ADDR,
                  ACC2_CONV_PARA_MEM_ADDR,
                  ACC2_POOL_PARA_MEM_ADDR,
                  ACC2_BIAS_IN_MEM_ADDR,
                  ACC2_CTRL_PORT_ADDR
                  );

    acc0.write_data(image_in,227*227*64,0);
    ACC_ctrl::write_weight(weight,100000*64,WEIGHT_MEM_ADDR);
    acc0.write_bias(bias,2000);
    acc1.write_bias(bias,2000);
    acc2.write_bias(bias,2000);

    //4.config ACC

/*    int pooling_kernel_size = 3;
    int pooling_stride = 2;
    int pooling_padding = 0;
    int pooling_out_size[8] = {27,13,13,0,0,0,6,6};*/
    uint32_t ctrl_list[16];

    cout <<"network data ready!"<<endl;
    cout <<"==========PASS:load weigth to FPGA DDR"<<endl;
   
    uint32_t para_list[16];
    uint32_t para_conv_array[3][16][16];
    uint32_t para_pool_array[3][16][16];


//===============================================================ACC0

//===layer1   conv                3-->128
    conv_layer_construct(           
                             3,                         // input_num,
                             128,                       // output_num,
                             11,                        // kernel_size,
                             227,                       // feature_in_size,
                             55,                        // feature_out_size,
                             4,                         // stride,
                             0,                         // padding,
                             1,                         // act,  
                             0,//(WEIGHT_MEM_ADDR)/32,      // weight_offset,
                             0,                         // bias_offset,
                             0,//(ACC_DATA_IN_MEM_ADDR_0)/32,    // data_in_offset,
                             0x10000,//(ACC_DATA_OUT_MEM_ADDR_0)/32,   // data_out_offset,
                             para_list                  // para_list
                         );
    for(i = 0 ; i < 16 ; i++)
    {
        para_conv_array[0][0][i] = para_list[i];
        para_pool_array[0][0][i] = para_list[i];
    }
//===layer2   pooling            128--->128
    pooling_layer_construct(
                              55,                                 // feature_in_size,
                              128,                                // input_num,
                              3,                                  // kernel_size,
                              27,                                 // feature_out_size,
                              2,                                  // stride,
                              0,                                  // padding,
                              1,                                  // act,
                              0x10000,                            // data_in_offset,
                              0,                                  // data_out_offset,
                              para_list                           // para_list
                            );
    for(i = 0 ; i < 16 ; i++)
    {
        para_conv_array[0][1][i] = para_list[i];
        para_pool_array[0][1][i] = para_list[i];   
    }
   



//===============================================================ACC1
//====layer3   conv                64--->128
    conv_layer_construct(           
                           64,                                              // input_num,
                           128,                                             // output_num,
                           5,                                               // kernel_size,
                           27,                                              // feature_in_size,
                           27,                                              // feature_out_size,
                           1,                                               // stride,
                           2,                                               // padding,
                           1,                                               // act,  
                           (WEIGHT_MEM_ADDR)/32+weight_offset_list[1],    //(WEIGHT_MEM_ADDR)/32+weight_offset_list[1],      // weight_offset,
                           0,    //0+bias_offset_list[1],                           // bias_offset,
                           0,    //(ACC_DATA_IN_MEM_ADDR_0)/32,                     // data_in_offset,
                           0x10000,    //(ACC_DATA_OUT_MEM_ADDR_0)/32,                    // data_out_offset,
                           para_list                                        // para_list
                         );
    for(i = 0 ; i < 16 ; i++)
    {
        para_conv_array[1][0][i] = para_list[i];
        para_pool_array[1][0][i] = para_list[i];   
    }

//===layer4   conv                64--->128
    conv_layer_construct(           
                         64,                                                // input_num,
                         128,                                               // output_num,
                         5,                                                 // kernel_size,
                         27,                                                // feature_in_size,
                         27,                                                // feature_out_size,
                         1,                                                 // stride,
                         2,                                                 // padding,
                         1,                                                 // act,  
                         (WEIGHT_MEM_ADDR)/32+weight_offset_list[2],        // weight_offset,
                         0,                                                 // bias_offset,
                         1458,        // (ACC_DATA_IN_MEM_ADDR_0)/32+27*27*2,                       // data_in_offset,
                         0x10000+2916,        // (ACC_DATA_OUT_MEM_ADDR_0)/32+27*27*4,                      // data_out_offset,
                         para_list                                          // para_list
                         );
    for(i = 0 ; i < 16 ; i++)
    {
        para_conv_array[1][1][i] = para_list[i];
        para_pool_array[1][1][i] = para_list[i];   
    }

//===layer5   pooling             256--->256
    pooling_layer_construct(
                            27,                                               // feature_in_size,
                            256,                                              // input_num,
                            3,                                                // kernel_size,
                            13,                                               // feature_out_size,
                            2,                                                // stride,
                            0,                                                // padding,
                            1,                                                // act,
                            0x10000,    //(ACC_DATA_OUT_MEM_ADDR_0)/32,                     // data_in_offset,
                            0,          //(ACC_DATA_IN_MEM_ADDR_0)/32,                      // data_out_offset,
                            para_list                                         // para_list
                            );

    for(i = 0 ; i < 16 ; i++)
    {
        para_conv_array[1][2][i] = para_list[i];
        para_pool_array[1][2][i] = para_list[i];   
    }

//===layer7   conv                256--->384
    conv_layer_construct(           
                            256,                                                // input_num,
                            384,                                                // output_num,
                            3,                                                  // kernel_size,
                            13,                                                 // feature_in_size,
                            13,                                                 // feature_out_size,
                            1,                                                  // stride,
                            1,                                                  // padding,
                            1,                                                  // act,  
                            (WEIGHT_MEM_ADDR)/32+weight_offset_list[3],         // weight_offset,
                            0,                                                  // bias_offset,
                            0,                                                  // data_in_offset,
                            0x10000,                                            // data_out_offset,
                            para_list                                           // para_list
                         );
    for(i = 0 ; i < 16 ; i++)
    {
        para_conv_array[1][3][i] = para_list[i];
        para_pool_array[1][3][i] = para_list[i];   
    }
//===============================================================ACC2
//===layer8   conv                192--->192
    conv_layer_construct(           
                           192,                                             // input_num,
                           192,                                             // output_num,
                           3,                                               // kernel_size,
                           13,                                              // feature_in_size,
                           13,                                              // feature_out_size,
                           1,                                               // stride,
                           1,                                               // padding,
                           1,                                               // act,  
                           (WEIGHT_MEM_ADDR)/32+weight_offset_list[4],      // weight_offset,
                           0,                                               // bias_offset,
                           0x10000,                                         // data_in_offset,
                           0,                                               // data_out_offset,
                           para_list                                        // para_list
                         );
    for(i = 0 ; i < 16 ; i++)
    {
        para_conv_array[2][0][i] = para_list[i];
        para_pool_array[2][0][i] = para_list[i];   
    }
//===layer9   conv                192--->192
    conv_layer_construct(           
                            192,                                              // input_num,
                            192,                                              // output_num,
                            3,                                                // kernel_size,
                            13,                                               // feature_in_size,
                            13,                                               // feature_out_size,
                            1,                                                // stride,
                            1,                                                // padding,
                            1,                                                // act,  
                            (WEIGHT_MEM_ADDR)/32+weight_offset_list[5],       // weight_offset,
                            0,                                                //0+bias_offset_list[5],                            // bias_offset,
                            0x10000+1014,//(ACC_DATA_OUT_MEM_ADDR_0)/32+13*13*6,                    // data_in_offset,
                            0+1014,//(ACC_DATA_IN_MEM_ADDR_0)/32+13*13*6,                     // data_out_offset,
                            para_list                                         // para_list
                         );
    for(i = 0 ; i < 16 ; i++)
    {
        para_conv_array[2][1][i] = para_list[i];
        para_pool_array[2][1][i] = para_list[i];   
    }

//===layer10  conv                192--->128
    conv_layer_construct(           
                            192,                                              // input_num,
                            128,                                              // output_num,
                            3,                                                // kernel_size,
                            13,                                               // feature_in_size,
                            13,                                               // feature_out_size,
                            1,                                                // stride,
                            1,                                                // padding,
                            1,                                                // act,  
                            (WEIGHT_MEM_ADDR)/32+weight_offset_list[6],       // weight_offset,
                            0,                      //0+bias_offset_list[6],                            // bias_offset,
                            0,                        //(ACC_DATA_IN_MEM_ADDR_0)/32,                     // data_in_offset,
                            0x10000,                 //(ACC_DATA_OUT_MEM_ADDR_0)/32,                      // data_out_offset,
                            para_list                                         // para_list
                         );
    for(i = 0 ; i < 16 ; i++)
    {
        para_conv_array[2][2][i] = para_list[i];
        para_pool_array[2][2][i] = para_list[i];   
    }


//===layer11  conv                192--->128
    conv_layer_construct(           
                         192,                                           // input_num,
                         128,                                           // output_num,
                         3,                                             // kernel_size,
                         13,                                            // feature_in_size,
                         13,                                            // feature_out_size,
                         1,                                             // stride,
                         1,                                             // padding,
                         1,                                             // act,  
                         (WEIGHT_MEM_ADDR)/32+weight_offset_list[7],    // weight_offset,
                         0,                                     //0+bias_offset_list[7],                         // bias_offset,
                         0+1014,                                      //(ACC_DATA_IN_MEM_ADDR_0)/32+13*13*6,                  // data_in_offset,
                         0x10000+676,                                      //(ACC_DATA_OUT_MEM_ADDR_0)/32+13*13*4,                   // data_out_offset,
                         para_list                                      // para_list
                         );
    for(i = 0 ; i < 16 ; i++)
    {
        para_conv_array[2][3][i] = para_list[i];
        para_pool_array[2][3][i] = para_list[i];   
    }
//====layer12  pooling            256--->256
    pooling_layer_construct(
                            13,                                       // feature_in_size,
                            256,                                      // input_num,
                            3,                                        // kernel_size,
                            6,                                        // feature_out_size,
                            2,                                        // stride,
                            0,                                        // padding,
                            1,                                        // act,
                            0x10000,                           //(ACC_DATA_OUT_MEM_ADDR_0)/32,              // data_in_offset,
                            0,                           //(ACC_DATA_IN_MEM_ADDR_0)/32,             // data_out_offset,
                            para_list                                 // para_list
                            );
    for(i = 0 ; i < 16 ; i++)
    {
        para_conv_array[2][4][i] = para_list[i];
        para_pool_array[2][4][i] = para_list[i];   
    }
    int mode = 0;
    timeval begin,end;
    while(1)
    {
        gettimeofday(&begin,NULL);
        if(mode == 0)
        {
            acc0.start_process(2,para_conv_array[0],para_pool_array[0],0);
            acc1.start_process(4,para_conv_array[1],para_pool_array[1],1);
            acc2.start_process(5,para_conv_array[2],para_pool_array[2],2);
        }
        else if(mode == 1)
        {
            acc0.start_process(2,para_conv_array[0],para_pool_array[0],2);
            acc1.start_process(4,para_conv_array[1],para_pool_array[1],0);
            acc2.start_process(5,para_conv_array[2],para_pool_array[2],1);
        }
        else if(mode == 2)
        {
            acc0.start_process(2,para_conv_array[0],para_pool_array[0],1);
            acc1.start_process(4,para_conv_array[1],para_pool_array[1],2);
            acc2.start_process(5,para_conv_array[2],para_pool_array[2],0);
        }
        acc1.checkEnd();
        gettimeofday(&end,NULL);
        cout <<"process time    " << (end.tv_usec - begin.tv_usec)/1000.0 <<"ms" <<endl;

        mode++;
        if(mode == 3)
        {
            mode = 0;
            break;
        }
    }


    



    acc2.read_data(feature_2,13*13*8*64,0,0);

    software_validate_conv(3,128,11,4,0,227,55,image_in,feature_out1,weight,bias);                              //layer1
    software_validate_pooling(128,
                               55,
                               27,
                               feature_out1,
                               feature_out2);                                                                   //layer2
    software_validate_conv(64,128,5,1,2,27,27,feature_out2,feature_out1,weight+weight_offset_list[1],bias);     //layer3
    software_validate_conv(64,128,5,1,2,27,27,feature_out2+1458,feature_out1+2916,weight+weight_offset_list[2],bias);     //layer4
    software_validate_pooling(256,
                               27,
                               13,
                               feature_out1,
                               feature_out2);                                                                   //layer5
    software_validate_conv(256,384,3,1,1,13,13,feature_out2,feature_out1,weight+weight_offset_list[3],bias);     //layer7
    software_validate_conv(192,192,3,1,1,13,13,feature_out1,feature_out2,weight+weight_offset_list[4],bias);     //layer8
    software_validate_conv(192,192,3,1,1,13,13,feature_out1+1014,feature_out2+1014,weight+weight_offset_list[5],bias);     //layer9
    software_validate_conv(192,128,3,1,1,13,13,feature_out2,feature_out1,weight+weight_offset_list[6],bias);     //layer10
    software_validate_conv(192,128,3,1,1,13,13,feature_out2+1014,feature_out1+676,weight+weight_offset_list[7],bias);     //layer11
    software_validate_pooling(256,
                               13,
                               6,
                               feature_out1,
                               feature_out2);        //layer12


 for(k = 0 ; k < 256;k++)
 {
    cout <<"========="<<k<<"===========" << endl;
    for(i = 0 ; i <10; i++)
    {
        for(j = 0 ; j < 10; j++)
            cout << setw(8) << feature_2[k/32*13*13 + i*13 + j][k%32]/64.0 <<" ";
        cout <<"|";
        for(j = 0 ; j < 10; j++)
            cout << setw(8) << feature_out2[k/32*13*13 + i*13 + j][k%32]/64.0<<" ";

        cout << endl;
    }
 }

 for(k = 0 ; k < 256;k++)
 {
    cout <<"========="<<k<<"===========" << endl;
    for(i = 0 ; i <13; i++)
    {
        for(j = 0 ; j < 13; j++)
            if((feature_2[k/32*13*13 + i*13 + j][k%32]/64.0)!=(feature_out2[k/32*13*13 + i*13 + j][k%32]/64.0))
            {
                cout <<"error"<< i << " "<< j <<endl;
                //while(1);
            }

    }
 }



/*
    for(i = 0 ; i <20; i++)
    {
        for(j = 0 ; j < 20; j++)
            cout << setw(5) << feature_2[i*55+j][1]/64.0 <<" ";
        cout << endl;
    }
    // cout << (0x10000000) << endl;
    // cout << (0x10000000)/64 << endl;
   


    cout <<"====================" << endl;
    k = 0;
    for(i = 0; i <20;i++)
    {
        for(j = 0 ; j < 20; j++)
            cout << setw(5) << feature_out1[i*55+j][1]<<" ";
        cout << endl;
    }*/

    // for(i = 1 ; i < 14; i++)
    // {
    //     cout << "i= " << i << endl; 
    //     acc0.start_process(i,para_conv_array,para_pool_array);
    // }
//=====================================================fc

   // acc0.read_data(feature_1,6*6*8*64,2,0);
   //===================================================================================================fc
/*    for(i = 0 ; i < (output_num_list[6] + output_num_list[7]); i++)
        for(j = 0; j < pooling_out_size[7]*pooling_out_size[7]; j++)
            feature_bank_for_fc1[i*pooling_out_size[6]*pooling_out_size[6]+j] = feature_1[(i/32)*pooling_out_size[6]*pooling_out_size[6]+j][i%32]/64.0;
    fc_software(feature_bank_for_fc1,feature_fc_1,weight_fc5,bias_fc5,input_num_list_fc[0],output_num_list_fc[0]);
    fc_software(feature_fc_1,feature_fc_2,weight_fc6,bias_fc6,input_num_list_fc[1],output_num_list_fc[1]);
    fc_software(feature_fc_2,feature_fc_1,weight_fc7,bias_fc7,input_num_list_fc[2],output_num_list_fc[2]);*/
/*    for(i = 0 ; i < 1000; i++)
        cout <<setw(4) << i <<":" << feature_fc_1[i] << endl;
    cout <<"test finish" << endl;*/
    return 0;
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



/*void fc_software(float* feature_in,float* feature_out,float* weight,float* bias,int num_input,int num_output)
{
    int i,j,k;
    float temp;
    for(i = 0 ; i < num_output; i++)
    {
        temp = bias[i];
        for(j = 0 ; j < num_input; j++)
            temp += feature_in[j]*weight[num_input*i+j];
        feature_out[i] = (temp>0)? temp : 0;
    }
}
*/





/*void load(const LayerParameter& src,int num_input,int num_output,int kernel_size,short int weight[][32],int* bias,int weight_offset,int bias_offset) 
{
    int src_idx = 0;
    for(int o = 0; o < num_output; o++) 
        for(int i = 0; i < num_input; i++) 
            for(int x = 0; x < kernel_size; x++) 
                for(int y = 0 ; y < kernel_size; y++)
                    weight[weight_offset + i/32 * (kernel_size * kernel_size * num_output) + o * kernel_size * kernel_size + x * kernel_size + y][i%32] 
                                                                                                = (short int)(src.blobs(0).data(src_idx++) * 64 );
    for (int o = 0; o < num_output; o++)
         bias[bias_offset + o] = src.blobs(1).data(o) * 64;
}

void load_fc(const LayerParameter& src,int num_input,int num_output,float* weight,float* bias)
{
    int src_idx = 0;
    int i,j;
    for(i = 0 ; i < num_output; i++)
        for(j = 0 ; j < num_input; j++)
            weight[i*num_input+j] = src.blobs(0).data(src_idx++);
    for(i = 0 ; i < num_output; i++)
        bias[num_output] = src.blobs(1).data(i);
}

void read_proto_from_text(const std::string& prototxt,
                          google::protobuf::Message *message) 
{
    int fd = CNN_OPEN_TXT(prototxt.c_str());
    if (fd == -1) 
        cout<<"file not found: "<<prototxt<<endl;

    google::protobuf::io::FileInputStream input(fd);
    input.SetCloseOnDelete(true);

    if (!google::protobuf::TextFormat::Parse(&input, message)) 
        cout<<"failed to parse"<<endl;
}

void create_net_from_caffe_net(const caffe::NetParameter& layer,int input_param[])
{
    cout << "create net from caffe net" << endl;
    caffe_layer_vector src_net(layer);
    if (layer.input_shape_size() > 0) 
    {
        input_param[0]  = static_cast<int>(layer.input_shape(0).dim(1));
        cout << "input_param[0]: " << input_param[0] << endl;
        input_param[1] = static_cast<int>(layer.input_shape(0).dim(2));
        cout << "input_param[1]: " << input_param[1] << endl;
    }
    else if (layer.layer(0).has_input_param()) 
    {
        cout << "input has layer param " << endl;
        input_param[0] = static_cast<int>(layer.layer(0).input_param().shape(0).dim(1));
        input_param[1] = static_cast<int>(layer.layer(0).input_param().shape(0).dim(2));
        cout << "input_param[0]: " << input_param[0] << endl;
        cout << "input_param[1]: " << input_param[1] << endl;
    }
}

void create_net_from_caffe_prototxt(const std::string& caffeprototxt,int input_param[])
{
    caffe::NetParameter np;
    read_proto_from_text(caffeprototxt, &np);
    //while(1);
    cout <<"net_name from prototxt: "<<np.name()<<endl;
    create_net_from_caffe_net(np,input_param);
}

void read_proto_from_binary(const std::string& protobinary,
                            google::protobuf::Message *message) 
{
    int fd = CNN_OPEN_BINARY(protobinary.c_str());
    google::protobuf::io::FileInputStream rawstr(fd);
    google::protobuf::io::CodedInputStream codedstr(&rawstr);

    rawstr.SetCloseOnDelete(true);
    codedstr.SetTotalBytesLimit(std::numeric_limits<int>::max(),
                                std::numeric_limits<int>::max() / 2);
    if (!message->ParseFromCodedStream(&codedstr)) 
        cout<<"failed to parse"<<endl;
}

void reload_weight_from_caffe_net(const caffe::NetParameter& layer,int input_param[],short int weight[][32],int* bias)
{
    caffe_layer_vector src_net(layer);
    for(int i = 0 ; i < 8;i++)
    {
        load(src_net[index_list[i]], input_num_list[i], output_num_list[i],conv_kernel_size_list[i],weight,bias,weight_offset_list[i],bias_offset_list[i]);
    }

    load_fc(src_net[index_list_fc[0]],input_num_list_fc[0],output_num_list_fc[0],weight_fc5,bias_fc5);
    load_fc(src_net[index_list_fc[1]],input_num_list_fc[1],output_num_list_fc[1],weight_fc6,bias_fc6);
    load_fc(src_net[index_list_fc[2]],input_num_list_fc[2],output_num_list_fc[2],weight_fc7,bias_fc7);
    cout <<"load successfully!"<<endl;
}
void reload_weight_from_caffe_protobinary(const std::string& caffebinary,int input_param[],short int weight[][32],int* bias)
{
    caffe::NetParameter np;
    read_proto_from_binary(caffebinary, &np);
    reload_weight_from_caffe_net(np,input_param,weight,bias);
}*/


/*

//=======================================================layer3   conv                64--->128
    conv_layer_construct(           
                           64,                                              // input_num,
                           128,                                             // output_num,
                           5,                                               // kernel_size,
                           27,                                              // feature_in_size,
                           27,                                              // feature_out_size,
                           1,                                               // stride,
                           2,                                               // padding,
                           1,                                               // act,  
                           (WEIGHT_MEM_ADDR)/32+weight_offset_list[1],      // weight_offset,
                           0+bias_offset_list[1],                           // bias_offset,
                           (ACC_DATA_IN_MEM_ADDR_0)/32,                     // data_in_offset,
                           (ACC_DATA_OUT_MEM_ADDR_0)/32,                    // data_out_offset,
                           para_list                                        // para_list
                         );
    for(i = 0 ; i < 16 ; i++)
    {
        para_conv_array[2][i] = para_list[i];
        para_pool_array[2][i] = para_list[i];   
    }

//=======================================================layer4   conv                64--->128
    conv_layer_construct(           
                         64,                                                // input_num,
                         128,                                               // output_num,
                         5,                                                 // kernel_size,
                         27,                                                // feature_in_size,
                         27,                                                // feature_out_size,
                         1,                                                 // stride,
                         2,                                                 // padding,
                         1,                                                 // act,  
                         (WEIGHT_MEM_ADDR)/32+weight_offset_list[2],        // weight_offset,
                         0+bias_offset_list[2],                             // bias_offset,
                         (ACC_DATA_IN_MEM_ADDR_0)/32+27*27*2,                       // data_in_offset,
                         (ACC_DATA_OUT_MEM_ADDR_0)/32+27*27*4,                      // data_out_offset,
                         para_list                                          // para_list
                         );
    for(i = 0 ; i < 16 ; i++)
    {
        para_conv_array[3][i] = para_list[i];
        para_pool_array[3][i] = para_list[i];   
    }





//=======================================================layer5   pooling             128--->128
    pooling_layer_construct(
                            27,                                               // feature_in_size,
                            128,                                              // input_num,
                            3,                                                // kernel_size,
                            13,                                               // feature_out_size,
                            2,                                                // stride,
                            0,                                                // padding,
                            1,                                                // act,
                            (ACC_DATA_OUT_MEM_ADDR_0)/32,                     // data_in_offset,
                            (ACC_DATA_IN_MEM_ADDR_0)/32,                      // data_out_offset,
                            para_list                                         // para_list
                            );

    for(i = 0 ; i < 16 ; i++)
    {
        para_conv_array[4][i] = para_list[i];
        para_pool_array[4][i] = para_list[i];   
    }


//=======================================================layer6   pooling             128--->128
    pooling_layer_construct(
                            27,                                   // feature_in_size,
                            128,                                  // input_num,
                            3,                                    // kernel_size,
                            13,                                   // feature_out_size,
                            2,                                    // stride,
                            0,                                    // padding,
                            1,                                    // act,
                            (ACC_DATA_OUT_MEM_ADDR_0)/32+27*27*4,         // data_in_offset,
                            (ACC_DATA_IN_MEM_ADDR_0)/32+13*13*4,          // data_out_offset,
                            para_list                             // para_list
                            );
    for(i = 0 ; i < 16 ; i++)
    {
        para_conv_array[5][i] = para_list[i];
        para_pool_array[5][i] = para_list[i];   
    }

//=======================================================layer7   conv                256--->384
    conv_layer_construct(           
                            256,                                                // input_num,
                            384,                                                // output_num,
                            3,                                                  // kernel_size,
                            13,                                                 // feature_in_size,
                            13,                                                 // feature_out_size,
                            1,                                                  // stride,
                            1,                                                  // padding,
                            1,                                                  // act,  
                            (WEIGHT_MEM_ADDR)/32+weight_offset_list[3],         // weight_offset,
                            0+bias_offset_list[3],                              // bias_offset,
                            (ACC_DATA_IN_MEM_ADDR_0)/32,                        // data_in_offset,
                            (ACC_DATA_OUT_MEM_ADDR_0)/32,                       // data_out_offset,
                            para_list                                           // para_list
                         );
    for(i = 0 ; i < 16 ; i++)
    {
        para_conv_array[6][i] = para_list[i];
        para_pool_array[6][i] = para_list[i];   
    }
//=======================================================layer8   conv                192--->192
    conv_layer_construct(           
                           192,                                             // input_num,
                           192,                                             // output_num,
                           3,                                               // kernel_size,
                           13,                                              // feature_in_size,
                           13,                                              // feature_out_size,
                           1,                                               // stride,
                           1,                                               // padding,
                           1,                                               // act,  
                           (WEIGHT_MEM_ADDR)/32+weight_offset_list[4],      // weight_offset,
                           0+bias_offset_list[4],                           // bias_offset,
                           (ACC_DATA_OUT_MEM_ADDR_0)/32,                    // data_in_offset,
                           (ACC_DATA_IN_MEM_ADDR_0)/32,                     // data_out_offset,
                           para_list                                        // para_list
                         );
    for(i = 0 ; i < 16 ; i++)
    {
        para_conv_array[7][i] = para_list[i];
        para_pool_array[7][i] = para_list[i];   
    }
//=======================================================layer9   conv                192--->192
    conv_layer_construct(           
                            192,                                              // input_num,
                            192,                                              // output_num,
                            3,                                                // kernel_size,
                            13,                                               // feature_in_size,
                            13,                                               // feature_out_size,
                            1,                                                // stride,
                            1,                                                // padding,
                            1,                                                // act,  
                            (WEIGHT_MEM_ADDR)/32+weight_offset_list[5],       // weight_offset,
                            0+bias_offset_list[5],                            // bias_offset,
                            (ACC_DATA_OUT_MEM_ADDR_0)/32+13*13*6,                    // data_in_offset,
                            (ACC_DATA_IN_MEM_ADDR_0)/32+13*13*6,                     // data_out_offset,
                            para_list                                         // para_list
                         );
    for(i = 0 ; i < 16 ; i++)
    {
        para_conv_array[8][i] = para_list[i];
        para_pool_array[8][i] = para_list[i];   
    }
//=======================================================layer10  conv                192--->128
    conv_layer_construct(           
                            192,                                              // input_num,
                            128,                                              // output_num,
                            3,                                                // kernel_size,
                            13,                                               // feature_in_size,
                            13,                                               // feature_out_size,
                            1,                                                // stride,
                            1,                                                // padding,
                            1,                                                // act,  
                            (WEIGHT_MEM_ADDR)/32+weight_offset_list[6],       // weight_offset,
                            0+bias_offset_list[6],                            // bias_offset,
                            (ACC_DATA_IN_MEM_ADDR_0)/32,                     // data_in_offset,
                            (ACC_DATA_OUT_MEM_ADDR_0)/32,                      // data_out_offset,
                            para_list                                         // para_list
                         );
    for(i = 0 ; i < 16 ; i++)
    {
        para_conv_array[9][i] = para_list[i];
        para_pool_array[9][i] = para_list[i];   
    }
//=======================================================layer11  conv                192--->128
    conv_layer_construct(           
                         192,                                           // input_num,
                         128,                                           // output_num,
                         3,                                             // kernel_size,
                         13,                                            // feature_in_size,
                         13,                                            // feature_out_size,
                         1,                                             // stride,
                         1,                                             // padding,
                         1,                                             // act,  
                         (WEIGHT_MEM_ADDR)/32+weight_offset_list[7],    // weight_offset,
                         0+bias_offset_list[7],                         // bias_offset,
                         (ACC_DATA_IN_MEM_ADDR_0)/32+13*13*6,                  // data_in_offset,
                         (ACC_DATA_OUT_MEM_ADDR_0)/32+13*13*4,                   // data_out_offset,
                         para_list                                      // para_list
                         );
    for(i = 0 ; i < 16 ; i++)
    {
        para_conv_array[10][i] = para_list[i];
        para_pool_array[10][i] = para_list[i];   
    }

//=======================================================layer12  pooling             128--->128
    pooling_layer_construct(
                            13,                                       // feature_in_size,
                            128,                                      // input_num,
                            3,                                        // kernel_size,
                            6,                                        // feature_out_size,
                            2,                                        // stride,
                            0,                                        // padding,
                            1,                                        // act,
                            (ACC_DATA_OUT_MEM_ADDR_0)/32,              // data_in_offset,
                            (ACC_DATA_IN_MEM_ADDR_0)/32,             // data_out_offset,
                            para_list                                 // para_list
                            );
    for(i = 0 ; i < 16 ; i++)
    {
        para_conv_array[11][i] = para_list[i];
        para_pool_array[11][i] = para_list[i];   
    }


//=======================================================conv13   pooling             128--->128
      
    pooling_layer_construct(
                              13,                                       // feature_in_size,
                              128,                                      // input_num,
                              3,                                        // kernel_size,
                              6,                                        // feature_out_size,
                              2,                                        // stride,
                              0,                                        // padding,
                              1,                                        // act,
                              (ACC_DATA_OUT_MEM_ADDR_0)/32+13*13*4,              // data_in_offset,
                              (ACC_DATA_IN_MEM_ADDR_0)/32+6*6*4,             // data_out_offset,
                              para_list                                 // para_list
                            );

    for(i = 0 ; i < 16 ; i++)
    {
        para_conv_array[12][i] = para_list[i];
        para_pool_array[12][i] = para_list[i];   
    }*/