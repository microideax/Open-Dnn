import helping_functions
import sys
import math
from model_partition import partition_to_k
# from model_split import model_split_by_list
from model_split import gop_calculate
from model_split import model_partition_ordered
import pprint
import threading
import multiprocessing
import time
# from compiler.ast import flatten
# from funcy import flatten


def flatten(alist):
    new_list = []
    for item in alist:
        if isinstance(item, (list, tuple)):
            new_list.extend(flat(item))
        else:
            new_list.append(item)
    return new_list


# convolutional layer performance
def conv_layer_perf(n, m, r, s, k, Tn, Tm, P_const, Tr, Tc):
    tmp = 0

    # revised layer performance
    R_iter = math.ceil(r / float(Tr))
    M_iter = math.ceil(m / float(Tm))
    N_iter = math.ceil(n / float(Tn))
    lat_read = math.ceil((min(Tn, n)/float(32))) * ((Tr-1)*s + k) * ((Tr-1)*s + k)
    # lat_read = 0
    lat_w_read = math.ceil(min(Tn, n)/float(32)) * Tm * k * k

    # if n == 3:
    #     lat_com = Tr * Tc * math.ceil(k*k)
        # lat_com = 0
    # else:
    lat_com = Tr * Tc * k * k

    lat_out = math.ceil(Tm/float(8)) * math.ceil(Tr) * math.ceil(Tc)
    # lat_out = 0
    # tmp = R_iter * R_iter * M_iter * (lat_read + N_iter*lat_com + lat_out)
    # tmp = R_iter * R_iter * M_iter * N_iter * lat_com
    tmp = R_iter * R_iter * M_iter * (lat_read + (N_iter) * max(lat_read, lat_com) + lat_out)

    return tmp

def conv_layer_perf_x(n, m, r, s, k, Tn, Tm, P_const, Tr, Tc, ln):
    tmp = 0

    # revised layer performance
    R_iter = math.ceil(r / float(Tr))
    M_iter = math.ceil(m / float(Tm))
    N_iter = math.ceil(n / float(Tn))
    lat_read = math.ceil((min(Tn, n)/float(8))) * ((Tr-1)*s + k) * ((Tr-1)*s + k)
    # lat_read = 0
    if ln ==1:
        lat_com = Tr * Tc * math.ceil(k * k /2)
    else:
        lat_com = Tr * Tc * k * k
    lat_out = math.ceil(Tm/float(8)) * math.ceil(Tr) * math.ceil(Tc)
    # lat_out = 0
    # tmp = R_iter * R_iter * M_iter * (lat_read + N_iter*lat_com + lat_out)
    # tmp = R_iter * R_iter * M_iter * N_iter * lat_com
    tmp = R_iter * R_iter * M_iter * ((N_iter + 1) * max(lat_read, lat_com) + lat_out)

    return tmp


def pool_layer_perf(m, r, k, Tm, P_const):
    tmp = (math.ceil(m / float(Tm))) * r * r * 3 * 3 + P_const
    return tmp


# TODO: complete with new layer performance model
# fc layer performance
def fc_layer_perf(n, m, r, k, Tn, Tm, P_const):
    tmp = 0
    tmp += (math.ceil(n / float(Tn))) * (math.ceil(m / float(Tm))) * r * r * k * k + P_const
    return tmp

# sub-net performance model function
def conv_net_perf(N, M, R, S, K, flag, Tn, Tm, P_const, Tr, Tc):
    tmp = 0
    # Tr = 16
    # Tc = 16
    for j in range(0, int(len(N))):
        if flag[j] == True:
            tmp += conv_layer_perf(N[j], M[j], R[j], S[j], K[j], Tn, Tm, P_const, Tr, Tc)
            tmp += pool_layer_perf(M[j], R[j], K[j], Tm, P_const)
        else:
            tmp += conv_layer_perf(N[j], M[j], R[j], S[j], K[j], Tn, Tm, P_const, Tr, Tc)
    return tmp

def conv_net_perf_theo(N, M, R, S, K, flag, Tn, Tm, P_const):
    tmp = 0
    for i in range(0, int(len(N))):
        tmp += int(N[i])*int(M[i])*int(R[i])*int(R[i])*int(K[i])*int(K[i])
    return tmp


