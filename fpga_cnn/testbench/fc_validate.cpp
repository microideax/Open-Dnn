#include <iostream>
#include <math.h>
#include <stdlib.h>

#include "fc_validate.h"

using namespace std;

fc_validate::fc_validate(int layer_num, int num_input, int num_output, int act) {
    int i, j, k;
    this->layer_num = layer_num;
    this->num_input = num_input;
    this->num_output = num_output;
    this->act = act;

    this->lnum_list[0] = layer_num;

    this->config_list[0] = num_input;
    this->config_list[1] = 0;
    this->config_list[2] = num_output;
    this->config_list[3] = 0;
    this->config_list[4] = 0;
    this->config_list[5] = 0;
    this->config_list[6] = 0;
    this->config_list[7] = 0;
    this->config_list[8] = 0;
    this->config_list[9] = 0;
    this->config_list[10] = 0;
    this->config_list[11] = 0;
    this->config_list[12] = 0;
    this->config_list[13] = 0;
    this->config_list[14] = 0;
    this->config_list[15] = 0;

//    ap_int<512> weight[32];
//    ap_int<512> in_feature[32];
//    ap_int<512> bias[32];
//    ap_int<512> out_feature[32];
//    ap_int<512> out_feature_software[32];

	weight = new ap_int<512>[1024*1024];
	in_feature = new ap_int<512>[1024];
//	bias = new ap_fixed<32,26>[num_output];
	out_feature = new ap_int<512>[1024];
	out_feature_software = new ap_int<512>[1024];


    // initial weight data with random numbers
    for(i = 0; i < (num_input/32)*num_output; i++){
        for(j = 0; j < 32; j++){
            weight[i].range(16*j+15, 16*j) = rand()%10 - 5; // rand()%10 - 5
        }
    }
    for(i = (num_input/32) * num_output; i < ((int)(ceil((float)num_input/32))) * num_output; i++)
	{
		for(j = 0 ; j < num_input % 32; j++)
			weight[i].range(15+16*j,16*j) = rand()%10 - 5; //rand()%10 - 5;
		for(j = num_input % 32 ; j < 32; j++)
			weight[i].range(15+16*j,16*j) = 0;
	}

    // initial input data with random numbers
    for(i = 0 ; i < num_input/32; i++){
	    for(j = 0 ; j < 32; j++){
			in_feature[i].range(15+16*j,16*j) = (rand()%2 -1) * 64; //(rand()%2 -1) * 64
        }
    }
	for(i = num_input/32 ; i < (int)(ceil(float(num_input)/32)); i++){
		for(j = 0 ; j < num_input % 32; j++){
			in_feature[i].range(15+16*j, 16*j) = (rand()%2 - 1) * 64; // (rand()%2 - 1) * 64
        }
		for(j = num_input % 32; j < 32; j++){
			in_feature[i].range(15+16*j, 16*j) = 0;
        }
	}

    // initial bias data with random numbers
    for(i = 0; i < num_output/32; i++){
        for(j = 0; j < 32; j++){
            bias[i].range(15+16*j, 16*j) = 0*64; // (rand()%2) * 64
        }
    }
}

void fc_validate::print_feature_in(void){
	int i,j;
	cout << "fc input feature:" << endl;
	for(i = 0 ; i < (int)(ceil(float(num_input)/32)); i++)
	{
		cout <<i<<":";
		for(j = 0 ; j < 32; j++)
			cout <<(short int)(in_feature[i].range(15+16*j,16*j)) <<" ";
		cout << endl;
	}
}

void fc_validate::print_weight(){
	int i,j,k;
	cout << "fc weight:" << endl;
	for(i = 0 ; i < (int)(ceil((float)num_input/32))*num_output; i++)
	{
		cout << i << ":";
		for(j = 0 ; j < 32; j++)
			cout << (short int)weight[i].range(15+16*j,16*j) <<" ";
		cout << endl;
	}
}
void fc_validate::print_bias(){
	int i, j, k;
	cout << "fc bias:" << endl;
	for(i = 0 ; i < (int)(ceil((float)num_output/32)); i++){
		cout << i << ":";
		for(j = 0; j < 32 && j < num_output; j++)
			cout << (short int)bias[i].range(15+16*j, 16*j) << " ";
		cout << endl;
	}
}
void fc_validate::print_feature_out(){
	int i,j;
	cout <<"fc feature out:"<<endl;
	for(i = 0 ; i < (int)(ceil(float(num_output)/32)); i++)
	{
		cout <<i<<":";
		for(j = 0 ; j < 32 && j < num_output; j++)
			cout << (short int)(out_feature[i].range(15+16*j, 16*j)/64) <<" ";
		cout << endl;
	}
}

void fc_validate::software_fc_process(void){
    int i, j, r, c;
    int temp;
    short int* temp_array_i = new short int[num_input];
    short int* temp_array_w = new short int[num_input];

    // transfer in_data and weight to regular data array
    for(i = 0; i < num_input/32; i++){
        for(j = 0; j < 32; j++){
            temp_array_i[32*i + j] = in_feature[i].range(15+16*j, 16*j);
            temp_array_w[32*i + j] = weight[i].range(15+16*j, 16*j);
        }
    }
    
    for(i = 0; i < num_output; i++){
        temp = (bias[i/32].range(15+16*(i%32), 16*(i%32)));
        for(j = 0; j < num_input; j++){
            temp += temp_array_i[j] * temp_array_w[j];
        }
        out_feature_software[(i/32)].range(15+16*(i%32), 16*(i%32)) = temp/64; // (temp < 0) ? 0 : temp
    }
}

void fc_validate::print_software_out(){
	int i, j;
	cout << "fc software feature out:" << endl;
	for(i=0; i< (int)(ceil(float(num_output)/32)); i++){
		cout << i << ":";
		for (j = 0; j < 32 && j < num_output; j++){
			cout << (short int)(out_feature_software[i].range(15+16*j, 16*j)/64) << " ";
		}
		cout << endl;
	}
}
