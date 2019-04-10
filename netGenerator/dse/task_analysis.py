import math

def acc_task_analysis(pair_list, sub_conv_N, sub_conv_M, sub_conv_r, sub_conv_R, sub_conv_K, sub_conv_S, sub_flag):

    total_acc_num = 0
    for i in range(0, len(pair_list)):
        total_acc_num += len(pair_list[i][1])

    acc_task_list = []
    for acc_num in range(0, total_acc_num):
        acc_task_list.append([])
    print("acc_task_list: ", acc_task_list)

    acc_num_counter = 0
    for sub_net_number in range(0, len(sub_conv_N)):
        print("sub_net_", sub_net_number, " layer_number: ", len(sub_conv_N[sub_net_number]))
        print("sub_net_", sub_net_number, " layer_acc_number: ", pair_list[sub_net_number][0][0])
        print("sub_net_cutting_point: ", pair_list[sub_net_number][0][1])
        # for sub_net_acc_core in range(0, pair_list[sub_net_number][0][0]):
        print("acc core_", acc_num_counter)
        if len(pair_list[sub_net_number][0][1]) == 1 and pair_list[sub_net_number][0][1][0] == -1:
            for layer_num in range(0, len(sub_conv_N[sub_net_number])):
                local_list = []
                local_list.append(sub_net_number)
                local_list.append(sub_conv_N[sub_net_number][layer_num])
                local_list.append(sub_conv_M[sub_net_number][layer_num])
                local_list.append(sub_conv_r[sub_net_number][layer_num])
                local_list.append(sub_conv_R[sub_net_number][layer_num])
                local_list.append(sub_conv_K[sub_net_number][layer_num])
                local_list.append(sub_conv_S[sub_net_number][layer_num])
                local_list.append(sub_flag[sub_net_number][layer_num])
                acc_task_list[acc_num_counter].append(local_list)
            acc_num_counter += 1
            print("sub_net no cut")
        elif len(pair_list[sub_net_number][0][1]) == 1 and pair_list[sub_net_number][0][1][0] == 1:
            for layer_num in range(0, pair_list[sub_net_number][0][1][0]):
                local_list = []
                local_list.append(sub_net_number)
                local_list.append(sub_conv_N[sub_net_number][layer_num])
                local_list.append(sub_conv_M[sub_net_number][layer_num])
                local_list.append(sub_conv_r[sub_net_number][layer_num])
                local_list.append(sub_conv_R[sub_net_number][layer_num])
                local_list.append(sub_conv_K[sub_net_number][layer_num])
                local_list.append(sub_conv_S[sub_net_number][layer_num])
                local_list.append(sub_flag[sub_net_number][layer_num])
                acc_task_list[acc_num_counter].append(local_list)
            acc_num_counter += 1
            for layer_num in range(pair_list[sub_net_number][0][1][0], len(sub_conv_N[sub_net_number])):
                local_list = []
                local_list.append(sub_net_number)
                local_list.append(sub_conv_N[sub_net_number][layer_num])
                local_list.append(sub_conv_M[sub_net_number][layer_num])
                local_list.append(sub_conv_r[sub_net_number][layer_num])
                local_list.append(sub_conv_R[sub_net_number][layer_num])
                local_list.append(sub_conv_K[sub_net_number][layer_num])
                local_list.append(sub_conv_S[sub_net_number][layer_num])
                local_list.append(sub_flag[sub_net_number][layer_num])
                acc_task_list[acc_num_counter].append(local_list)
            acc_num_counter += 1
            print("sub net cut into 2")
        else:
            for layer_num in range(0, pair_list[sub_net_number][0][1][0]):
                local_list = []
                local_list.append(sub_net_number)
                local_list.append(sub_conv_N[sub_net_number][layer_num])
                local_list.append(sub_conv_M[sub_net_number][layer_num])
                local_list.append(sub_conv_r[sub_net_number][layer_num])
                local_list.append(sub_conv_R[sub_net_number][layer_num])
                local_list.append(sub_conv_K[sub_net_number][layer_num])
                local_list.append(sub_conv_S[sub_net_number][layer_num])
                local_list.append(sub_flag[sub_net_number][layer_num])
                acc_task_list[acc_num_counter].append(local_list)
            acc_num_counter += 1
            for layer_num in range(pair_list[sub_net_number][0][1][0], pair_list[sub_net_number][0][1][1]):
                local_list = []
                local_list.append(sub_net_number)
                local_list.append(sub_conv_N[sub_net_number][layer_num])
                local_list.append(sub_conv_M[sub_net_number][layer_num])
                local_list.append(sub_conv_r[sub_net_number][layer_num])
                local_list.append(sub_conv_R[sub_net_number][layer_num])
                local_list.append(sub_conv_K[sub_net_number][layer_num])
                local_list.append(sub_conv_S[sub_net_number][layer_num])
                local_list.append(sub_flag[sub_net_number][layer_num])
                acc_task_list[acc_num_counter].append(local_list)
            acc_num_counter += 1
            for layer_num in range(pair_list[sub_net_number][0][1][1], len(sub_conv_N[sub_net_number])):
                local_list = []
                local_list.append(sub_net_number)
                local_list.append(sub_conv_N[sub_net_number][layer_num])
                local_list.append(sub_conv_M[sub_net_number][layer_num])
                local_list.append(sub_conv_r[sub_net_number][layer_num])
                local_list.append(sub_conv_R[sub_net_number][layer_num])
                local_list.append(sub_conv_K[sub_net_number][layer_num])
                local_list.append(sub_conv_S[sub_net_number][layer_num])
                local_list.append(sub_flag[sub_net_number][layer_num])
                acc_task_list[acc_num_counter].append(local_list)
            acc_num_counter += 1
            print("sub net cut into 3")

    return acc_task_list, total_acc_num