def model_partition_by_gop(conv_N, conv_M, conv_r, conv_R, conv_K, conv_S, conv_G, flag, cut_flag):
    sub_conv_N = []
    sub_conv_M = []
    sub_conv_r = []
    sub_conv_R = []
    sub_conv_K = []
    sub_conv_S = []
    sub_flag = []
    balance_ratio = 0
    min_ration = 0.5
    min_pair = [0,0]
    sub_gops = [[],[],[]]
    model_len = int(len(conv_N))
    for i in range(0, model_len - 2):
        for j in range(i+1, model_len - 1):
            sub_conv_N, sub_conv_M, sub_conv_r, sub_conv_R, sub_conv_K, sub_conv_S, sub_flag = model_partition_ordered(
                conv_N, conv_M, conv_r, conv_R, conv_K, conv_S, conv_G, flag, i+1, j+1)
            # print sub_conv_N
            for k in range(0, 3):
                sub_gops[k] = gop_calculate(sub_conv_N[k], sub_conv_M[k], sub_conv_R[k], sub_conv_K[k])
                # sub_gops[k] = conv_net_perf_theo(sub_conv_N[k], sub_conv_M[k], sub_conv_R[k], sub_conv_K[k])
            balance_ratio = (max(sub_gops) - min(sub_gops))/float(min(sub_gops))
            # print sub_gops

            # print "2: ", i, j, sub_gops, balance_ratio, sub_conv_N, sub_conv_M
            print("Verigy cut status: ", i, j, cut_flag[i], cut_flag[j])
            if i == 0 and j == 1:
                min_ration = balance_ratio
                print("initial balance ratio: ", balance_ratio)
                min_pair = [i, j]
            else:
                if cut_flag[i] == 1 & cut_flag[j] == 1:
                    if balance_ratio < min_ration:
                        min_ration = balance_ratio
                        min_pair = [i, j]
                        print("min_ratio: ", min_ration, min_pair)

    return min_pair, min_ration


# Optimal Tm, Tn pair selection with given amount of DSP
def constrained_dse(N, M, r, R, K, S, flag, DSP, P_const, factor, acc_per_die):
    opt_pair = []
    cycle_per_layer = []
    min_local_cycle = 2000000000000
    BRAM_bank_total = 1440*0.8 #for each die
    buffer_bank = 0
    off_chip_bank = 0
    # for layer in range(0, len(N)):
    #     buffer_bank += int(2*M[layer]*math.ceil(R[layer]*R[layer]/float(1024)))
    # acc_bank = int((BRAM_bank_total - buffer_bank)/acc_per_die)
    # print "acc_bank, buffer_bank, M, R: ", acc_bank, buffer_bank, M[-1], R[-1]
    # if acc_bank < 0:
    #     Tr_boundary = BRAM_bank_total
    #     print "Illigal acc_bank number, memory is not enough for tmp_bank!!!"
    # else:
    Tr_boundary = int(BRAM_bank_total/acc_per_die)

    for Tr in range(1, 33):
        for Tm in range(1, max(M) + 1):
            Tn_max = min(max(N), int(int(DSP / Tm)), Tm)
            for Tn in range(1, Tn_max + 1):
                # print "Search in Tr range 1 - ", int(math.floor(math.sqrt(Tr_boundary/float(Tm))))
                local_cycles = conv_net_perf(N, M, R, S, K, flag, Tn, Tm, P_const, Tr, Tr)
                if local_cycles < min_local_cycle and local_cycles != 0 and (int((Tn+Tm)*math.ceil(Tr*Tr/1024)) < int(BRAM_bank_total/acc_per_die)):
                    min_local_cycle = local_cycles
                    opt_pair = [Tm, Tn, Tr, local_cycles]

    # collected the detailed performance for each layer in a sub-net
    # for j in range(0, int(len(N))):
    #     tmp = 0
    #     tmp = conv_layer_perf(N[j], M[j], R[j], S[j], K[j], opt_pair[1], opt_pair[0], P_const, opt_pair[2], opt_pair[2])
    #     cycle_per_layer.append(tmp)

    cycle_per_layer.append(0)

    # Acc_num = 1
    # opt_pair.append(Acc_num)

    return opt_pair, min_local_cycle, cycle_per_layer


# k: number of accelerators, 1, 2, 3 only
def split_sub_net(start_index, end_index, k):
    no_layer = end_index - start_index

    # if the layers are more than accelerators
    if k <= no_layer:
        if k == 1:
            yield [-1]
        if k == 2:
           for i in range(start_index + 1, end_index):
               yield [i]
        if k == 3:
            for i in range(start_index + 1, end_index):
                for j in range(i + 1, end_index):
                    yield [i, j]

    # if the layers are less than accelerators
    else:
        yield [-2]



