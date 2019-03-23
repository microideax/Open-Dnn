def conv_default_function(idx, parameters):

	creator = "conv_acc<"+",".join(parameters)+"> convAcc" + str(idx) +";"
	func = '''void conv_layer_acc_'''+str(idx)+'''(
		Tparam N,
		Tparam K,
		Tparam M,
		Tparam R_IN,
		Tparam C_IN,
		Tparam C_OUT,
		Tparam R_OUT,
		Tparam S,
		Tparam P,
		Tparam act,
		Tparam inport,
		Tparam weight_offset,
		Tparam bias_offset,
		Tparam in_offset,
		Tparam out_offset,
		ap_fixed<32,26>* layer_bias,
		data_type_itf* i_weight,
		data_type_itf* i_data,
		data_type_itf* out_data
	){
	    convAcc'''+str(idx)+'''.conv_layer_acc_mbuf(N, K, M, R_IN, C_IN, C_OUT, R_OUT, S, P, act, inport, weight_offset, bias_offset, in_offset, out_offset,layer_bias,i_weight,i_data,out_data);
	};'''
	return creator, func

def max_pool_default_function(idx, parameters):
	creator = "max_pool_acc<"+",".join(parameters)+"> maxPoolAcc"+str(idx)+";"
	func = '''void max_pool_layer_acc_'''+str(idx)+'''(
		Tparam R_in,
		Tparam C_in,
		Tparam N,
		Tparam K,
		Tparam R,
		Tparam C,
		Tparam S,
		Tparam P,
		Tparam act,
		Tparam i_offset,
		Tparam o_offset,
		data_type_itf* i_data,
		data_type_itf* o_data){
		    maxPoolAcc'''+str(idx)+'''.max_pool_layer_mbuf(R_in, C_in, N, K, R, C, S, P, act, i_offset, o_offset, i_data, o_data);
		};'''
	return creator, func

def fc_default_function(idx, parameters):
	creator = "fc_acc<"+",".join(parameters)+"> fcAcc"+str(idx)+";"

	func = '''void fc_layer_acc_'''+str(idx)+'''(
		Tparam N,
		Tparam K,
		Tparam M,
		Tparam R_IN,
		Tparam C_IN,
		Tparam C_OUT,
		Tparam R_OUT,
		Tparam S,
		Tparam P,
		Tparam act,
		Tparam weight_offset,
		Tparam bias_offset,
		Tparam in_offset,
		Tparam out_offset,
		data_type_itf* layer_bias,
		data_type_itf* i_weight,
		data_type_itf* i_data,
		data_type_itf* out_data
		){
			fcAcc'''+str(idx)+'''.fc_layer_acc_mbuf(N, M, R_IN, C_IN, K, act,
					i_weight, layer_bias,
					weight_offset, bias_offset, in_offset, out_offset,
					i_data, out_data);
		};'''
	return creator, func


def conv_pool_default_func(parameters):
	if len(parameters) == 3:
		idx1, idx2, idx3 = parameters
		idx1 = idx1.strip()
		idx2 = idx2.strip()
		idx3 = idx3.strip()
		print("conv pool generation parameter list len(): ", len(parameters))
		creator, func = conv_pool(idx1, idx2, idx3)
	elif len(parameters) == 2:
		idx1, idx2 = parameters
		idx1 = idx1.strip()
		idx2 = idx2.strip()
		print("conv pool generation parameter list len(): ", len(parameters))
		creator, func = single_conv(idx1, idx2)

	return creator, func


