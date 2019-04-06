import argparse

def gen_sub_head():

    strs = '''
#ifndef _CONSTRUCT_NET_H_
#define _CONSTRUCT_NET_H_

#include <iostream>
#include <ap_fixed.h>
#include "acc_instance.h"

using namespace std;
'''
    return strs

def gen_param_port(parameters):

    param_port_str = ''' 
        Tparam param_port['''+str(parameters[1]) + '''],
        data_type_itf weight_in['''+str(parameters[2]) + '''],
        '''
    return param_port_str

def gen_data_port(parameters):
    port_str = ''' '''
    print(parameters[0])
    if str(parameters[0]) == '1':
        port_str = '''
        data_type_itf data_in_0['''+str(parameters[3])+'''],
        data_type_itf data_out_0['''+str(parameters[4])+'''],
        '''
    if str(parameters[0]) == '2':
        port_str = '''
        data_type_itf data_in_0['''+str(parameters[3])+'''],
        data_type_itf data_out_0['''+str(parameters[4])+'''],
        data_type_itf data_in_1['''+str(parameters[5])+'''],
        data_type_itf data_out_1['''+str(parameters[6])+'''],
        int select ) {
        '''
    if str(parameters[1]) == '3':
        port_str = '''
        data_type_itf data_in_0['''+str(parameters[3])+'''],
        data_type_itf data_out_0['''+str(parameters[4])+'''],
        data_type_itf data_in_1['''+str(parameters[5])+'''],
        data_type_itf data_out_1['''+str(parameters[6])+'''],
        data_type_itf data_in_2['''+str(parameters[7])+'''],
        data_type_itf data_out_2['''+str(parameters[8])+'''],
        int select ) {
        '''
    return port_str

def gen_param_pragma(parameters):

    param_pragma = '''
#pragma HLS INTERFACE s_axilite port=return bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=select bundle=CRTL_BUS

#pragma HLS INTERFACE s_axilite port=param_port bundle=CRTL_BUS
#pragma HLS INTERFACE m_axi port=param_port offset=slave depth='''+str(parameters[1])+''' bundle=PARAM_IN
//#pragma HLS INTERFACE s_axilite port=bias_in bundle=CRTL_BUS
//#pragma HLS INTERFACE m_axi port=bias_in offset=slave depth='''+str(parameters[2])+''' bundle=BIAS_IN
#pragma HLS INTERFACE s_axilite port=weight_in bundle=CRTL_BUS
#pragma HLS INTERFACE m_axi port=weight_in offset=slave depth='''+str(parameters[2])+''' bundle=WEIGHT_IN 
    '''
    return param_pragma

def gen_data_pragma(parameters):

    data_pragma = '''
#pragma HLS INTERFACE s_axilite port=data_in_0 bundle=CRTL_BUS
#pragma HLS INTERFACE m_axi port=data_in_0 offset=slave depth='''+str(parameters[3])+''' bundle=DATA_IN     
    '''
    if str(parameters[0]) == '1':
        data_pragma += '''
#pragma HLS INTERFACE bram port=data_out_0
    '''
    if str(parameters[0]) == '2':
        data_pragma += '''
#pragma HLS INTERFACE bram port=data_out_0
#pragma HLS INTERFACE bram port=data_in_1
#pragma HLS INTERFACE bram port=data_out_1        
        '''
    if str(parameters[0]) == '3':
        data_pragma += '''
#pragma HLS INTERFACE bram port=data_out_0
#pragma HLS INTERFACE bram port=data_in_1
#pragma HLS INTERFACE bram port=data_out_1 
#pragma HLS INTERFACE bram port=data_in_2
#pragma HLS INTERFACE bram port=data_out_2        
            '''
    return data_pragma