# by John: find the optimal number of accelerators in each sub-net
def per_die_config_dse_multiAcc_flex(sub_conv_N, sub_conv_M, sub_conv_r, sub_conv_R, sub_conv_K, sub_conv_S, sub_flag):

    print("sub_conv_N (original): ", sub_conv_N)
    print("sub_flag (original): ", sub_flag)

    opt_res = []

    # i: iterate over each sub-net
    for i in range(0, len(sub_conv_N)):
        # print "sub_conv_N[" + str(i) + "]: ", sub_conv_N[i]
        min_cycle = sys.maxsize
        min_idx = -1

        sub_conv_net_gop = gop_calculate(sub_conv_N[i], sub_conv_M[i], sub_conv_R[i], sub_conv_K[i])
        cycle_list = []
        pair_list = []

        # when the number of accelerators is j
        for j in range(1, 3 + 1):
            # cycle should be compared here, to find optimal accelerator number and config
            lat_list = []
            start_index = 0

            # k: the index to split the sub_conv_N
            for k in split_sub_net(0, len(sub_conv_N[i]), j):
                DSP = int(6840/3*0.8)
                dsp_list = []
                local_cycle_list = []
                local_pair_list = []
                sub_net_gop_list = []
                factor = 1

                # re-caculate sub_conv_N, sub_conv_M, sub_conv_R, sub_conv_K
                sub_conv_N_new = []
                sub_conv_M_new = []
                sub_conv_r_new = []
                sub_conv_R_new = []
                sub_conv_K_new = []
                sub_conv_S_new = []
                sub_flag_new   = []

                # -2: illegal setting, pass
                if k[0] == -2:
                    print("illegal partitioning of sub-net, passing!")
                    continue

                # -1: only one accelerator
                if k[0] == -1:
                    sub_conv_N_new.append(sub_conv_N[i])
                    sub_conv_M_new.append(sub_conv_M[i])
                    sub_conv_r_new.append(sub_conv_r[i])
                    sub_conv_R_new.append(sub_conv_R[i])
                    sub_conv_K_new.append(sub_conv_K[i])
                    sub_conv_S_new.append(sub_conv_S[i])
                    sub_flag_new.append(sub_flag[i])

                # else: 2 or 3 accelerators
                else:
                    zi = list(zip([0] + k, k + [None]))
                    print("testing zi in python 3.5", zi, len(zi))
                    for idx in range(0, len(zi)):
                        sub_conv_M_new.append(flatten(sub_conv_M[i])[zi[idx][0]:zi[idx][1]])
                        sub_conv_N_new.append(flatten(sub_conv_N[i])[zi[idx][0]:zi[idx][1]])
                        sub_conv_r_new.append(flatten(sub_conv_r[i])[zi[idx][0]:zi[idx][1]])
                        sub_conv_R_new.append(flatten(sub_conv_R[i])[zi[idx][0]:zi[idx][1]])
                        sub_conv_K_new.append(flatten(sub_conv_K[i])[zi[idx][0]:zi[idx][1]])
                        sub_conv_S_new.append(flatten(sub_conv_S[i])[zi[idx][0]:zi[idx][1]])
                        sub_flag_new.append(flatten(sub_flag[i])[zi[idx][0]:zi[idx][1]])
                    print("sub_conv_N_new: ", sub_conv_N_new)


                # m: the mth sub-sub-net in the sub-net
                temp_pair_list = []
                for m in range(0, len(sub_conv_N_new)):
                    # print "sub_conv_N_new[" + str(m) + "]: ", sub_conv_N_new[m]
                    sub_net_gop_list.append(gop_calculate(sub_conv_N_new[m], sub_conv_M_new[m], sub_conv_R_new[m], sub_conv_K_new[m]))
                    # allocate_dsp by layer gops
                    dsp_list.append(math.ceil(DSP * (sub_net_gop_list[m])/sub_conv_net_gop))
                    # search best <Tm,Tn> configurations
                    pair, cycle, cycle_per_layer = constrained_dse(sub_conv_N_new[m], sub_conv_M_new[m],
                                                                     sub_conv_r_new[m], sub_conv_R_new[m],
                                                                     sub_conv_K_new[m],
                                                                     sub_conv_S_new[m], sub_flag_new[m],
                                                                     int(dsp_list[m]), int(37),
                                                                     factor, j)
                    local_cycle_list.append(cycle)
                    temp_pair_list.append(pair)

                    # local_pair_list.append(pair)

                cycle_list.append([j, k, max(local_cycle_list)])
                pair_list.append(temp_pair_list)

        # find the minimum cycles and the corresponding index for each sub-net
        for n in range(0, len(cycle_list)):
            if cycle_list[n][2] < min_cycle:
                min_cycle = cycle_list[n][2]
                min_idx = n
        opt_res.append([cycle_list[min_idx], pair_list[min_idx]])
    return opt_res


