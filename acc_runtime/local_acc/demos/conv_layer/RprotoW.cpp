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
#include "acc_config.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#define CNN_OPEN_BINARY(filename) open(filename, O_RDONLY)
#define CNN_OPEN_TXT(filename) open(filename, O_RDONLY)

using namespace caffe;
using namespace std;
using namespace cv;
using google::protobuf::io::FileInputStream;
using google::protobuf::io::FileOutputStream;
using google::protobuf::io::ZeroCopyInputStream;
using google::protobuf::io::CodedInputStream;
using google::protobuf::io::ZeroCopyOutputStream;
using google::protobuf::io::CodedOutputStream;
using google::protobuf::Message;

void create_net_from_caffe_prototxt(const std::string& caffeprototxt,int input_param[]);
void reload_weight_from_caffe_protobinary(const std::string& caffebinary,int input_param[],short int weight[][32],int* bias);

const int feature_in_size = 227;
short int feature_in[feature_in_size*feature_in_size][32] = {0};

int main(int argn, char* argv[])
{

    int i,j;
    string caffemodel = argv[1];
    string prototxt   = argv[2];

    const int num_input = 3;
    const int num_ouput = 96;
    const int conv_kernel_size = 11;
    const int pooling_kernel_size = 3;
    const int conv_stride = 4;
    const int pooling_stride = 2;
    const int conv_out_size = 55;
    const int pooling_out_size = 27;
    short int feature_out[num_ouput / 3 * conv_out_size*conv_out_size][32]={0};
    short int pooling_out[3*pooling_out_size*pooling_out_size][32] = {0};
    short int weight[num_ouput * conv_kernel_size * conv_kernel_size][32] = {0};
    int       bias[num_ouput] = {0};

    
    cout << "Read prototxt file from: " << prototxt << endl;
    cout << "Read caffemodel file from: " << caffemodel << endl;
    int input_param[] = {0, 0};
    create_net_from_caffe_prototxt(argv[2], input_param);
    reload_weight_from_caffe_protobinary(argv[1], input_param,weight,bias);

    Mat frame;
    Mat smallImage;
    Mat splitImage[3];

    Mat inputmergeImage;
    int inputmergeRows = feature_in_size;
    int inputmergeCols = feature_in_size * 3 + 5 * 2;

    Mat outmergeImage_conv;
    Mat outputFeature_conv[num_ouput];
    int outmergeImage_convRows = conv_out_size * 6 + 5 * 5;
    int outmergeImage_convCols = conv_out_size * 16 + 5 * 15;

    Mat outmergeImage_pooling;
    Mat outputFeature_pooling[num_ouput];
    int outmergeImage_poolingRows = pooling_out_size * 6 + 5 * 5;
    int outmergeImage_poolingCols = pooling_out_size * 16 + 5 * 15;
    VideoCapture capture(0);
    write_weight_bias(weight,conv_kernel_size*conv_kernel_size*64*num_ouput,bias,num_ouput*4,WEIGHT_MEM,BIAS_MEM);

    if(capture.isOpened())
    {
        cout << "success"<< endl;
        capture >> frame;
        resize(frame,smallImage,Size(feature_in_size,feature_in_size));
        imshow("camera image",smallImage);
        split(smallImage,splitImage);

        inputmergeImage.create(inputmergeRows,inputmergeCols,splitImage[0].type());

        for(i = 0 ; i < num_ouput; i++)
            outputFeature_conv[i].create(Size(conv_out_size,conv_out_size),splitImage[0].type());
        outmergeImage_conv.create(outmergeImage_convRows,outmergeImage_convCols,outputFeature_conv[0].type());

        for(i = 0 ; i < num_ouput; i++)
            outputFeature_pooling[i].create(Size(pooling_out_size,pooling_out_size),splitImage[0].type());
        outmergeImage_pooling.create(outmergeImage_poolingRows,outmergeImage_poolingCols,outputFeature_pooling[0].type());

        for(i = 0 ; i < feature_in_size*feature_in_size; i++)
            for(j = 0 ; j < num_input;j++)
                feature_in[i][j]=(short int)((splitImage[j].at<uchar>(i/feature_in_size,i%feature_in_size)) << 6);

    }
    while(capture.isOpened())
    {
        capture >> frame;
        resize(frame,smallImage,Size(feature_in_size,feature_in_size));
        imshow("camera image",smallImage);
        split(smallImage,splitImage);
        for(i = 0 ; i < feature_in_size*feature_in_size; i++)
            for(j = 0 ; j < num_input;j++)
                feature_in[i][j]=(short int)((splitImage[j].at<uchar>(i/feature_in_size,i%feature_in_size)) << 6);

        write_data(feature_in,feature_in_size*feature_in_size*64,DATA_IN_MEM);
        conv_layer_construct(num_input,                      //input_num
                            num_ouput,                       //output_num
                            conv_kernel_size,                   //kernel_size
                            feature_in_size,                    //feature_in_size
                            conv_out_size,                      //feature_out_size
                            conv_stride,                        //stride
                            0,                                  //padding
                            1,                                  //act
                            feature_out,                        //feature_out
                            conv_out_size*conv_out_size*64*(num_ouput/32),          //feature_out_length
                            WEIGHT_OFFSET,
                            BIAS_OFFSET,
                            DATA_IN_OFFSET,
                            DATA_OUT_OFFSET,
                            DATA_OUT_MEM
                            );

        pooling_layer_construct(conv_out_size,     //feature_in_size
                                num_ouput,      //input_num
                                pooling_kernel_size,  //kernel_size
                                pooling_out_size,  //feature_out_size
                                pooling_stride,    //stride
                                0,                 //padding
                                1,                 //act
                                pooling_out,       //feature_out
                                pooling_out_size*pooling_out_size*(num_ouput/32)*64,         //feature_out_length
                                DATA_OUT_OFFSET, 
                                POOLING_OUT_OFFSET,
                                POOLING_OUT_MEM
                                );  
        disp_performance(num_input,
                         num_ouput,
                         conv_kernel_size,
                        conv_out_size
                        );

        for(i = 0 ; i < num_input; i++)
            splitImage[i].copyTo(inputmergeImage(Rect(i*(feature_in_size+5),0,feature_in_size,feature_in_size)));

        for(i = 0 ; i < num_ouput; i++)
            for(j = 0 ; j < conv_out_size * conv_out_size; j++)
                outputFeature_conv[i].at<uchar>(j/conv_out_size,j%conv_out_size) = (feature_out[j + conv_out_size*conv_out_size*(i/32)][i%32]/64.0) <= 255 ?(feature_out[j + conv_out_size*conv_out_size*(i/32)][i%32]/64.0) : 255;  

        for(i = 0 ; i < num_ouput ; i++)
            outputFeature_conv[i].copyTo(outmergeImage_conv(Rect((i%16)*(conv_out_size+5),(i/16)*(conv_out_size+5),conv_out_size,conv_out_size)));  

        for(i = 0 ; i < num_ouput; i++)
            for(j = 0 ; j < pooling_out_size * pooling_out_size; j++)
                outputFeature_pooling[i].at<uchar>(j/pooling_out_size,j%pooling_out_size) = (pooling_out[j + pooling_out_size*pooling_out_size*(i/32)][i%32]/64.0) <= 255 ? (pooling_out[j + pooling_out_size*pooling_out_size*(i/32)][i%32]/64.0) : 255;  

        for(i = 0 ; i < num_ouput ; i++)
            outputFeature_pooling[i].copyTo(outmergeImage_pooling(Rect((i%16)*(pooling_out_size+5),(i/16)*(pooling_out_size+5),pooling_out_size,pooling_out_size)));  
       
        imshow("input feature",inputmergeImage);
        imshow("conv output feature",outmergeImage_conv);
        imshow("pooling output feature",outmergeImage_pooling);

        char key = static_cast<char>(cvWaitKey(1));
            if(key == 27)
                break;
    }
    return 0;
}


