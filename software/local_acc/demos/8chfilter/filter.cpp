#include "../../api_lib/inc/layer.h"
#include "../../api_lib/inc/acc_ctrl.h"
#include "acc_config.h"
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

const uint32_t 	feature_in_size = 320;
const uint32_t 	feature_out_size = 320;
const uint8_t 	input_num = 8;
const uint32_t 	output_num = 32;
const uint32_t 	kernel_size = 3;
short int   feature_out[feature_out_size * feature_out_size][32]={0};
short int   feature_in[feature_in_size * feature_in_size][32] = {0};
short int 	weight[kernel_size * kernel_size * output_num][32] = {0};


void load_weight();


int main()
{
	int i,j;
	uint32_t 	stride = 1;
	uint32_t 	padding = 1;
	uint32_t 	act = 1;
	int      	weight_length = 32 *output_num * kernel_size * kernel_size* 2;
	int         feature_in_length = feature_in_size * feature_in_size * 32 * 2;
	int         feature_out_length = feature_out_size * feature_out_size * 32 * 2;

	Mat frame;
	Mat smallImage;
	Mat inputFeature[input_num];
	Mat splitImage[3];
	Mat outputFeature[output_num];
	Mat outputDispFeature[output_num];
	int dispSize = 160;
	Mat outmergeImage;
	int outmergeImageRows = dispSize * 4 + 10 * 3;
	int outmergeImageCols = dispSize * 8 + 10 * 7;

    cout << "Warning: please make sure your camera is connected and opencv could access the camera!!!" << endl;

	outmergeImage.create(outmergeImageRows,outmergeImageCols,outputFeature[0].type());
	VideoCapture capture(0);
	load_weight();
	write_weight(weight,weight_length,WEIGHT_MEM);
	if(capture.isOpened())
	{
		cout << "success" << endl;
		capture >> frame;
		resize(frame,smallImage,Size(320,320));
		imshow("smallImage(FPGA)",smallImage);
		split(smallImage,splitImage);

		for(i = 0 ; i < 8; i++)
			inputFeature[i] = splitImage[i%3];

		for(i = 0 ; i < output_num ;i++)
			outputFeature[i].create(inputFeature[0].size(),inputFeature[0].type());		
	}
		
	while(capture.isOpened())
	{
		capture >> frame;
		resize(frame,smallImage,Size(320,320));
		imshow("smallImage(FPGA)",smallImage);
		split(smallImage,splitImage);
		for(i = 0 ; i < 8; i++)
			inputFeature[i] = splitImage[i%3];

		for(i = 0 ; i < feature_in_size*feature_in_size ; i++)
			for(j = 0 ; j < input_num;j++)
				feature_in[i][j] = (short int)((inputFeature[j].at<uchar>(i/320,i%320)) << 6);

        write_data(feature_in,feature_in_length,DATA_IN_MEM);

  		conv_layer_construct(input_num,                      //input_num
                            output_num,                       //output_num
                            kernel_size,                   //kernel_size
                            feature_in_size,                    //feature_in_size
                            feature_out_size,                      //feature_out_size
                            stride,                        //stride
                            padding,                                  //padding
                            act,                                  //act
                            feature_out,                        //feature_out
                            feature_out_length,          //feature_out_length
                            WEIGHT_OFFSET,
                            BIAS_OFFSET,
                            DATA_IN_OFFSET,
                            DATA_OUT_OFFSET,
                            DATA_OUT_MEM
                            );
  		cout <<"TEST"<< endl;
		for(i = 0 ; i < 320;i++)
			for(int j = 0 ; j < 320 ; j++)
				for(int k = 0 ; k < output_num;k++)
					outputFeature[k].at<uchar>(i,j) = (unsigned char)(feature_out[i*320+j][k]>>6);		
			
		for(i = 0 ; i < output_num; i++)
			resize(outputFeature[i],outputDispFeature[i],Size(dispSize,dispSize));

		for(i = 0 ; i < 32 ; i++)
		 	outputDispFeature[i].copyTo(outmergeImage(Rect((i%8)*(dispSize+10),(i/8)*(dispSize+10),dispSize,dispSize)));

		imshow("outputFeature(FPGA)",outmergeImage);	
		char key = static_cast<char>(cvWaitKey(1));
			if(key == 27)
				break;
	}
	return 0;
}