def local_search(sub_conv_N, sub_conv_M, sub_conv_r, sub_conv_R, sub_conv_K, sub_conv_S, sub_flag):
    """
    :param sub_conv_N: the input sub_conv_N is already splitted into several sub-nets
    :param sub_conv_M: same as above
    :param sub_conv_r: saa
    :param sub_conv_R: saa
    :param sub_conv_K: saa
    :param sub_conv_S: saa
    :param sub_flag: saa
    :return: the most optimal configuration for current sub-nets for an optimal system latency
    """
    DSP = 6840 / 3
    # datatype = fixed
    factor = 1

    pair_1 = []
    lat_1 = 0
    util_1 = 0
    pair_2 = []
    lat_2 = 0
    util_2 = 0
    pair_3 = []
    lat_3 = 0
    util_3 = 0

    pair_list = []
    lat_list = []
    util_list = []
    gop_list = []
    gop_per_subnet = []
    gop_total = 0
    dsp_per_acc = []
    dsp_occupied = 0
    # print "lists in sub_conv_N"
    # print len(sub_conv_N)
    # print sub_conv_N

    step = int(1)
    ratio = 0.05
    search_counter = 0
    Resolution = 10
    ratio_init = 0

    """initializing the dsp number for per acc based on the ops requirement"""
    for i in range(0, len(sub_conv_N)):
        gop_per_subnet.append(gop_calculate(sub_conv_N[i], sub_conv_M[i], sub_conv_R[i], sub_conv_K[i]))
        gop_total += gop_per_subnet[i]
    print("gop_per_subnet in local_search: ", gop_per_subnet)

    for i in range(0, len(sub_conv_N)):
        if i < len(sub_conv_N) - 1:
            dsp_per_acc.append(math.ceil(DSP * (gop_per_subnet[i]/float(gop_total))))
            dsp_occupied += dsp_per_acc[i]
        else:
            dsp_per_acc.append(math.ceil(DSP - dsp_occupied))

    """ Iteratively find the system level optimal configuration for the all the sub-nets"""
    search_stop = 0
    while search_stop == 0 and search_counter < Resolution + 1:
        for i in range(0, len(sub_conv_N)):
            pair, cycle, cycle_per_layer = constrained_dse(sub_conv_N[i], sub_conv_M[i], sub_conv_r[i], sub_conv_R[i],
                                                           sub_conv_K[i],
                                                           sub_conv_S[i], sub_flag[i], int(2200), int(37),
                                                           factor)
            pair_list.append(pair)
            lat_list.append(cycle)
            util_list.append(pair[0]*pair[1]/float(DSP))
            if len(pair_list) > len(sub_conv_N):
                for remove_cnt in range(0, len(sub_conv_N)):
                    pair_list.remove(pair_list[0])
                    lat_list.remove(lat_list[0])
                    util_list.remove(util_list[0])

        ratio_tmp = ((max(lat_list) - min(lat_list)) / float(min(lat_list)))
        # print ratio_tmp
        if search_counter == 0:
            ratio_init = ratio_tmp
            # or search_counter == Resolution:
        if ratio_tmp < ratio:
            search_stop = 1
        else:
            max_idx = lat_list.index(min(lat_list))
            min_idx = lat_list.index(max(lat_list))
            if ratio_tmp - ratio > float(0.1):
                if (dsp_per_acc[max_idx] - 5*step > 0):
                    dsp_per_acc[max_idx] = dsp_per_acc[max_idx] - 5*step
                    dsp_per_acc[min_idx] = dsp_per_acc[min_idx] + 5*step
                else:
                    dsp_per_acc[max_idx] = dsp_per_acc[max_idx] - step
                    dsp_per_acc[min_idx] = dsp_per_acc[min_idx] + step
            else:
                if (dsp_per_acc[max_idx] - step > 0):
                    dsp_per_acc[max_idx] = dsp_per_acc[max_idx] - step
                    dsp_per_acc[min_idx] = dsp_per_acc[min_idx] + step

        search_counter = search_counter + 1
    # if search_stop == 1:
        # and search_counter == 101
    print("local search stopped at =", search_counter - 1, "current ratio: ", ratio_tmp)
    print("initial ratio ->", ratio_init)

    return pair_list, lat_list, util_list

