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

#define CNN_OPEN_BINARY(filename) open(filename, O_RDONLY)
#define CNN_OPEN_TXT(filename) open(filename, O_RDONLY)


using google::protobuf::io::FileInputStream;
using google::protobuf::io::FileOutputStream;
using google::protobuf::io::ZeroCopyInputStream;
using google::protobuf::io::CodedInputStream;
using google::protobuf::io::ZeroCopyOutputStream;
using google::protobuf::io::CodedOutputStream;
using google::protobuf::Message;

void create_net_from_caffe_prototxt(const std::string& caffeprototxt,int input_param[]);
void reload_weight_from_caffe_protobinary(const std::string& caffebinary,int input_param[],short int weight[][32],int* bias);
void load_fc(const LayerParameter& src,int num_input,int num_output,float* weight,float* bias);
void fc_software(float* feature_in,float* feature_out,float* weight,float* bias,int num_input,int num_output);



const int feature_in_size = 227;
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
int weight_offset_list[8] = {0,11616,18016,24416,52064,62432,72800,79712};
int bias_offset_list[8] = {0,96,224,352,736,928,1120,1248};
int conv_in_size_list[8] = {227,27,27,13,13,13,13,13};
int conv_out_size_list[8] = {55,27,27,13,13,13,13,13};
int main(int argn, char* argv[])
{
    int i,j,k,x,y,z;
    string caffemodel = argv[1];
    string prototxt   = argv[2];

    int pooling_kernel_size = 3;
    int pooling_stride = 2;
    int pooling_padding = 0;
    int pooling_out_size[8] = {27,13,13,0,0,0,6,6};

   
    cout << "Read prototxt file from: " << prototxt << endl;
    cout << "Read caffemodel file from: " << caffemodel << endl;
    int input_param[] = {0, 0};
    create_net_from_caffe_prototxt(argv[2], input_param);
    reload_weight_from_caffe_protobinary(argv[1], input_param,weight,bias);

    Mat frame = imread("lena.png");
    Mat smallImage;
    Mat splitImage[3];
    resize(frame,smallImage,Size(feature_in_size,feature_in_size));
    split(smallImage,splitImage);
    write_weight(weight,100000*64,WEIGHT_MEM);
    cout <<"PASS:load weigth to FPGA DDR"<<endl;
    write_bias(bias,1500,BIAS_MEM);
    cout <<"PASS:load bias to FPGA DDR"<<endl;
   // write_weight_bias(weight,100000*64,bias,1500*4,WEIGHT_MEM,BIAS_MEM);

    for(int i = 0 ; i < feature_in_size*feature_in_size; i++)
        for(int j = 0 ; j < 3;j++)
            image_in[i][j]=(short int)((splitImage[j].at<uchar>(i/feature_in_size,i%feature_in_size)) << 6) ;
    write_data(image_in,feature_in_size*feature_in_size*64,DATA_MEM_0_ADDR);

//====================================================================================================layer1 conv
    conv_layer_construct(
                        input_num_list[0],                     
                        output_num_list[0],                      
                        conv_kernel_size_list[0],                   
                        conv_in_size_list[0],                   
                        conv_out_size_list[0],                   
                        conv_stride_list[0],                       
                        conv_padding_list[0],                                 
                        1,                                  
                        feature_1,                      
                        conv_out_size_list[0]* conv_out_size_list[0]*64*(output_num_list[0]/32),        
                        WEIGHT_OFFSET,
                        BIAS_OFFSET,
                        DATA_MEM_0_OFFSET,
                        DATA_MEM_1_OFFSET,
                        DATA_MEM_1_ADDR
                        );
//====================================================================================================layer1 pooling
    pooling_layer_construct(
                            conv_out_size_list[0],    
                            output_num_list[0],     
                            pooling_kernel_size,  
                            pooling_out_size[0], 
                            pooling_stride,  
                            pooling_padding,       
                            1,        
                            feature_2,   
                            pooling_out_size[0]*pooling_out_size[0]*(output_num_list[0]/32)*64,  
                            DATA_MEM_1_OFFSET, 
                            DATA_MEM_2_OFFSET,
                            DATA_MEM_2_ADDR
                            ); 
    for(i = pooling_out_size[0]*pooling_out_size[0] ; i < pooling_out_size[0]*pooling_out_size[0]*2 ; i++)
        for(j = 16 ; j < 32; j++)
            feature_bank_for_layer3[i-pooling_out_size[0]*pooling_out_size[0]][j-16] = feature_2[i][j];
    for(i = pooling_out_size[0]*pooling_out_size[0]*2; i < pooling_out_size[0]*pooling_out_size[0]*3; i++)
        for(j = 0 ; j < 16; j++)
            feature_bank_for_layer3[i-pooling_out_size[0]*pooling_out_size[0]*2][j+16] = feature_2[i][j];
    for(i = pooling_out_size[0]*pooling_out_size[0]*2; i < pooling_out_size[0]*pooling_out_size[0]*3; i++)
        for(j = 16; j < 32; j++)
            feature_bank_for_layer3[i-pooling_out_size[0]*pooling_out_size[0]][j-16] = feature_2[i][j];
//===================================================================================================layer2 conv
    conv_layer_construct(
                        input_num_list[1],                         
                        output_num_list[1],                        
                        conv_kernel_size_list[1],                   
                        conv_in_size_list[1],                   
                        conv_out_size_list[1],                     
                        conv_stride_list[1],                        
                        conv_padding_list[1],                     
                        1,                                         
                        feature_1,                                  
                        conv_out_size_list[1]*  conv_out_size_list[1]*64*(output_num_list[1]/32),
                        WEIGHT_OFFSET+weight_offset_list[1],
                        BIAS_OFFSET+bias_offset_list[1],
                        DATA_MEM_2_OFFSET,  
                        DATA_MEM_1_OFFSET,  
                        DATA_MEM_1_ADDR
                        );
//====================================================================================================layer2 pooling
   pooling_layer_construct( 
                            conv_out_size_list[1],    
                            output_num_list[1],       
                            pooling_kernel_size,      
                            pooling_out_size[1],        
                            pooling_stride, 
                            pooling_padding,        
                            1,            
                            feature_2,          
                            pooling_out_size[1]* pooling_out_size[1]*(output_num_list[1]/32)*64, 
                            DATA_MEM_1_OFFSET, 
                            DATA_MEM_2_OFFSET,
                            DATA_MEM_2_ADDR
                            ); 
    for(i = 0 ; i < pooling_out_size[1]*pooling_out_size[1]*4 ; i++)
        for(j = 0 ; j < 32; j++)
            feature_bank_for_layer4[i][j] = feature_2[i][j];
//====================================================================================================layer3 conv
    write_data(feature_bank_for_layer3,conv_in_size_list[2]*conv_in_size_list[2]*64*ceil((double)input_num_list[2]/32),DATA_MEM_2_ADDR);
    conv_layer_construct(
                        input_num_list[2],                         
                        output_num_list[2],                       
                        conv_kernel_size_list[2],                  
                        conv_in_size_list[2],                   
                        conv_out_size_list[2],                     
                        conv_stride_list[2],                       
                        conv_padding_list[2],                       
                        1,                                           
                        feature_1,                            
                        conv_out_size_list[2]* conv_out_size_list[2]*64*(output_num_list[2]/32), 
                        WEIGHT_OFFSET+ weight_offset_list[2],
                        BIAS_OFFSET+ bias_offset_list[2],
                        DATA_MEM_2_OFFSET,  
                        DATA_MEM_1_OFFSET,  
                        DATA_MEM_1_ADDR
                        );
//====================================================================================================layer3 pooling
    pooling_layer_construct( 
                            conv_out_size_list[2], 
                            output_num_list[2],      
                            pooling_kernel_size,  
                            pooling_out_size[2],  
                            pooling_stride, 
                            pooling_padding,             
                            1,              
                            feature_2,    
                            pooling_out_size[2]*pooling_out_size[2]*(output_num_list[2]/32)*64,  
                            DATA_MEM_1_OFFSET, 
                            DATA_MEM_2_OFFSET,
                            DATA_MEM_2_ADDR
                            ); 
    for(i = 0 ; i < pooling_out_size[2]*pooling_out_size[2]*4 ; i++)
        for(j = 0 ; j < 32; j++)
            feature_bank_for_layer4[i+pooling_out_size[2]*pooling_out_size[2]*4][j] = feature_2[i][j];
//=====================================================================================================layer4 conv   256--->384
    write_data(feature_bank_for_layer4,conv_in_size_list[3]*conv_in_size_list[3]*64*(input_num_list[3]/32),DATA_MEM_2_ADDR);
    conv_layer_construct(
                        input_num_list[3],                         
                        output_num_list[3],                         
                        conv_kernel_size_list[3],                
                        conv_in_size_list[3],                   
                        conv_out_size_list[3],                    
                        conv_stride_list[3],                   
                        conv_padding_list[3],                     
                        1,                                       
                        feature_1,                                 
                        conv_out_size_list[3]*conv_out_size_list[3] *64*(output_num_list[3]/32),    
                        WEIGHT_OFFSET+weight_offset_list[3],
                        BIAS_OFFSET+bias_offset_list[3],
                        DATA_MEM_2_OFFSET, 
                        DATA_MEM_1_OFFSET,  
                        DATA_MEM_1_ADDR
                        );
    for(i = 0 ; i < conv_out_size_list[3]*conv_out_size_list[3]*6; i++)
        for(j = 0 ; j < 32; j++)
            feature_bank_for_layer6[i][j] = feature_1[i+conv_out_size_list[3]*conv_out_size_list[3]*6][j];
    //==================================================================================layer5 conv  192--->192
    conv_layer_construct(
                        input_num_list[4],                        
                        output_num_list[4],                       
                        conv_kernel_size_list[4],                  
                        conv_in_size_list[4],                 
                        conv_out_size_list[4],                  
                        conv_stride_list[4],                      
                        conv_padding_list[4],                      
                        1,                                         
                        feature_2,                              
                        conv_out_size_list[4]*conv_out_size_list[4]*64*(output_num_list[4]/32),   
                        WEIGHT_OFFSET+weight_offset_list[4],
                        BIAS_OFFSET+ bias_offset_list[4],
                        DATA_MEM_1_OFFSET, 
                        DATA_MEM_2_OFFSET,  
                        DATA_MEM_2_ADDR
                        );
    for(i = 0 ; i < conv_out_size_list[4]*conv_out_size_list[4]*6; i++)
        for(j = 0 ; j < 32; j++)
            feature_bank_for_layer7[i][j] = feature_2[i][j];
    //=========================================================================layer6 conv 192--->192
    write_data(feature_bank_for_layer6,conv_in_size_list[5]*conv_in_size_list[5]*64*input_num_list[5]/32,DATA_MEM_1_ADDR);
    conv_layer_construct(
                        input_num_list[5],                         
                        output_num_list[5],                      
                        conv_kernel_size_list[5],                  
                        conv_in_size_list[5],                    
                        conv_out_size_list[5],                     
                        conv_stride_list[5],                        
                        conv_padding_list[5],                      
                        1,                                         
                        feature_2,                             
                        conv_out_size_list[5]*conv_out_size_list[5]*64*(output_num_list[5]/32),   
                        WEIGHT_OFFSET+weight_offset_list[5],
                        BIAS_OFFSET+ bias_offset_list[5],
                        DATA_MEM_1_OFFSET,  
                        DATA_MEM_2_OFFSET,  
                        DATA_MEM_2_ADDR
                        );
    for(i = 0 ; i < conv_out_size_list[5]* conv_out_size_list[5]*6; i++)
        for(j = 0 ; j < 32; j++)
            feature_bank_for_layer8[i][j] = feature_2[i][j];
//=========================================================================================layer7 conv   192--->128
    write_data(feature_bank_for_layer7,conv_in_size_list[6]*conv_in_size_list[6]*64*input_num_list[6]/32,DATA_MEM_1_ADDR);  
    conv_layer_construct(
                        input_num_list[6],                        
                        output_num_list[6],                        
                        conv_kernel_size_list[6],                  
                        conv_in_size_list[6],                   
                        conv_out_size_list[6],                    
                        conv_stride_list[6],                      
                        conv_padding_list[6],                    
                        1,                                  
                        feature_2,                               
                        conv_out_size_list[6]*conv_out_size_list[6]*64*(output_num_list[6]/32),    
                        WEIGHT_OFFSET+weight_offset_list[6],
                        BIAS_OFFSET+ bias_offset_list[6],
                        DATA_MEM_1_OFFSET,  
                        DATA_MEM_2_OFFSET,  
                        DATA_MEM_2_ADDR
                        );
//=======================================================================================layer7 pooling
    pooling_layer_construct( 
                            conv_out_size_list[6],    
                            output_num_list[6],           
                            pooling_kernel_size,          
                            pooling_out_size[6],           
                            pooling_stride,   
                            pooling_padding,            
                            1,              
                            feature_1,        
                            pooling_out_size[6]*pooling_out_size[6]*(output_num_list[6]/32)*64,  
                            DATA_MEM_2_OFFSET, 
                            DATA_MEM_1_OFFSET,
                            DATA_MEM_1_ADDR
                            ); 

    for(i = 0 ; i < output_num_list[6]; i++)
        for(j = 0 ; j < pooling_out_size[6]*pooling_out_size[6]; j++)
            feature_bank_for_fc1[i*pooling_out_size[6]*pooling_out_size[6]+j] = feature_1[(i/32)*pooling_out_size[6]*pooling_out_size[6]+j][i%32]/64.0;
//=======================================================================================layer8 conv   192--->128
    write_data(feature_bank_for_layer8,conv_in_size_list[7]*conv_in_size_list[7]*64*input_num_list[7]/32,DATA_MEM_1_ADDR);
    conv_layer_construct(
                        input_num_list[7],                        
                        output_num_list[7],                     
                        conv_kernel_size_list[7],                  
                        conv_in_size_list[7],                    
                        conv_out_size_list[7],                   
                        conv_stride_list[7],             
                        conv_padding_list[7],              
                        1,                                     
                        feature_2,                          
                        conv_out_size_list[7]*conv_out_size_list[7]*64*(output_num_list[7]/32),   
                        WEIGHT_OFFSET+weight_offset_list[7],
                        BIAS_OFFSET+bias_offset_list[7],
                        DATA_MEM_1_OFFSET,  
                        DATA_MEM_2_OFFSET,  
                        DATA_MEM_2_ADDR
                        );
//=======================================================================================layer8 pooling
    pooling_layer_construct( 
                            conv_out_size_list[7],                 
                            output_num_list[7],              
                            pooling_kernel_size,                 
                            pooling_out_size[7],             
                            pooling_stride,     
                            pooling_padding,                
                            1,                
                            feature_1,         
                            pooling_out_size[7]*pooling_out_size[7]*(output_num_list[7]/32)*64,
                            DATA_MEM_2_OFFSET, 
                            DATA_MEM_1_OFFSET,
                            DATA_MEM_1_ADDR
                            ); 
    for(i = 0 ; i < output_num_list[7]; i++)
        for(j = 0 ; j < pooling_out_size[7]*pooling_out_size[7]; j++)
            feature_bank_for_fc1[output_num_list[7]*pooling_out_size[7]*pooling_out_size[7]+i*pooling_out_size[7]*pooling_out_size[7]+j] = feature_1[(i/32)*pooling_out_size[7]*pooling_out_size[7]+j][i%32]/64.0;
//===============================================================================fc
    fc_software(feature_bank_for_fc1,feature_fc_1,weight_fc5,bias_fc5,input_num_list_fc[0],output_num_list_fc[0]);
    fc_software(feature_fc_1,feature_fc_2,weight_fc6,bias_fc6,input_num_list_fc[1],output_num_list_fc[1]);
    fc_software(feature_fc_2,feature_fc_1,weight_fc7,bias_fc7,input_num_list_fc[2],output_num_list_fc[2]);
    for(i = 0 ; i < 1000; i++)
        cout <<setw(4) << i <<":" << feature_fc_1[i] << endl;
    cout <<"test finish" << endl;
    return 0;
}


void fc_software(float* feature_in,float* feature_out,float* weight,float* bias,int num_input,int num_output)
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

void load(const LayerParameter& src,int num_input,int num_output,int kernel_size,short int weight[][32],int* bias,int weight_offset,int bias_offset) 
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
}