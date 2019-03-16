def conv_param_write(parameters, store_file):

    with open(store_file, "w") as wf:

        for i in range(0, len(parameters)):
            for j in range(0, len(parameters[i][1])):
                conv_param = "conv, data_type_itf, Tparam, data_type, data_type_w, data_type_o, " \
                    + str(parameters[i][1][j][0]) + ", " \
                    + str(parameters[i][1][j][1]) + ", " \
                    + str(parameters[i][1][j][2]) + ", " \
                    + str(parameters[i][1][j][2]) + ", 5, 5, 32, 32, 32"
                wf.write(conv_param + "\n")


def pool_param_write(parameters, store_file):

    with open(store_file, "w+") as wf:
        for i in range(0, len(parameters)):
            for j in range(0, len(parameters[i][1])):
                pool_param = "max_pool, data_type_itf, Tparam, data_type, data_type_w, data_type_o, " \
                    + str(parameters[i][1][j][0]) + ", " \
                    + str(parameters[i][1][j][1]) + ", " \
                    + str(parameters[i][1][j][2]) + ", 2, 3"
                wf.write(pool_param + "\n")


def generate_param_file(parameters, store_file):

    # with open(store_file, "w") as wf:

        # write the conv parameters
        # for i in range(0, len(parameters)):
        #     for j in range(0, len(parameters[i][1])):
    conv_param_write(parameters, store_file)


        # write the pooling parameters

        # wirte conv_pool function parameters


if __name__ == "__main__":
    generate_param_file(parameters, "acc_ins_params.txt")
    # conv_param_write(parameters)