def constrained_dse_layer(N, M, r, R, K, S, flag, DSP, P_const, factor):
    opt_pair = []
    cycle_per_layer = []
    min_local_cycle = 2000000000000

    for Tm in range(1, M + 1):
        Tn_max = min(N, int(int(DSP / Tm)), Tm)
        for Tn in range(1, Tn_max + 1):
            local_cycles = conv_layer_perf(N, M, R, S, K, Tn, Tm, P_const=37)
            if local_cycles < min_local_cycle and local_cycles != 0:
                min_local_cycle = local_cycles
                opt_pair = [Tm, Tn, local_cycles]

    # collected the detailed performance for each layer in a sub-net
    # for j in range(0, int(len(N))):
    #     tmp = 0
    #     tmp = conv_layer_perf(N[j], M[j], R[j], S[j], K[j], opt_pair[1], opt_pair[0], P_const)
    cycle_per_layer.append(local_cycles)

    Acc_num = 1
    opt_pair.append(Acc_num)

    return opt_pair, min_local_cycle, cycle_per_layer

def per_die_config_dse(sub_conv_N, sub_conv_M, sub_conv_r, sub_conv_R, sub_conv_K, sub_conv_S, sub_flag):
    DSP = 6840 / 3
    pair_list = []
    lat_list = []
    util_list = []
    factor = 1
    opt_ratio = 0

    for i in range(0, len(sub_conv_N)):
        pair, cycle, cycle_per_layer = constrained_dse(sub_conv_N[i], sub_conv_M[i], sub_conv_r[i], sub_conv_R[i],
                                                       sub_conv_K[i],
                                                       sub_conv_S[i], sub_flag[i], int(2200*2), int(37),
                                                       factor)
        pair_list.append(pair)
        lat_list.append(cycle)
        util_list.append(pair[0] * pair[1] / float(DSP))
        if len(pair_list) > len(sub_conv_N):
            for remove_cnt in range(0, len(sub_conv_N)):
                pair_list.remove(pair_list[0])
                lat_list.remove(lat_list[0])
                util_list.remove(util_list[0])
    #
    # ratio_tmp = ((max(lat_list) - min(lat_list)) / float(min(lat_list)))
    # print "initial diff_ratio: ", ratio_tmp
    #
    # max_lat_index = lat_list.index(max(lat_list))
    # # find the max latency sub_net
    # for j in range(0, len(sub_conv_N[max_lat_index])):
    #     if len(sub_conv_N[max_lat_index]) >=4:
    #         max_acc_num = 4
    #     else:
    #         max_acc_num = len(sub_conv_N[max_lat_index])
    #     for acc_num in range(0, max_acc_num):
    #         #TODO: keep partitioning the sub_net and search the best number of acc and corresponding configuration

    return pair_list, lat_list, util_list

