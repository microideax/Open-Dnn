import cv2
import caffe
import numpy as np
import argparse
import os
import math
import time
from caffe.proto import caffe_pb2
from google.protobuf import text_format

def write_param_inline(name_string, param_list, txt_file):
    f = open(txt_file, "a")
    strs = name_string
    f.write(strs)
    for n in range(0, len(param_list)):
        f.write(str(param_list[n]) + " ")
    f.write('\n')
    f.close()
    


def extract_caffe_model(model, weights, output_path, storefile):
    """extract caffe model's parameters and write them to files
    Args:
      model: path of '.prototxt'
      weights: path of '.caffemodel'
      output_path: output path of numpy params
    Returns:
      None
    """

    nn_layer_type = []
    nn_in_data_size_conv = []
    nn_channel_size_conv = []
    nn_padding_conv = []
    nn_stride_conv = []
    nn_in_number_conv = []
    nn_out_number_conv = []
    nn_group_conv = []
    nn_bias_conv = []
    nn_in_data_size_pooling = []
    nn_channel_size_pooling = []
    nn_padding_pooling = []
    nn_stride_pooling = []
    nn_in_number_pooling = []
    nn_in_data_size_fc = []
    nn_in_number_fc = []
    nn_out_number_fc = []
    nn_channel_size_fc = []


    # np.set_printoptions(threshold='nan')

    net_unit = []
    net = caffe.Net(model, caffe.TEST)
    # net.copy_from(weights)
    parsible_net = caffe_pb2.NetParameter()
    text_format.Merge(open(args.model).read(), parsible_net)
    print(parsible_net)

    # if not os.path.exists(output_path):
    #     os.makedirs(output_path)

    layer_out_dim = []
    layer_name_list = []
    layer_param_list = []
    layer_input_list = []

    print("----------------------layer_name, feature dim in net blobs--------------------------")
    for layer_name, dim in net.blobs.items():
        layer_name_list.append(layer_name)
        layer_out_dim.append((dim.data.shape[-1], dim.data.shape[-2]))
        layer_param_list.append([layer_name, dim.data.shape[-1], dim.data.shape[-2]])
        print(layer_name, dim.data.shape, len(dim.data.shape), dim.data.shape[1], dim.data.shape[-1], dim.data.shape[-2])
        layer_input_list.append((layer_name, dim.data.shape[1]))
        print("layer param numbers:", len(layer_param_list))
    print(layer_input_list)


    tops = net.top_names
    bots = net.bottom_names
    layer_types = net.layer_dict

    print("----------------------layer tops & layer bottoms--------------------------")
    print(tops)

    top_num = 0
    top_flag = []
    bot_flag = []
    cutable = []
    print("traverse through tops----------------------------------")
    for top_key, top_value in tops.items():
        top_num = top_num+1
        if len(top_value) > 1:
            top_flag.append(0)
            print(top_key, top_value)
        else:
            top_flag.append(1)
    bot_num = 0
    print("traverse through bottoms----------------------------------")
    for bot_key, bot_value in bots.items():
        if len(bot_value) > 1:
            bot_flag.append(0)
            print(bot_key, bot_value)
        else:
            bot_flag.append(1)
        bot_num = bot_num + 1

    print("top_flag:", top_flag)
    print("bot_flag:", bot_flag)
    unit_flag = 1
    for i in range(0, bot_num):
        if top_flag[i] == 0 and bot_flag[i] == 1:
            unit_flag = 0
        elif top_flag[i] == 1 and bot_flag[i] == 0:
            unit_flag = 1
        else:
            unit_flag = unit_flag
        print(unit_flag, end=' ')
        cutable.append(unit_flag)
    print()
    print("cutable flag:", cutable)

    print("final cutable points: ", cutable)
    layer_num = 0
    param_num = 0
    for layer in net.layers:
        # print(layer.type)
        if layer.type == 'Convolution' or layer.type == 'Pooling' or layer.type == 'Concat' or layer.type == 'InnerProduct':
            print(layer_param_list[param_num], layer.type, cutable[layer_num])
            # f.write(layer.type + " ")
            nn_layer_type.append(layer.type)
            if layer.type == 'Convolution':
                nn_in_data_size_conv.append(layer_param_list[param_num][1])
            if layer.type == 'Pooling':
                nn_in_data_size_pooling.append(layer_param_list[param_num][1])

        if layer.type == 'Convolution' or layer.type == 'Pooling' or layer.type == 'Concat' or layer.type == 'InnerProduct' or layer.type == 'LRN':
            param_num = param_num + 1
        layer_num = layer_num + 1

    layer_count = 0
    temp_layer_list = []

    # for layer in parsible_net.layer:
    #     temp_layer_list.append(layer)


    count = 0
    for layer in parsible_net.layer:
        if layer.type == "Convolution":
            kernel = layer.convolution_param.kernel_size[0] if len(layer.convolution_param.kernel_size) else 1
            stride = layer.convolution_param.stride[0] if len(layer.convolution_param.stride) else 1
            pad = layer.convolution_param.pad[0] if len(layer.convolution_param.pad) else 0

            print(layer.name)
            tmp_count = 0
            tmp_dim_list = []
            for layer_name, dim in net.blobs.items():
                tmp_dim_list.append(dim.data.shape[1])
                tmp_count = tmp_count + 1
                if tmp_count > 1:
                    if layer_name == layer.name:
                        inchannel = tmp_dim_list[-2]
            outchannel = layer.convolution_param.num_output
            group = layer.convolution_param.group
            nn_channel_size_conv.append(kernel)
            nn_stride_conv.append(stride)
            nn_padding_conv.append(pad)
            nn_in_number_conv.append(inchannel)
            nn_out_number_conv.append(outchannel)
            nn_bias_conv.append(outchannel)
            nn_group_conv.append(group)
        if layer.type == "Pooling":
            kernel = layer.pooling_param.kernel_size
            stride = layer.pooling_param.stride
            pad = layer.pooling_param.pad
            tmp_count = 0
            tmp_dim_list = []
            for layer_name, dim in net.blobs.items():
                tmp_dim_list.append(dim.data.shape[1])
                tmp_count = tmp_count + 1
                if tmp_count > 1:
                    print("previous layer info: ", layer_name, dim.data.shape[1], tmp_dim_list[-2])
                    if layer_name == layer.name:
                        in_num = tmp_dim_list[-2]
            nn_channel_size_pooling.append(kernel)
            nn_stride_pooling.append(stride)
            nn_padding_pooling.append(pad)
            nn_in_number_pooling.append(in_num)
        if layer.type == "InnerProduct":
            output = layer.inner_product_param.num_output
            tmp_count = 0
            tmp_dim_list = []
            for layer_name, dim in net.blobs.items():
                tmp_dim_list.append(dim.data.shape)
                tmp_count = tmp_count + 1
                # print("--------------------")
                # print(tmp_dim_list)
                if tmp_count > 1:
                    # print("previous layer info: ", layer_name, dim.data.shape)
                    if layer_name == layer.name:
                        if len(tmp_dim_list[-2]) == 2:
                            in_num = tmp_dim_list[-2][-1]
                        if len(tmp_dim_list[-2]) == 4:
                            in_num = tmp_dim_list[-2][-1] * tmp_dim_list[-2][-2] * tmp_dim_list[-2][-3]
            # nn_channel_size_fc
            # nn_in_data_size_fc
            nn_out_number_fc.append(output)
            nn_in_number_fc.append(in_num)

        count = count + 1



    # Writing the extracted params to an intermediate file
    write_param_inline("Network Structure: ", nn_layer_type, storefile)
    write_param_inline("nn_in_data_size_conv: ", nn_in_data_size_conv, storefile)
    write_param_inline("nn_channel_size_conv: ", nn_channel_size_conv, storefile)
    write_param_inline("nn_padding_conv: ", nn_padding_conv, storefile)
    write_param_inline("nn_stride_conv: ", nn_stride_conv, storefile)
    write_param_inline("nn_in_number_conv: ", nn_in_number_conv, storefile)
    write_param_inline("nn_out_number_conv: ", nn_out_number_conv, storefile)
    write_param_inline("nn_group_conv: ", nn_group_conv, storefile)
    write_param_inline("nn_bias_conv: ", nn_bias_conv, storefile)
    write_param_inline("nn_in_data_size_pooling: ", nn_in_data_size_pooling, storefile)
    write_param_inline("nn_channel_size_pooling: ", nn_channel_size_pooling, storefile)
    write_param_inline("nn_padding_pooling: ", nn_padding_pooling, storefile)
    write_param_inline("nn_stride_pooling: ", nn_stride_pooling, storefile)
    write_param_inline("nn_in_number_pooling: ", nn_in_number_pooling, storefile)
    write_param_inline("nn_in_data_size_fc: ", nn_in_data_size_fc, storefile)
    write_param_inline("nn_in_number_fc: ", nn_in_number_fc, storefile)
    write_param_inline("nn_out_number_fc: ", nn_out_number_fc, storefile)
    write_param_inline("nn_channel_size_fc: ", nn_channel_size_fc, storefile)



if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--model", help="model prototxt path .prototxt")
    parser.add_argument("--weights", help="caffe model weights path .caffemodel")
    parser.add_argument("--output", help="output path")
    args = parser.parse_args()
    extract_caffe_model(args.model, args.weights, args.output, "net_config_params.txt")
    # gen_net_config_params("net_config_params.txt")
