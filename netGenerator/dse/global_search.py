
import helping_functions
import sys
import math
from model_partition import partition_to_k
from model_split import model_split_by_list
from model_split import gop_calculate
import pprint
import threading
import multiprocessing
import time
from local_search import local_search

result_Q = multiprocessing.Queue()
PROCESS_NUM = 4


class SearchProcess(multiprocessing.Process):
    def __init__(self, param_v, processIdx, result_Q):
        multiprocessing.Process.__init__(self)
        self.layer_list = param_v[0]
        self.acc_cluster_num = param_v[1]
        self.conv_N = param_v[2]
        self.conv_M = param_v[3]
        self.conv_r = param_v[4]
        self.conv_R = param_v[5]
        self.conv_K = param_v[6]
        self.conv_S = param_v[7]
        self.flag = param_v[8]
        self.overall_lat = param_v[9]
        self.processIdx = processIdx
        self.result_Q = result_Q

    def run(self):

        start = time.time()
        process_gop_list = []
        process_item_list = []
        process_util_list = []
        process_pair_list = []

        search_counter = 0

        print "Process " + str(self.processIdx) + " starts global search."

        for idx, item in enumerate(partition_to_k(self.layer_list, self.acc_cluster_num, False), 0):
            if idx % PROCESS_NUM == self.processIdx:
                sub_gop_list = []
                search_counter = search_counter + 1
                sub_conv_N, sub_conv_M, sub_conv_r, sub_conv_R, sub_conv_K, sub_conv_S, sub_flag \
                    = model_split_by_list(self.conv_N, self.conv_M, self.conv_r, self.conv_R, self.conv_K, self.conv_S, self.flag, item)
                sub_pair_list, sub_lat_list, sub_util_list = \
                    local_search(sub_conv_N, sub_conv_M, sub_conv_r, sub_conv_R, sub_conv_K, sub_conv_S, sub_flag)

                for i in range(0, len(sub_conv_N)):
                    sub_gop_list.append(gop_calculate(sub_conv_N[i], sub_conv_M[i], sub_conv_R[i], sub_conv_K[i]))

                if max(sub_lat_list) < self.overall_lat:
                    overall_lat = max(sub_lat_list)
                    if len(process_pair_list) < 6:
                        process_item_list.append(item)
                        process_pair_list.append(sub_pair_list)
                        # process_pair_list.append([overall_lat])
                        process_util_list.append([overall_lat])
                        process_gop_list.append(sub_gop_list)
                        # process_util_list.append(sub_util_list)
                        # process_pair_list.append(sub_util_list)
                    # else:
                    #     max_among_mins = process_pair_list.index(max(overall_lat))
                    #     process_pair_list.remove(process_pair_list[max_among_mins])
                    #     process_pair_list.append(sub_pair_list)
                    #     process_pair_list.append([overall_lat])
                    #     process_pair_list.append(sub_util_list)

            # print "For set ID: " + str(idx) + ", the final explored points = ", search_counter

        if len(process_pair_list) != 0:
            self.result_Q.put((process_pair_list, process_item_list, process_gop_list, process_util_list))

        end = time.time()
        print "Thread ", self.processIdx, " :", (end - start)


def global_search(layer_list, acc_cluster_num, conv_N, conv_M, conv_r, conv_R, conv_K, conv_S, flag, overall_lat):
    """
    :param layer_list: a list containing each layer information in the form of a tuple (layer index, layer name).
    :param acc_cluster_num:
    :param conv_N:
    :param conv_M:
    :param conv_r:
    :param conv_R:
    :param conv_K:
    :param conv_S:
    :param flag:
    :param pair_list:
    :param overall_lat:
    :return:
    """
    sub_conv_N = []
    sub_conv_M = []
    sub_conv_r = []
    sub_conv_R = []
    sub_conv_K = []
    sub_conv_S = []
    sub_flag = []
    sub_pair_list = []
    sub_lat_list = []
    sub_util_list = []

    gop_list = []
    item_list = []
    util_list = []
    pair_list = []

    processes = []
    for i in range(PROCESS_NUM):
        p = SearchProcess((layer_list, acc_cluster_num, conv_N, conv_M, conv_r, conv_R, conv_K, conv_S, flag, overall_lat), i, result_Q)
        processes.append(p)

    for p in processes:
        p.start()

    for p in processes:
        p.join()

    results = list()
    while not result_Q.empty():
        results.append(result_Q.get())
    for item in results:
        pair_list = pair_list + item[0]
        item_list = item_list + item[1]
        gop_list = gop_list + item[2]
        util_list = util_list + item[3]

    return pair_list, item_list, gop_list, util_list