def gen_offset(idx, parameters):

    offset_str = ''' '''
    if str(parameters[0]) == '1':
        offset_str = ''' 
    int acc0_mem_inport_offset = 0;
    int acc0_mem_outport_offset = 0;
    
    if (select == 0)
    {
        acc0_mem_inport_offset = 0;
        acc0_mem_outport_offset = 0;
    }
    else
    {
        acc0_mem_inport_offset = 0;
        acc0_mem_outport_offset = 0;
    }
    '''
    if str(parameters[0]) == '2':
        offset_str = ''' 
    int acc0_mem_inport_offset = 0;
	int acc0_mem_outport_offset = 0;
	int acc1_mem_inport_offset = 0;
	int acc1_mem_outport_offset = 0;

	if (select == 0)
	{
		acc0_mem_inport_offset = 0;
		acc0_mem_outport_offset = 0;
		acc1_mem_inport_offset = '''+str(int(int(parameters[3])/2))+ ''';
		acc1_mem_outport_offset = '''+str(int(int(parameters[5])/2))+''';
	}
	else
	{
		acc0_mem_inport_offset = '''+str(int(int(parameters[3])/2))+''';
		acc0_mem_outport_offset = '''+str(int(int(parameters[5])/2))+ ''';
		acc1_mem_inport_offset = 0;
		acc1_mem_outport_offset = 0;
	}
    '''
    if str(parameters[0]) == '3':
        offset_str = ''' 
    int acc0_mem_inport_offset = 0;
    int acc0_mem_outport_offset = 0;
    int acc1_mem_inport_offset = 0;
    int acc1_mem_outport_offset = 0;
    int acc2_mem_inport_offset = 0;
    int acc2_mem_outport_offset = 0;

    if (select == 0)
    {
        acc0_mem_inport_offset = 0;
        acc0_mem_outport_offset = 0;
        acc1_mem_inport_offset = ''' + str(int(int(parameters[3]) / 2)) + ''';
        acc1_mem_outport_offset = ''' + str(int(int(parameters[5]) / 2)) + ''';
        acc2_mem_inport_offset = 0;
        acc2_mem_outport_offset = 0;
    }
    else
    {
        acc0_mem_inport_offset = ''' + str(int(int(parameters[3]) / 2)) + ''';
        acc0_mem_outport_offset = ''' + str(int(int(parameters[5]) / 2)) + ''';
        acc1_mem_inport_offset = 0;
        acc1_mem_outport_offset = 0;
        acc2_mem_inport_offset = ''' + str(int(int(parameters[7]) / 2)) + ''';
        acc2_mem_outport_offset = ''' + str(int(int(parameters[9]) / 2)) + ''';
    }
    '''

    return offset_str

def gen_convpool_func(parameters):

    func_bd = ''' '''
    for i in range(0, int(parameters[0])):
        func_bd += '''
    conv_pool_acc_'''+str(i)+'''(param_port + '''+str(i*512)+''',       
                //bias_in,       
                weight_in,  
                data_in_'''+str(i)+''' + acc'''+str(i)+'''_mem_inport_offset,  
                data_out_'''+str(i)+''' +acc'''+str(i)+'''_mem_outport_offset); 
    '''
    return func_bd

def gen_subnet_func(idx, parameters):

    print(idx)
    param_port = gen_param_port(parameters)
    data_port = gen_data_port(parameters)
    param_pragma = gen_param_pragma(parameters)
    data_pragma = gen_data_pragma(parameters)
    offset_str = gen_offset(idx, parameters)
    convpool_func_bd = gen_convpool_func(parameters)

    subnet_func_bd = '''void sub_net_''' + str(idx) + '''( ''' \
                     + param_port \
                     + data_port \
                     + param_pragma \
                     + data_pragma \
                     + offset_str \
                     + convpool_func_bd \
                     + ''' };'''

    return subnet_func_bd

def construct_function(idx, parameters):

    func_body = ''' '''

    '''
        data_type_itf data_in_0[2048],
        data_type_itf data_out_0[2048],
        data_type_itf data_in_1[2048],
        data_type_itf data_out_1[2048],
        int select
    ){
        
    '''
    return func_body


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

    print(ps_list)

    return ps_list


def generate_consnet(ps_file, store_file):
    ps = load_parameter(ps_file)
    keys = ["sub_net_0", "sub_net_1", "sub_net_2"]
    print(keys)
    with open(store_file, "w") as wf:
        sub_head = gen_sub_head()
        wf.write(sub_head + "\n")
        sub_net_counter = 0
        for key in keys:
            if key not in ps:
                continue
            lists = ps[key]
            print("sub net counter:", sub_net_counter)
            print("lists", lists[0], "list len(): ", len(lists))
            # for i in range(len(lists)):
            func = gen_subnet_func(sub_net_counter, lists[0])
            wf.write(func + "\n\n")
            sub_net_counter += 1
        wf.write("#endif\n")
    print("ok")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--params", help="accelerator param file")
    args = parser.parse_args()
    generate_consnet(args.params, "construct_net.h")
