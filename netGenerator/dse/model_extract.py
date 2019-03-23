import helping_functions
import sys

# this function is used to extract the network model information based on the key words in the
# net_config_params.txt file
def model_extract(include_fc):

    arr = helping_functions.read_params(sys.argv[1])
    prms, prms_str = helping_functions.extraction(arr)

    init_conv_N = prms[prms_str.index("nn_in_number_conv")]
    init_conv_r = prms[prms_str.index("nn_in_data_size_conv")]
    init_conv_M = prms[prms_str.index("nn_out_number_conv")]
    init_conv_P = prms[prms_str.index("nn_padding_conv")]
    init_conv_K = prms[prms_str.index("nn_channel_size_conv")]
    init_conv_S = prms[prms_str.index("nn_stride_conv")]
    init_conv_G = prms[prms_str.index("nn_group_conv")]
    init_fc_N   = prms[prms_str.index("nn_in_number_fc")]
    init_fc_Rin = prms[prms_str.index("nn_in_data_size_fc")]
    init_fc_M   = prms[prms_str.index("nn_out_number_fc")]
    init_fc_K   = prms[prms_str.index("nn_channel_size_fc")]
    init_pool_N = prms[prms_str.index("nn_in_data_size_pooling")]
    cut_flag_conv = prms[prms_str.index("conv_cut_flag")]
    cut_flag_pool = prms[prms_str.index("pool_cut_flag")]
    cut_flag_fc   = prms[prms_str.index("fc_cut_flag")]

    nn_in_number_conv_values1 = []
    if isinstance(init_fc_N, list):
        for fc_in_number in init_fc_N:
            nn_in_number_conv_values1.append(fc_in_number)
    else:
        nn_in_number_conv_values1.append(0)

    nn_out_number_conv_values1 = []
    if isinstance(init_fc_M, list):
        for fc_out_number in init_fc_M:
            nn_out_number_conv_values1.append(fc_out_number)
    else:
        nn_out_number_conv_values1.append(0)

    nn_fc_sizes_conv = []
    if isinstance(init_fc_Rin, list):
        for fc_in_size in init_fc_Rin:
            nn_fc_sizes_conv.append(fc_in_size)
    else:
        nn_fc_sizes_conv.append(0)

    nn_channel_size_conv_values = []
    if isinstance(init_fc_K, list):
        for kernel_size in init_fc_K:
            nn_channel_size_conv_values.append(kernel_size)
    else:
        nn_channel_size_conv_values.append(0)

    nn_stride_values1 = []
    if isinstance(init_fc_Rin, list):
        for stride_value in init_fc_Rin:
            nn_stride_values1.append(stride_value)
    else:
        nn_stride_values1.append(1)

    conv_only_M = [int(val) for val in init_conv_M]
    # print init_conv_M
    # print conv_only_M

    nn_conv_group_values = []
    if isinstance(init_conv_G, list):
        for group_value in init_conv_G:
            nn_conv_group_values.append(group_value)
        else:
            nn_conv_group_values.append(1)

    nn_fc_cut_flag = []
    if isinstance(cut_flag_fc, list):
        for cut_value in cut_flag_fc:
            nn_fc_cut_flag.append(cut_value)
    else:
        nn_fc_cut_flag.append(1)

    if(include_fc == 'include_fc'):
        init_conv_N = init_conv_N + nn_in_number_conv_values1
        init_conv_M = init_conv_M + nn_out_number_conv_values1
        init_conv_r = init_conv_r + nn_fc_sizes_conv
        init_conv_K = init_conv_K + nn_channel_size_conv_values
        init_conv_S = init_conv_S + nn_stride_values1
        cut_flag_conv = cut_flag_conv + nn_fc_cut_flag

    conv_N = [int(string) for string in init_conv_N]
    conv_M = [int(string) for string in init_conv_M]
    conv_r = [int(string) for string in init_conv_r]
    conv_K = [int(string) for string in init_conv_K]
    conv_S = [int(string) for string in init_conv_S]
    conv_P = [int(string) for string in init_conv_P]
    conv_G = [int(string) for string in init_conv_G]
    cut_flag = [int(string) for string in cut_flag_conv]

    if not init_fc_Rin:
        conv_P = conv_P + [0]
    else:
        conv_P = conv_P + [0] * len(init_fc_Rin)
        conv_P = conv_P + [0]
    conv_G = [int(string) for string in init_conv_G]
    max_conv_N = max(conv_N)
    max_conv_M = max(conv_M)
    max_conv_S = max(conv_S)
    max_conv_K = max(conv_K)

    conv_R = []
    conv_layer_num = int(len(conv_r))
    for r in range(0, conv_layer_num):
        R = (conv_r[r] - conv_K[r] + conv_S[r] + 2*conv_P[r])/conv_S[r]
        conv_R.append(R)

    # find the positions of Conv layers followed by Pooling layer
    flag = [False] * conv_layer_num
    count = 0
    print(prms[0])
    print(len(prms[0]))
    for prms_index in range(len(prms[0]) - 2):
        if "Convolution" in prms[0][prms_index]:
            # if "Pooling" in prms[0][prms_index + 1] + prms[0][prms_index + 2]:
            if "Pooling" in prms[0][prms_index + 1]:
                flag[count] = True
            count += 1

    # print "conv_N: ", conv_N
    # print "conv_M: ", conv_M
    # print "conv_r: ", conv_r
    # print "conv_R: ", conv_R
    # print "conv_K: ", conv_K
    # print "conv_S: ", conv_S
    # print "flag", flag
    # print "cut_flag", cut_flag

    return conv_N, conv_M, conv_r, conv_R, conv_K, conv_S, conv_G, flag, cut_flag, init_pool_N

# if __name__ == "__main__":
#     conv_N, conv_M, conv_r, conv_R, conv_K, conv_S = model_extract()