void load_weight()
{
	int i,j;
	for(i = 0 ; i < input_num; i++)     //kernel for output0    sobel--x
	{
		weight[0][i] =  1  << 6;weight[1][i] =  2  << 6;weight[2][i] =  1  << 6;
		weight[3][i] =  0  << 6;weight[4][i] =  0  << 6;weight[5][i] =  0  << 6;
		weight[6][i] = -1  << 6;weight[7][i] = -2  << 6;weight[8][i] = -1  << 6;
	}
	for(i = 0 ; i < input_num; i++)     //kernel for output1     sobel--y
	{

		weight[9][i]  = -1  << 6;weight[10][i] =  0  << 6;weight[11][i] =  1  << 6;
		weight[12][i] = -2  << 6;weight[13][i] =  0  << 6;weight[14][i] =  2  << 6;
		weight[15][i] = -1  << 6;weight[16][i] =  0  << 6;weight[17][i] =  1  << 6;
	}
	for(i = 0 ; i < input_num; i++)     //kernel for output2     roberts--x
	{
		weight[18][i] = -1  << 6;weight[19][i] = -2  << 6;weight[20][i] = -1  << 6;
		weight[21][i] =  0  << 6;weight[22][i] =  0  << 6;weight[23][i] =  0  << 6;
		weight[24][i] =  1  << 6;weight[25][i] =  2  << 6;weight[26][i] =  1  << 6;
	}
	for(i = 0 ; i < input_num; i++)      //kernel for output3    roberts--y
	{

		weight[27][i] = -1  << 6;weight[28][i] =  0  << 6;weight[29][i] =  1  << 6;
		weight[30][i] = -2  << 6;weight[31][i] =  0  << 6;weight[32][i] =  2  << 6;
		weight[33][i] = -1  << 6;weight[34][i] =  0  << 6;weight[35][i] =  1  << 6;
	}
	for(i = 0 ; i < input_num;i++)     //kernel for output4      laplacian --1
	{
		weight[36][i] =  0  << 6;weight[37][i] = -1  << 6;weight[38][i] =  0  << 6;
		weight[39][i] = -1  << 6;weight[40][i] =  4  << 6;weight[41][i] = -1  << 6;
		weight[42][i] =  0  << 6;weight[43][i] = -1  << 6;weight[44][i] =  0  << 6;
	}
	for(i = 0 ; i < input_num ;i++)     //kernel for output5      laplacian --2
	{
		weight[45][i] = -1  << 6;weight[46][i] = -1  << 6;weight[47][i] = -1  << 6;
		weight[48][i] = -1  << 6;weight[49][i] =  8  << 6;weight[50][i] = -1  << 6;
		weight[51][i] = -1  << 6;weight[52][i] = -1  << 6;weight[53][i] = -1  << 6;
	}
	for(i = 0 ; i < input_num ;i++)     //kernel for output6    Prewitt --1
	{
		weight[54][i] = -1  << 6;weight[55][i] =  0  << 6;weight[56][i] =  1  << 6;
		weight[57][i] = -1  << 6;weight[58][i] =  0  << 6;weight[59][i] =  1  << 6;
		weight[60][i] = -1  << 6;weight[61][i] =  0  << 6;weight[62][i] =  1  << 6;
	}
	for(i = 0 ; i < input_num ;i++)     //kernel for output7       Prewitt --2
	{
		weight[63][i] =  1  << 6;weight[64][i] =  1  << 6;weight[65][i] =  1  << 6;
		weight[66][i] =  0  << 6;weight[67][i] =  0  << 6;weight[68][i] =  0  << 6;
		weight[69][i] = -1  << 6;weight[70][i] = -1  << 6;weight[71][i] = -1  << 6;
	}
	for(i = 72; i < 32 * kernel_size * kernel_size; i++)
	{
		weight[i][0] =  (rand()%5 - 2)  << 6;
		for(j = 1 ; j < input_num;j++)
			weight[i][j] = weight[i][0];
	}
}
