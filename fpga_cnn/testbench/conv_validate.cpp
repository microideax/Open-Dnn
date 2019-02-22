#include <iostream>
#include <math.h>
#include <stdlib.h>
#include "conv_validate.h"
using namespace std;


conv_validate::conv_validate(int layer_num,int num_input,int num_output,int kernel_size,int stride,int padding, int inputfeature_size, int inport)
{
	int i,j,k,x,y,z;
	this->layer_num = layer_num;
	this->num_input = num_input;
	this->num_output = num_output;
	this->stride = stride;
	this->padding = padding;
	this->inputfeature_size = inputfeature_size;
	this->kernel_size = kernel_size;
	outputfeature_size = (inputfeature_size + 2*padding - kernel_size)/stride + 1;
	act = 1;

//	for(int i =0; i < 16; i++){
//		this->lnum_list[0] = 0;
//	}
	for(i = 0; i < 528; i++){
		this->param_list[i] = 0;
	}

	this->lnum_list[0] = layer_num;

	this->config_list[0] = num_input;
	this->config_list[1] = kernel_size;
	this->config_list[2] = num_output;
	this->config_list[3] = inputfeature_size;
	this->config_list[4] = inputfeature_size;
	this->config_list[5] = outputfeature_size;
	this->config_list[6] = outputfeature_size;
	this->config_list[7] = stride;
	this->config_list[8] = padding;
	this->config_list[9] = act;
	this->config_list[10] = 0;
	this->config_list[11] = 0;
	this->config_list[12] = 0;
	this->config_list[13] = 0;
	this->config_list[14] = inport;
	this->config_list[15] = 0;

	// initialize param_list
	for(i = 0; i < 16; i++)
	{
		this->param_list[i] = this->lnum_list[i];
	}
	for(i = 16; i < 32; i++)
	{
		this->param_list[i] = this->config_list[i - 16];
	}
	weight = new ap_int<512>[(int)(ceil((double)num_output/32))*num_input*kernel_size*kernel_size];
	input_feature = new ap_int<512>[inputfeature_size * inputfeature_size * (int)(ceil(((double)num_input)/32))];
	bias = new ap_fixed<32,26>[num_output];
//	output_feature = new ap_int<512>[outputfeature_size * outputfeature_size * (int)(ceil(((double)num_output)/32))];
	output_feature = new ap_int<512>[2048];
	output_feature_software = new ap_int<512>[outputfeature_size * outputfeature_size * (int)(ceil(((double)num_output)/32))];

	for(i = 0 ; i < (num_output/32) * num_input * kernel_size * kernel_size; i++)
		for(j = 0 ; j < 32; j++)
			weight[i].range(15+16*j,16*j) = rand()%10; //rand()%2 * 64;
	for(i = (num_output/32) * num_input * kernel_size * kernel_size; i < ((int)(ceil((double)num_output/32))) * num_input * kernel_size * kernel_size;i++)
	{
		for(j = 0 ; j < num_output % 32; j++)
			weight[i].range(15+16*j,16*j) = rand()%10; //rand()%2 * 64;
		for(j = num_output % 32 ; j < 32; j++)
			weight[i].range(15+16*j,16*j) = 0;
	}


	for(i = 0 ; i < num_input/32 ;i++)
		for(j = 0 ; j < inputfeature_size; j++)
			for(k = 0 ; k <inputfeature_size; k++)
				for(x = 0 ; x < 32; x++)
					input_feature[i*inputfeature_size*inputfeature_size + j*inputfeature_size + k].range(15+16*x,16*x) = (rand()%3 -1) * 64;

	for(i = num_input/32 ; i < (int)(ceil(double(num_input)/32)); i++)
		for(j = 0 ; j < inputfeature_size; j++)
			for(k = 0 ; k < inputfeature_size; k++)
			{
				for(x = 0 ; x < num_input % 32; x++)
					input_feature[i*inputfeature_size*inputfeature_size + j*inputfeature_size + k].range(15+16*x,16*x) = (rand()%3 - 1) * 64;
				for(x = num_input % 32; x < 32; x++)
					input_feature[i*inputfeature_size*inputfeature_size + j*inputfeature_size + k].range(15+16*x,16*x) = 0;
			}
	for(i = 0 ; i < num_output; i++)
		bias[i].range(15,0) = 10;//rand()%10;
}


void conv_validate :: print_feature_in(void)
{
	int i,j;
	cout << "input feature:" << endl;
	for(i = 0 ; i < inputfeature_size * inputfeature_size * (int)(ceil(double(num_input)/32)); i++)
	{
		cout <<i<<":";
		for(j = 0 ; j < 32; j++)
			cout <<(short int)(input_feature[i].range(15+16*j,16*j)) <<" ";
		cout << endl;
	}
}


void conv_validate :: print_weight(void)
{
	int i,j,k;
	cout << "weight:"<<endl;
	for(i = 0 ; i < (int)(ceil((double)num_input/32))*num_output * kernel_size*kernel_size; i++)
	{
		cout << i << ":";
		for(j = 0 ; j < 32; j++)
			cout << weight[i].range(15+16*j,16*j) <<" ";
		cout << endl;
	}
}

