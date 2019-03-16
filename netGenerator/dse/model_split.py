import sys
import math
import random


def model_partition_ordered(conv_N, conv_M, conv_r, conv_R, conv_K, conv_S, conv_G, flag, i, j):

    if len(conv_N) <= 3:
        print("Network size is smaller than 3, results are illegal!")

    sub_conv_N = []
    sub_conv_M = []
    sub_conv_r = []
    sub_conv_R = []
    sub_conv_K = []
    sub_conv_S = []
    sub_flag = []
    sub_group = []

    sub_conv_N.append(conv_N[0: int(i)])
    sub_conv_N.append(conv_N[int(i): int(j)])
    sub_conv_N.append(conv_N[int(j): int(len(conv_N))])

    sub_conv_M.append(conv_M[0: int(i)])
    sub_conv_M.append(conv_M[int(i): int(j)])
    sub_conv_M.append(conv_M[int(j): int(len(conv_N))])

    sub_conv_r.append(conv_r[0: int(i)])
    sub_conv_r.append(conv_r[int(i): int(j)])
    sub_conv_r.append(conv_r[int(j): int(len(conv_N))])

    sub_conv_R.append(conv_R[0: int(i)])
    sub_conv_R.append(conv_R[int(i): int(j)])
    sub_conv_R.append(conv_R[int(j): int(len(conv_N))])

    sub_conv_K.append(conv_K[0: int(i)])
    sub_conv_K.append(conv_K[int(i): int(j)])
    sub_conv_K.append(conv_K[int(j): int(len(conv_N))])

    sub_conv_S.append(conv_S[0: int(i)])
    sub_conv_S.append(conv_S[int(i): int(j)])
    sub_conv_S.append(conv_S[int(j): int(len(conv_N))])

    sub_flag.append(flag[0: int(i)])
    sub_flag.append(flag[int(i): int(j)])
    sub_flag.append(flag[int(j): int(len(conv_N))])

    return sub_conv_N, sub_conv_M, sub_conv_r, sub_conv_R, sub_conv_K, sub_conv_S, sub_flag


def model_split_unordered(conv_N, conv_M, conv_r, conv_R, conv_K, conv_S, flag):
    sub_conv_N = [[], [], []]
    sub_conv_M = [[], [], []]
    sub_conv_r = [[], [], []]
    sub_conv_R = [[], [], []]
    sub_conv_K = [[], [], []]
    sub_conv_S = [[], [], []]
    sub_flag = [[], [], []]

    cluster_num = []

    while (len(sub_conv_N[0]) == 0 or len(sub_conv_N[1]) == 0 or len(sub_conv_N[2]) == 0):
        cluster_num = []
        sub_conv_N = [[], [], []]
        sub_conv_M = [[], [], []]
        sub_conv_r = [[], [], []]
        sub_conv_R = [[], [], []]
        sub_conv_K = [[], [], []]
        sub_conv_S = [[], [], []]
        sub_flag = [[], [], []]
        for i in range(0, int(len(conv_N))):
            cluster_num.append(random.randrange(0, 3, 1))
        for i in range(0, int(len(conv_N))):
            sub_conv_N[cluster_num[i]].append(conv_N[i])
            sub_conv_M[cluster_num[i]].append(conv_M[i])
            sub_conv_r[cluster_num[i]].append(conv_r[i])
            sub_conv_R[cluster_num[i]].append(conv_R[i])
            sub_conv_K[cluster_num[i]].append(conv_K[i])
            sub_conv_S[cluster_num[i]].append(conv_S[i])
            sub_flag[cluster_num[i]].append(flag[i])

        print(cluster_num)
    return sub_conv_N, sub_conv_M, sub_conv_r, sub_conv_R, sub_conv_K, sub_conv_S, sub_flag


def model_split_by_label(conv_N, conv_M, conv_r, conv_R, conv_K, conv_S, flag, lablelist):
    sub_conv_N = []
    sub_conv_M = []
    sub_conv_r = []
    sub_conv_R = []
    sub_conv_K = []
    sub_conv_S = []
    sub_flag = []

    for i in range(0, max(lablelist) + 1):
        sub_conv_N.append([])
        sub_conv_M.append([])
        sub_conv_r.append([])
        sub_conv_R.append([])
        sub_conv_K.append([])
        sub_conv_S.append([])
        sub_flag.append([])

    # print sub_conv_N
    for i in range(len(conv_N)):
        sub_conv_N[lablelist[i]].append(conv_N[i])
        sub_conv_M[lablelist[i]].append(conv_M[i])
        sub_conv_r[lablelist[i]].append(conv_r[i])
        sub_conv_R[lablelist[i]].append(conv_R[i])
        sub_conv_K[lablelist[i]].append(conv_K[i])
        sub_conv_S[lablelist[i]].append(conv_S[i])
        sub_flag[lablelist[i]].append(flag[i])

    return sub_conv_N, sub_conv_M, sub_conv_r, sub_conv_R, sub_conv_K, sub_conv_S, sub_flag


def model_split_by_list(conv_N, conv_M, conv_r, conv_R, conv_K, conv_S, flag, par_list):
    sub_conv_N = []
    sub_conv_M = []
    sub_conv_r = []
    sub_conv_R = []
    sub_conv_K = []
    sub_conv_S = []
    sub_flag = []

    cluster_num = len(par_list)

    for i in range(0, cluster_num):
        sub_conv_N.append([])
        sub_conv_M.append([])
        sub_conv_r.append([])
        sub_conv_R.append([])
        sub_conv_K.append([])
        sub_conv_S.append([])
        sub_flag.append([])

    # print sub_conv_N
    for j in range(0, cluster_num):
        for i in range(0, len(par_list[j])):
            sub_conv_N[j].append(conv_N[par_list[j][i]])
            sub_conv_M[j].append(conv_M[par_list[j][i]])
            sub_conv_r[j].append(conv_r[par_list[j][i]])
            sub_conv_R[j].append(conv_R[par_list[j][i]])
            sub_conv_K[j].append(conv_K[par_list[j][i]])
            sub_conv_S[j].append(conv_S[par_list[j][i]])
            sub_flag[j].append(flag[par_list[j][i]])

    return sub_conv_N, sub_conv_M, sub_conv_r, sub_conv_R, sub_conv_K, sub_conv_S, sub_flag


def gop_calculate(conv_N, conv_M, conv_R, conv_K):
    op_layer = 0
    conv_layer_num = int(len(conv_N))

    for i in range(0, conv_layer_num):
        tmp = conv_M[i] * conv_N[i] * conv_R[i] * conv_R[i] * conv_K[i] * conv_K[i] * 2
        # print tmp, "  "
        op_layer += tmp

    gop_all = op_layer
    # print "gop_calculate():", conv_N, conv_M, conv_R, conv_K, gop_all
    return gop_all


def max_layer_dataout(conv_N, conv_M, conv_R, conv_K):
    max_dataout = 0
    conv_layer_num = int(len(conv_N))
    for i in range(0, conv_layer_num):
        layer_out = conv_M[i] * conv_R[i] * conv_R[i]
        if max_dataout < layer_out:
            max_dataout = layer_out

    return max_dataout

# if __name__ == "__main__":
#     sub_conv_N = model_split()
