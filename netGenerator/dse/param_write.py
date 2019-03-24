def conv_param_write(conv_param_list, store_file):

    with open(store_file, "w") as wf:

        for i in range(0, len(conv_param_list)):
            for j in range(0, len(conv_param_list[i][1])):
                conv_param = "conv, data_type_itf, Tparam, data_type, data_type_w, data_type_o, " \
                    + str(conv_param_list[i][1][j][0]) + ", " \
                    + str(conv_param_list[i][1][j][1]) + ", " \
                    + str(conv_param_list[i][1][j][2]) + ", " \
                    + str(conv_param_list[i][1][j][2]) + ", 5, 5, 32, 32, 32"
                wf.write(conv_param + "\n")
    wf.close()


def pool_param_write(pool_param_list, store_file):

    with open(store_file, "a+") as wf:
        # for i in range(0, len(parameters)):
        #     for j in range(0, len(parameters[i][1])):
        #         pool_param = "max_pool, data_type_itf, Tparam, data_type, data_type_w, data_type_o, " \
        #             + str(parameters[i][1][j][0]) + ", " \
        #             + str(parameters[i][1][j][1]) + ", " \
        #             + str(parameters[i][1][j][2]) + ", 2, 3"
        #         wf.write(pool_param + "\n")
        for i in range(0, len(pool_param_list)):
            pool_param = "max_pool, data_type_itf, Tparam, data_type, data_type_w, data_type_o, 32, 16, 16, 2, 3"
            wf.write(pool_param + "\n")
    wf.close()


def layer_acc_param_write(layer_acc_list, store_file):
    with open(store_file, "a+") as wf:
        conv_core_counter = 0
        pool_core_counter = 0
        for i in range(0, len(layer_acc_list)):
            pool_flag = False
            for j in range(0, len(layer_acc_list[i])):
                print(layer_acc_list[i])
                print(layer_acc_list[i][j])
                print(layer_acc_list[i][j][-1])
                if layer_acc_list[i][j][-1] == True:
                    pool_flag = True
                    print("Found layer with pooling")
                # else:
                #     pool_flag = False
            if pool_flag == True:
                layer_acc = "conv_pool, " + str(conv_core_counter) + "," + str(conv_core_counter) + "," + str(
                    pool_core_counter) + "\n"
                conv_core_counter += 1
                pool_core_counter += 1
            else:
                layer_acc = "conv_pool, " + str(conv_core_counter) + "," + str(conv_core_counter) + "\n"
                conv_core_counter += 1
            wf.write(layer_acc)


#TODO: write the sub_net function parameters into file
def sub_param_write(subn_param_list, store_file):

    with open(store_file, "a+") as wf:
        # for i in range(0, len(parameters)):
        #     for j in range(0, len(parameters[i][1])):
        #         pool_param = "max_pool, data_type_itf, Tparam, data_type, data_type_w, data_type_o, " \
        #             + str(parameters[i][1][j][0]) + ", " \
        #             + str(parameters[i][1][j][1]) + ", " \
        #             + str(parameters[i][1][j][2]) + ", 2, 3"
        #         wf.write(pool_param + "\n")
        for i in range(0, len(subn_param_list)):
            subn_param = "sub_net, "+str(i)+", 2,1024,4096,2048,2048,2048,2048,2048"
            wf.write(subn_param + "\n")
    wf.close()


def generate_param_file(conv_param_list, pool_param_list, layer_acc_list, store_file):

    # with open(store_file, "w") as wf:

        # write the conv parameters
        # for i in range(0, len(parameters)):
        #     for j in range(0, len(parameters[i][1])):
    conv_param_write(conv_param_list, store_file)
    pool_param_write(pool_param_list, store_file)
    layer_acc_param_write(layer_acc_list, store_file)
    sub_param_write(pool_param_list, store_file)


        # write the pooling parameters

        # wirte conv_pool function parameters


if __name__ == "__main__":
    generate_param_file(conv_param_list, pool_param_list, layer_acc_list, "acc_ins_params.txt")
    # conv_param_write(parameters)