# by Yao: fix the number of accelerators to the number of layers
def per_die_config_dse_multiAcc(sub_conv_N, sub_conv_M, sub_conv_r, sub_conv_R, sub_conv_K, sub_conv_S, sub_flag):
    DSP = 6840 / 3
    dsp_list = []
    pair_list = []
    lat_list = []
    util_list = []
    factor = 1
    opt_ratio = 0

    for i in range(0, len(sub_conv_N)):
        dsp_list.append([])
        sub_net_gop = gop_calculate(sub_conv_N[i], sub_conv_M[i], sub_conv_R[i], sub_conv_K[i])
        for j in range(0, len(sub_conv_N[i])):
            # allocate_dsp by layer gops
            dsp_list[i].append(DSP * (sub_conv_N[i][j]*sub_conv_M[i][j]*sub_conv_R[i][j]*sub_conv_R[i][j]*sub_conv_K[i][j]*sub_conv_K[i][j]) / sub_net_gop)
            # do contrained dse for layer
            pair, cycle, cycle_per_layer = constrained_dse_layer(sub_conv_N[i][j], sub_conv_M[i][j], sub_conv_r[i][j], sub_conv_R[i][j],
                                                           sub_conv_K[i][j],
                                                           sub_conv_S[i][j], sub_flag[i][j], int(dsp_list[i][j]), int(37),
                                                           factor)
            pair_list.append(pair)
            lat_list.append(cycle)
            util_list.append(pair[0] * pair[1] / float(int(dsp_list[i][j])))
    print("dsp_list value: ", dsp_list, pair_list)
    print("util_list value: ", util_list)

            # note done best configuration

    for i in range(0, len(sub_conv_N)):
        pair, cycle, cycle_per_layer = constrained_dse(sub_conv_N[i], sub_conv_M[i], sub_conv_r[i], sub_conv_R[i],
                                                       sub_conv_K[i],
                                                       sub_conv_S[i], sub_flag[i], int(DSP), int(37),
                                                       factor)

        if len(pair_list) > len(sub_conv_N):
            for remove_cnt in range(0, len(sub_conv_N)):
                pair_list.remove(pair_list[0])
                lat_list.remove(lat_list[0])
                util_list.remove(util_list[0])
    #
    # ratio_tmp = ((max(lat_list) - min(lat_list)) / float(min(lat_list)))
    # print "initial diff_ratio: ", ratio_tmp
    #
    # max_lat_index = lat_list.index(max(lat_list))
    # # find the max latency sub_net
    # for j in range(0, len(sub_conv_N[max_lat_index])):
    #     if len(sub_conv_N[max_lat_index]) >=4:
    #         max_acc_num = 4
    #     else:
    #         max_acc_num = len(sub_conv_N[max_lat_index])
    #     for acc_num in range(0, max_acc_num):
    #         #TODO: keep partitioning the sub_net and search the best number of acc and corresponding configuration

    return pair_list, lat_list, util_list


def single_item_search(layer_list, acc_cluster_num, conv_N, conv_M, conv_r, conv_R, conv_K, conv_S, flag, pair_list,
                  overall_lat):
    """
    :param layer_list: a list containing each layer information in the form of a tuple (layer index, layer name).
    :param acc_num:
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
    item_list = []
    search_counter = 0
    print("started global search")

    item = [0], [1, 2], [3, 4]

    print(item)
    sub_conv_N, sub_conv_M, sub_conv_r, sub_conv_R, sub_conv_K, sub_conv_S, sub_flag \
            = model_split_by_list(conv_N, conv_M, conv_r, conv_R, conv_K, conv_S, flag, item)
    sub_pair_list, sub_lat_list, sub_util_list = \
            local_search(sub_conv_N, sub_conv_M, sub_conv_r, sub_conv_R, sub_conv_K, sub_conv_S, sub_flag)

    if max(sub_lat_list) < overall_lat:
        overall_lat = max(sub_lat_list)
        if len(pair_list) < 3:
            item_list.append(item)
            pair_list.append(sub_pair_list)
            pair_list.append([overall_lat])

    # print "Final explored points = ", search_counter
    return pair_list, item_list

# network dsp utilization
def net_dsp_util(N, M, Tm, Tn, DSP, lat):
    overall_util = 0
    overall_lat = 0
    for j in range(0, int(len(N))):
        dsp_x = min(Tm, M[j])
        dsp_y = min(Tn, N[j])
        overall_util += (dsp_x * dsp_y * lat[j]) / float(DSP)
        overall_lat += lat[j]
    net_util = overall_util / float(overall_lat)
    return net_util


# accelerator runtime performance model
def acc_one_p_cycles(Tm, Tn, Tr, Tc, K, ):
    cycles = Tr * Tc * K * K
    return cycles

    # pair_1, lat_1, util_1 = constrained_dse(sub_conv_N[0], sub_conv_M[0], sub_conv_r[0], sub_conv_R[0], sub_conv_K[0],
    #                                 sub_conv_S[0], sub_flag[0], int(DSP/factor), int(37), factor)
    # pair_2, lat_2, util_2 = constrained_dse(sub_conv_N[1], sub_conv_M[1], sub_conv_r[1], sub_conv_R[1], sub_conv_K[1],
    #                                 sub_conv_S[1], sub_flag[1], int(DSP/factor*0.8), int(37), factor)
    # pair_3, lat_3, util_3 = constrained_dse(sub_conv_N[2], sub_conv_M[2], sub_conv_r[2], sub_conv_R[2], sub_conv_K[2],
    #                                 sub_conv_S[2], sub_flag[2], int(DSP/factor), int(37), factor)
