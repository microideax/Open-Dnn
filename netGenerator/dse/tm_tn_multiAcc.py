import helping_functions
import sys
import math
from model_extract import model_extract
from model_split import model_partition_ordered
from model_split import model_split_unordered
from model_split import gop_calculate
from model_split import max_layer_dataout
from model_split import model_split_by_label
from model_split import model_split_by_list
from model_split import model_partition_ordered
# from cluster import clusters_layers_kmeans
from model_partition import partition
from model_partition import partition_to_k
from global_search import global_search
from local_search import single_item_search
from local_search import model_partition_by_gop
from local_search import local_search
from local_search import per_die_config_dse
from local_search import per_die_config_dse_multiAcc
from local_search import per_die_config_dse_multiAcc_flex
from local_search import conv_net_perf
from param_write import generate_param_file
from local_search import flatten
from task_analysis import acc_task_analysis
from task_analysis import subnet_task_analysis
import time


def print_line(stage_name):
    if stage_name == "line":
        print("-" * int(math.ceil((int(80)))))
    else:
        print("\n")
        print("-" * int(math.ceil((int(80) - len(stage_name))/2)), stage_name, "-" * int(math.ceil((int(80) - len(stage_name))/2)))

def multiAcc_dse():
    # define the network parameter containers
    conv_N = []
    conv_M = []
    conv_r = []
    conv_R = []
    conv_K = []
    conv_S = []
    flag = []
    cut_flag = []
    pool_N = []

    sub_conv_N = []
    sub_conv_M = []
    sub_conv_r = []
    sub_conv_R = []
    sub_conv_K = []
    sub_conv_S = []
    sub_flag = []

    pair_1 = []
    pair_2 = []
    pair_3 = []
    lat_1 = 0
    lat_2 = 0
    lat_3 = 0
    sub_lat_list = []
    lat_list = []

    util_1 = 0
    util_2 = 0
    util_3 = 0
    sub_util_list = []
    util_list = []

    OPs = 0
    sub_pair_list = []
    item_list = []
    pair_list = []
    overall_lat = 60551400
    layer_list = []
    gop_list = []

    """
    step 1: extract model from txt file with parameter no_include_fc / include_fc
    """
    conv_N, conv_M, conv_r, conv_R, conv_K, conv_S, conv_G, flag, cut_flag, pool_N = model_extract('no_include_fc')
    # print("Extracted cut flag: ", cut_flag)
    # print("Extracted pool flag:", flag)
    OPs = gop_calculate(conv_N, conv_M, conv_R, conv_K)
    max_layerout = max_layer_dataout(conv_N, conv_M, conv_R, conv_K)

    print_line("Model extract phase")
    print("1: ", "Model extracted")
    print("1: ", "Overall convolution operation required: ", OPs)
    print("1: ", "Max layer output data: ", max_layerout)
    # print_line("Model split finish")

    """
    step 2: randomly cluster, param k=4, layer label results are in item
    """
    print_line("Model partition phase")
    for i in range(0, len(conv_N)):
        layer_list.append(i)
    # kmeans=clusters_layers_kmeans(conv_N, conv_M, conv_r, conv_R, conv_K, conv_S, 2)
    # print kmeans
    partition_location, diff_ratio = model_partition_by_gop(conv_N, conv_M, conv_r, conv_R, conv_K, conv_S, conv_G, flag, cut_flag)
    print("2: layers extracted", conv_N)
    print("2: layers cutable  ", cut_flag)
    print("2: partition location", partition_location)
    print("2: diff_ratio: ", diff_ratio)

    sub_conv_N, sub_conv_M, sub_conv_r, sub_conv_R, sub_conv_K, sub_conv_S, sub_flag \
        =model_partition_ordered(conv_N, conv_M, conv_r, conv_R, conv_K, conv_S, conv_G, flag, partition_location[0]+1, partition_location[1]+1)
    # print "2: Best partition output: ", partition_location, diff_ratio
    print("2:", sub_conv_N)

    sub_gop_list = []
    for i in range(0, len(sub_conv_N)):
        sub_gop_list.append(gop_calculate(sub_conv_N[i], sub_conv_M[i], sub_conv_R[i], sub_conv_K[i]))


    print("2: gop of sub_nets", sub_gop_list)
    print("2: length of sub_conv_N", len(sub_conv_N[0]), len(sub_conv_N[1]), len(sub_conv_N[2]))
    print("2", sub_flag)
    print("2: length of sub_flag", len(sub_flag[0]), len(sub_flag[1]), len(sub_flag[2]))
    sub_pair_list = []
    sub_lat_list = []
    sub_util_list = []

    print_line("Best Configuration Search")
    overall_start = time.time()
    # acc_cluster_num = 3
    # pair_list, item_list, gop_list, util_list = global_search(layer_list, acc_cluster_num, conv_N, conv_M, conv_r, conv_R, conv_K, conv_S, flag, overall_lat)
    # pair_list, gop_list, util_list = per_die_config_dse_multiAcc(sub_conv_N, sub_conv_M, sub_conv_r, sub_conv_R, sub_conv_K,
                                                              # sub_conv_S, sub_flag)
    pair_list = per_die_config_dse_multiAcc_flex(sub_conv_N, sub_conv_M, sub_conv_r, sub_conv_R, sub_conv_K, sub_conv_S, sub_flag)

    overall_end = time.time()

    print_line("DSE Output")
    print("Best Configuration Search Results for layer accelerators: ")
    for i in range(0, len(pair_list)):
        print(pair_list[i])

    acc_task_list, total_acc_num = acc_task_analysis(pair_list, sub_conv_N, sub_conv_M, sub_conv_r, \
                                      sub_conv_R, sub_conv_K, sub_conv_S, sub_flag)

    print("Accelerator task list: ")
    for acc_num in range(0, len(acc_task_list)):
        print("acc core", acc_num, " task list: ", acc_task_list[acc_num])

    print_line("Subnet Task Out")
    subnet_task_list = subnet_task_analysis(pair_list, acc_task_list, sub_conv_N, sub_conv_M, sub_conv_r, \
                                      sub_conv_R, sub_conv_K, sub_conv_S, sub_flag)
    print("sub net interface list:")
    for i in range(0, len(subnet_task_list)):
        print(subnet_task_list[i])


    print_line("Write out configurations")
    print(len(pair_list), "sub-nets are generated")
    print(total_acc_num, "accelerators are written into the cofig file")
    generate_param_file(pair_list, pool_N, acc_task_list, subnet_task_list, "acc_ins_params.txt")

    print_line("netGen run time system info")
    print("Overall time cost:", overall_end - overall_start, "s")
    print_line("line")


    print_line("test")
    print(conv_net_perf(sub_conv_N[2], sub_conv_M[2], sub_conv_R[2], sub_conv_S[2], sub_conv_K[2], sub_flag[2], 8, 274, 37, 4, 4))


if __name__ == "__main__":
    conv_N = multiAcc_dse()