def conv_pool(idx1, idx2, idx3):
	func ='''void conv_pool_acc_'''+str(idx1)+'''(
	Tparam* param_port,
	ap_fixed<32,26>* bias_in,
	data_type_itf* weight_in,
	data_type_itf* data_in,
	data_type_itf* data_out
	   )
	{
	    Tparam layer_num_local[16];
	    Tparam param_conv_local[16];
	    Tparam param_pool_local[16];

	    for (unsigned int ll = 0; ll < 16; ll++)
	    {
	    	layer_num_local[ll] = param_port[ll];
	    }

	    cout << "LAYER ACC: CONV Loading layer number for current accelerator ..." << endl;
	    cout << "LAYRE ACC: CONV will process "<< layer_num_local[0] << " layers" <<endl;
	    for (unsigned int l = 0; l < layer_num_local[0]; l++) {
	    	cout << "LAYER ACC: CONV Processing " << l << "th layer ..." << endl;
	        for (unsigned int i = 0; i < 16; i++)
	        {
	            param_conv_local[i] = param_port[16 + l*16 + i];
	            param_pool_local[i] = param_port[16 + 256 + l*16 + i];
	        }
	        if (param_pool_local[15] == 0)
	        {
	            cout << "LAYER ACC: Execute conv layer without pool, enable_pool =  " << param_pool_local[15] << endl;
	            conv_layer_acc_'''+str(idx2)+'''(param_conv_local[0], // N
	                         param_conv_local[1], // K
	                         param_conv_local[2], // M
	                         param_conv_local[3], // Rin
	                         param_conv_local[4], // C
	                         param_conv_local[5], // R
	                         param_conv_local[6], // C
	                         param_conv_local[7], // S
	                         param_conv_local[8], // P
	                         param_conv_local[9], // act
	                         param_conv_local[14], //inport
	                         param_conv_local[10], // w_offset
	                         param_conv_local[11], // b_offset
	                         param_conv_local[12], // in_offset
	                         param_conv_local[13], // out_offset
	                         bias_in,
	                         weight_in,
	                         data_in,
	                         data_out);
	        }
	        else
	        {
	            cout << "LAYER ACC: POOL Execute pool layer with pooling enable: " << param_pool_local[15] << endl;
	            max_pool_layer_acc_'''+str(idx3)+'''(param_pool_local[0], // R_in,
	                           param_pool_local[1], // C_in
	                           param_pool_local[2], // N
	                           param_pool_local[3], // K
	                           param_pool_local[4], // R
	                           param_pool_local[5], // C
	                           param_pool_local[6], // S
	                           param_pool_local[7], // P
	                           param_pool_local[8], // act
	                           param_pool_local[9], // in_offset
	                           param_pool_local[10], // out_offset
	                           data_out,
	                           data_out);
	        }
	    }
	};'''

	return "", func


def single_conv(idx1, idx2):
	func ='''void conv_pool_acc_'''+str(idx1)+'''(
	Tparam* param_port,
	ap_fixed<32,26>* bias_in,
	data_type_itf* weight_in,
	data_type_itf* data_in,
	data_type_itf* data_out
	   )
	{
	    Tparam layer_num_local[16];
	    Tparam param_conv_local[16];
	    Tparam param_pool_local[16];

	    for (unsigned int ll = 0; ll < 16; ll++)
	    {
	    	layer_num_local[ll] = param_port[ll];
	    }

	    cout << "LAYER ACC: CONV Loading layer number for current accelerator ..." << endl;
	    cout << "LAYRE ACC: CONV will process "<< layer_num_local[0] << " layers" <<endl;
	    for (unsigned int l = 0; l < layer_num_local[0]; l++) {
	    	cout << "LAYER ACC: CONV Processing " << l << "th layer ..." << endl;
	        for (unsigned int i = 0; i < 16; i++)
	        {
	            param_conv_local[i] = param_port[16 + l*16 + i];
	            param_pool_local[i] = param_port[16 + 256 + l*16 + i];
	        }
	        if (param_pool_local[15] == 0)
	        {
	            cout << "LAYER ACC: Execute conv layer without pool, enable_pool =  " << param_pool_local[15] << endl;
	            conv_layer_acc_'''+str(idx2)+'''(param_conv_local[0], // N
	                         param_conv_local[1], // K
	                         param_conv_local[2], // M
	                         param_conv_local[3], // Rin
	                         param_conv_local[4], // C
	                         param_conv_local[5], // R
	                         param_conv_local[6], // C
	                         param_conv_local[7], // S
	                         param_conv_local[8], // P
	                         param_conv_local[9], // act
	                         param_conv_local[14], //inport
	                         param_conv_local[10], // w_offset
	                         param_conv_local[11], // b_offset
	                         param_conv_local[12], // in_offset
	                         param_conv_local[13], // out_offset
	                         bias_in,
	                         weight_in,
	                         data_in,
	                         data_out);
	        }
	        }
	        }'''

	return "", func


