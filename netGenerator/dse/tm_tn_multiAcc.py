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
import time

def print_line(stage_name):
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
    conv_N, conv_M, conv_r, conv_R, conv_K, conv_S, conv_G, flag, cut_flag = model_extract('no_include_fc')
    print("Extracted cut flag: ", cut_flag)
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
        sub_gop_list.append(gop_calculate(sub_conv_N[i], sub_conv_M[i], sub_conv_r[i], sub_conv_K[i]))


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

    print_line("DSEoutpout")
    print("Best Configuration Search Results: ")
    for i in range(0, len(pair_list)):
        print(pair_list[i])

    # print item_list
    #print "gop_list: ",  gop_list
    #print "pair_list: ", pair_list
    #print "util_list: ", util_list
    # for i in range(0, len(util_list)):
    #     print util_list[i], sum(util_list[i])
    print("------------------------Final optimal configuration-------------------------------")
    # print "Network clustered results =", item_list[util_list.index(min(util_list))]
    # print "<Tm, Tn> = ", pair_list[util_list.index(min(util_list))]
    # print "Estimated overall latency = ", min(util_list)
    print("Overall time cost:", overall_end - overall_start, "s")
    print("----------------------------------------------------------------------------------")

    # item = return_partition(layer_list, 4, False)
    #
    # '''step 3: split the layers based on label clustering results'''
    # print("layer number is: ", int(len(conv_N)))
    # sub_conv_N, sub_conv_M, sub_conv_r, sub_conv_R, sub_conv_K, sub_conv_S, sub_flag \
    #     = model_split_by_list(conv_N, conv_M, conv_r, conv_R, conv_K, conv_S, flag, item)
    # print sub_conv_N
    # print "model clustering test done!!!"
    #
    # '''step 4: do local search for all sub-models and find optimial <Tm, Tn> pair, lat, and util'''
    # sub_pair_list, sub_lat_list, sub_util_list = \
    #     local_search(sub_conv_N, sub_conv_M, sub_conv_r, sub_conv_R, sub_conv_K, sub_conv_S, sub_flag)
    # print sub_pair_list, sub_lat_list, sub_util_list
    #
    # if max(sub_lat_list) < overall_lat:
    #     overall_lat = max(sub_lat_list)
    #     if len(pair_list) < 10:
    #         pair_list.append(sub_pair_list)
    #         pair_list.append([overall_lat])
    #     else:
    #         max_among_mins = pair_list.index(max(overall_lat))
    #         pair_list.remove(pair_list[max_among_mins])
    #         pair_list.append(sub_pair_list)
    #         pair_list.append([overall_lat])

    # print(pair_1, "%.2f" % util_1, pair_2, "%.2f" % util_2, pair_3, "%.2f" % util_3, lat_1, lat_2, lat_3)
    # for i in range(1, int(len(conv_N)-1)):
    #     for j in range(int(i+1), int(len(conv_N))):
    # for i in range(1, 10):
    #     for j in range(1, 10):
    #         sub_conv_N, sub_conv_M, sub_conv_r, sub_conv_R, sub_conv_K, sub_conv_S, sub_flag = model_split_ordered(conv_N, conv_M, conv_r, conv_R, conv_K, conv_S, flag, i, j)
    # sub_conv_N, sub_conv_M, sub_conv_r, sub_conv_R, sub_conv_K, sub_conv_S, sub_flag = model_split_unordered(conv_N, conv_M, conv_r, conv_R, conv_K, conv_S, flag)
    # sub_conv_N, sub_conv_M, sub_conv_r, sub_conv_R, sub_conv_K, sub_conv_S, sub_flag = model_split_by_label(conv_N, conv_M, conv_r, conv_R, conv_K, conv_S, flag, kmeans.labels_)
    # print(sub_conv_N)
    # pair_1, lat_1, pair_2, lat_2, pair_3, lat_3, util_1, util_2, util_3 = local_search(sub_conv_N, sub_conv_M, sub_conv_r, sub_conv_R, sub_conv_K, sub_conv_S, sub_flag)
    #
    # print(i, j, pair_1, "%.2f" % util_1, pair_2, "%.2f" % util_2, pair_3, "%.2f" % util_3, lat_1, lat_2, lat_3)
    #
    # if max(lat_1, lat_2, lat_3) < overall_lat:
    #     overall_lat = max(lat_1, lat_2, lat_3)
    #     # if len(pair_list) < 50:
    #     pair_list.append([i, j])
    #     pair_list.append(pair_1)
    #     pair_list.append(pair_2)
    #     pair_list.append(pair_3)
    #     pair_list.append([overall_lat])
    #     # else:
    #     #     max_among_mins  = pair_list.index(max(overall_lat))
    #     #     pair_list.remove(pair_list[max_among_mins])
    #     #     pair_list.append(pair_1)
    #     #     pair_list.append(pair_2)
    #     #     pair_list.append(pair_3)
    #     #     pair_list.append(overall_lat)
    # print(pair_list)

    # #step 3:
    # find_min_in_pairs()
    # min_among_mins = pair_list.index(min(overall_lat))
    # print(pair_list[min_among_mins])

    print("---------------------------- test part -------------------------------------------")
    print(conv_net_perf(sub_conv_N[2], sub_conv_M[2], sub_conv_R[2], sub_conv_S[2], sub_conv_K[2], sub_flag[2], 8, 274, 37, 4, 4))


if __name__ == "__main__":
    conv_N = multiAcc_dse()