def subnet_task_analysis(pair_list, acc_task_list, sub_conv_N, sub_conv_M, sub_conv_r, sub_conv_R, sub_conv_K, sub_conv_S, sub_flag):
    #         i  pair_list[0][0] 1024
    #sub_net, 0, 2,              1024, 4096,2048,2048,2048,2048,2048
    #sub_net, 1, 2,1024,4096,2048,2048,2048,2048,2048
    #sub_net, 2, 2,1024,4096,2048,2048,2048,2048,2048
    subnet_task_list = [[], [], []]
    acc_num = []
    param_num = []
    bias = []
    weight_num = []
    data_in_0 = []
    data_out_0 = []
    data_in_1 = []
    data_out_1 = []
    data_in_2 = []
    data_out_2 = []

    for i in range(0, len(sub_conv_N)):
        sub_w_num = 0
        sub_b_num = 0
        max_i_num = 0
        max_o_num = 0
        for j in range(0, len(sub_conv_N[i])):
            layer_b = sub_conv_M[i][j]
            # sub_b_num += math.ceil(float(layer_b) / 32)
            sub_w_num += math.ceil(float(layer_b) / 32)
            layer_w = sub_conv_N[i][j] * sub_conv_M[i][j] * sub_conv_K[i][j] * sub_conv_K[i][j]
            sub_w_num += math.ceil(float(layer_w)/32)
        data_i = []
        acc_max_i = []
        acc_max_o = []
        for k in range(0, len(acc_task_list)):
            for l in range(0, len(acc_task_list[k])):
                local_max_i = 0
                local_max_o = 0
                if acc_task_list[k][l][0] == i:
                    # print("sub net", i, "acc task list", k, len(acc_task_list[k]), l)
                    i_size = math.ceil(float(acc_task_list[k][l][1] * \
                                             acc_task_list[k][l][3] * acc_task_list[k][l][3])/32)
                    o_size = math.ceil(float(acc_task_list[k][l][2] * \
                                             acc_task_list[k][l][4] * acc_task_list[k][l][4])/32)
                    if acc_task_list[k][l][7]:
                        o_size += math.ceil(float(acc_task_list[k][l][2] * \
                                             acc_task_list[k][l][4] * acc_task_list[k][l][4])/32/4)
                    if local_max_i < i_size:
                        local_max_i = i_size
                    if local_max_o < o_size:
                        local_max_o = o_size
            if acc_task_list[k][l][0] == i:
                data_i.append(local_max_i * 2)  #twiced for double buffering
                data_i.append(local_max_o * 2)
        # print(i, pair_list[i][0][0], 1024, sub_b_num, sub_w_num, data_i)
        subnet_task_list[i].append(i)
        subnet_task_list[i].append(pair_list[i][0][0])
        subnet_task_list[i].append(1024)
        subnet_task_list[i].append(sub_w_num)
        subnet_task_list[i].append(data_i)

    return subnet_task_list


if __name__ == "__main__":
    acc_task_analysis()