def conv_default_func(parameters):
	idx1,idx2, idx3 = parameters
	idx1 = idx1.strip()
	idx2 = idx2.strip()
	idx3 = idx3.strip()

	func ='''void conv_pool_acc_'''+str(idx1)+'''(
	Tparam* param_port,
	ap_fixed<32,26>* bias_in,
	data_type_itf* weight_in,
	data_type_itf* data_in,
	data_type_itf* data_out
	   )
	{
	    Tparam layer_num_local[16];
	    Tparam param_conv_local[16];
	    Tparam param_pool_local[16];

	    for (unsigned int ll = 0; ll < 16; ll++)
	    {
	    	layer_num_local[ll] = param_port[ll];
	    }

	    cout << "LAYER ACC: CONV Loading layer number for current accelerator ..." << endl;
	    cout << "LAYRE ACC: CONV will process "<< layer_num_local[0] << " layers" <<endl;
	    for (unsigned int l = 0; l < layer_num_local[0]; l++) {
	    	cout << "LAYER ACC: CONV Processing " << l << "th layer ..." << endl;
	        for (unsigned int i = 0; i < 16; i++)
	        {
	            param_conv_local[i] = param_port[16 + l*16 + i];
	            param_pool_local[i] = param_port[16 + 256 + l*16 + i];
	        }
	        if (param_pool_local[15] == 0)
	        {
	            cout << "LAYER ACC: Execute conv layer without pool, enable_pool =  " << param_pool_local[15] << endl;
	            conv_layer_acc_'''+str(idx2)+'''(param_conv_local[0], // N
	                         param_conv_local[1], // K
	                         param_conv_local[2], // M
	                         param_conv_local[3], // Rin
	                         param_conv_local[4], // C
	                         param_conv_local[5], // R
	                         param_conv_local[6], // C
	                         param_conv_local[7], // S
	                         param_conv_local[8], // P
	                         param_conv_local[9], // act
	                         param_conv_local[14], //inport
	                         param_conv_local[10], // w_offset
	                         param_conv_local[11], // b_offset
	                         param_conv_local[12], // in_offset
	                         param_conv_local[13], // out_offset
	                         bias_in,
	                         weight_in,
	                         data_in,
	                         data_out);
	        }
	        else
	        {
	            cout << "LAYER ACC: POOL Execute pool layer with pooling enable: " << param_pool_local[15] << endl;
	            max_pool_layer_acc_'''+str(idx3)+'''(param_pool_local[0], // R_in,
	                           param_pool_local[1], // C_in
	                           param_pool_local[2], // N
	                           param_pool_local[3], // K
	                           param_pool_local[4], // R
	                           param_pool_local[5], // C
	                           param_pool_local[6], // S
	                           param_pool_local[7], // P
	                           param_pool_local[8], // act
	                           param_pool_local[9], // in_offset
	                           param_pool_local[10], // out_offset
	                           data_out,
	                           data_out);
	        }
	    }
	};'''

	return "", func


def load_parameter(filename):
	lists = []
	with open(filename) as f:
		while 1:
			line = f.readline()
			if not line:
				break
			lists.append(line.strip().split(","))

	ps_list = {}
	print("loaded parameters")
	for l in lists:
		if l[0] not in ps_list:
			ps_list[l[0]] = []
		ps_list[l[0]].append(l[1:])

	return ps_list

def switch_function(idx, flag, parameters):
	if flag =="conv":
		return conv_default_function(idx, parameters)
	elif flag =="max_pool":
		return max_pool_default_function(idx, parameters)
	elif flag =="fc":
		return fc_default_function(idx, parameters)
	elif flag =="conv_pool":
		return conv_pool_default_func(parameters)
	else:
		print("Error with function generation flag")

def generate_file(ps_file, store_file):
	ps = load_parameter(ps_file)
	keys = ["conv", "max_pool", "fc", "conv_pool"]
	with open(store_file, "w") as wf:
		strs = '''#ifndef _ACC_INSTANCE_H_
#define _ACC_INSTANCE_H_
#include "config.h"
#include "conv_acc_2ibuf.h"
#include "fc_acc_innerpp.h"
#include "max_pool_acc_innerpp.h"\n
using namespace std;'''
		wf.write(strs + "\n")

		for key in keys:
			lists = ps[key]
			for i in range(len(lists)):
				head, func = switch_function(i, key, lists[i])
				wf.write(head + "\n\n" + func + "\n\n")
		wf.write("#endif\n")
	print("ok")

if __name__ == "__main__":
	generate_file("acc_ins_param.txt", "acc_instance.h")