void load(const LayerParameter& src,int num_input,int num_output,int kernel_size,short int weight[][32],int* bias) 
{
    int src_idx = 0;
    //load weight
    for (int o = 0; o < num_output; o++) 
        for (int i = 0; i < num_input; i++) 
            for (int x = 0; x < kernel_size; x++) 
                for(int y = 0 ; y < kernel_size; y++)
                    weight[o * kernel_size * kernel_size +  x * kernel_size + y][i] = (short int)(src.blobs(0).data(src_idx++) * 64 );
        
    if(src.convolution_param().bias_term()==false)
        ;
    else
        //load bias
        for (int o = 0; o < num_output; o++) 
            bias[o] = src.blobs(1).data(o) * 64;
    
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
    int num_layers = src_net.size();
    cout << "src_net size: " << num_layers << endl;

    int num_input=input_param[0];
    int input_size=input_param[1];
    int num_output=0;
    for (int i = 0; i < src_net.size(); i++) 
    {
        int pad=0;
        int kernel_size=0;
        int stride=1;

        if(src_net[i].type()=="Convolution"||src_net[i].type()=="ConvolutionRistretto")
        {//get conv_layers' kernel_size,num_output
            cout << "index number: " << i << endl;
            cout << "src_net[" << i << "] type: " << src_net[i].type() << endl;
            cout << "src_net[" << i << "] name: " << src_net[i].name() << endl;
            ConvolutionParameter conv_param = src_net[i].convolution_param();
            num_output=conv_param.num_output();
            if (conv_param.pad_size()>0)
                pad=conv_param.pad(0);
            kernel_size=conv_param.kernel_size(0);
            if (conv_param.stride_size()>0)
                stride=conv_param.stride(0);
            input_size = (input_size + 2 * pad - kernel_size) / stride + 1;
            num_input=num_input/conv_param.group();
        }
        if (i == 1) 
        {
            cout << "num input:" << num_input << endl;
            cout << "num output:"<< num_output << endl;
            load(src_net[i], num_input, num_output, kernel_size,weight,bias);
        }
    }
}
void reload_weight_from_caffe_protobinary(const std::string& caffebinary,int input_param[],short int weight[][32],int* bias)
{
    caffe::NetParameter np;
    read_proto_from_binary(caffebinary, &np);
    reload_weight_from_caffe_net(np,input_param,weight,bias);
}