void conv_validate :: print_bias(void)
{
	int i;
	cout <<"bias:"<<endl;
	for(i = 0 ; i < num_output; i++)
		cout << i << ":" << bias[i] <<endl;
}


void conv_validate :: print_feature_out(void)
{
	int i,j;
	cout <<"feature out:"<<endl;
	for(i = 0 ; i < outputfeature_size * outputfeature_size * (int)(ceil(double(num_output)/32)); i++)
	{
		cout <<i<<":";
		for(j = 0 ; j < 32; j++)
			cout <<output_feature[i].range(15+16*j,16*j) <<" ";
		cout << endl;
	}
}

void conv_validate :: software_conv_process(void)       //assume padding = 2
{
	int i,j,k,x,y,z;
	int temp;
	short int* temp_array = new short int[(inputfeature_size+2*padding) * (inputfeature_size+2*padding) * num_input];

	for(k = 0 ; k < num_input; k++)
		for(i = 0 ; i < inputfeature_size+2*padding; i++)
			for(j = 0 ; j < inputfeature_size+2*padding ; j++)
				temp_array[ k * (inputfeature_size+2*padding ) * (inputfeature_size+2*padding ) + i * (inputfeature_size+2*padding ) + j] = 0;

	for(j = 0 ; j < num_input; j++)
		for(x = padding; x < inputfeature_size + padding; x++)
			for(y = padding; y < inputfeature_size + padding; y++)
				temp_array[ j * (inputfeature_size+2*padding ) * (inputfeature_size+2*padding ) + x * (inputfeature_size+2*padding ) + y]
							= input_feature[(j/32) * inputfeature_size * inputfeature_size +(x-padding)*inputfeature_size + (y-padding)].range(16*(j%32)+15,16*(j%32));

	cout <<"software processing..." << endl;
	for(i = 0 ; i < num_output; i++)
	{
//		    cout <<"input1:"<<endl;
//			for(x = 0 ; x < inputfeature_size+2*padding; x++)
//			{
//				cout << x << ":";
//				for(y = 0 ; y < inputfeature_size+2*padding ; y++)
//				{
//					cout <<setw(5)<< temp_array[x* (inputfeature_size+2*padding ) +y] << " ";
//
//				}
//				cout << endl;
//			}
//			cout << "input2:" << endl;
//			for(x = 0 ; x < inputfeature_size+2*padding; x++)
//			{
//				cout << x << ":";
//				for(y = 0 ; y < inputfeature_size+2*padding ; y++)
//				{
//					cout <<setw(5)<< temp_array[(inputfeature_size+2*padding) * (inputfeature_size+2*padding) + x* (inputfeature_size+2*padding) +y] << " ";
//
//				}
//				cout << endl;
//			}
		for(x = 0 ; x < inputfeature_size-kernel_size+1+2*padding; x+=stride)
			for(y = 0; y < inputfeature_size-kernel_size+1+2*padding;y+=stride)
			{
				temp = bias[i].range(15,0);
				for(j = 0 ; j < num_input; j++)
				{


					for(k = 0 ; k < kernel_size;k++)
					{
						for(z = 0 ; z < kernel_size; z++)
						{

							temp += (int)(temp_array[j * (inputfeature_size+2*padding ) * (inputfeature_size+2*padding ) + x * (inputfeature_size+2*padding) + y + (k*(inputfeature_size+2*padding) + z)])
									//*(int)(weight[(j/32)*num_output*kernel_size*kernel_size +i*kernel_size*kernel_size + k*kernel_size + z].range(15+16*(j%32),16*(j%32)))/64;
								   *(int)(weight[(i/32)*num_input*kernel_size*kernel_size +j*kernel_size*kernel_size + k*kernel_size + z].range(15+16*(i%32),16*(i%32)))/64;

//							cout << "temp = " << temp
//							      <<"feature = " << (int)(temp_array[x * (inputfeature_size+2*padding) + y + (k*(inputfeature_size+2*padding) + z)])
//								  <<"weight = "  << (int)(weight[(j/32)*num_output*kernel_size*kernel_size +i*kernel_size*kernel_size + k*kernel_size + z].range(15+16*(j%32),16*(j%32)))
//								  <<endl;

						}
					}
					//cout << j<<":temp = " << temp << endl;
				}

				output_feature_software[(i/32) *outputfeature_size * outputfeature_size + (x/stride) * outputfeature_size + y/stride].range(16*(i%32)+15,16*(i%32)) = (temp < 0) ? 0 : temp;


				//cout << "temp = " << temp << endl;
			}

		//	while(1);

	}

}


void conv_validate :: print_feature_out_softeare(void)
{
	int i,j;
	cout <<"software feature out:"<<endl;
	for(i = 0 ; i < outputfeature_size * outputfeature_size * (int)(ceil(double(num_output)/32)); i++)
	{
		cout <<i<<":";
		for(j = 0 ; j < 32; j++)
			cout <<output_feature_software[i].range(15+16*j,16*j) <<" ";
		cout << endl;
	}
}

void conv_validate :: test_fun(void)
{
	cout << "class test : "<< outputfeature_size << endl;
	cout << (int)(ceil((double)num_input/32)) << endl;
}
