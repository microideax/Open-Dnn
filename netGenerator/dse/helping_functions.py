EOL = "\n"
SEPARATER = "   "
SPACE = " "
PARAMETER_BEGIN = "("
PARAMETER_END = ")"
BODY_BEGIN = "{"
BODY_END = "}"
ARRAY_BEGIN = "["
ARRAY_END = "]"
CLASS_BEGIN = "<"
CLASS_END = ">"
COMMA = ","
COMMA_SPACE = ", "
EOS = ";"
CALL_SYMBOL = "."
FOR = "for"
EQUAL = " = "
INCREMENT = "++"
LESS = " < "


def read_params(file_name):
    arr = []
    with open(file_name) as f:
        lines = f.readlines()
        for line in lines:
            l = line.strip().split(':')
            arr.extend(l)
    return arr


def prompt(s):
    var = raw_input(s)
    return var


def generate_for_loop(counter, counter_type, begin, end, for_body, lc, inc, prefix=SEPARATER):

    for_l = FOR + SPACE + PARAMETER_BEGIN + counter_type + SPACE + counter +  EQUAL + str(begin) +\
           EOS + SPACE + counter + LESS + str(end) + EOS + SPACE + counter
    if inc == 1:
        for_l += INCREMENT
    else:
        for_l += " += " + str(inc)

    for_l += PARAMETER_END + SPACE + BODY_BEGIN + EOL

    for b in for_body:
        for_l +=  prefix*(lc+1) + b + EOL

    for_l += prefix*lc + BODY_END + EOL

    return for_l


def generate_for_loop1(counter, counter_type, begin, end, for_body, prefix=SEPARATER):

    for_l = FOR + SPACE + PARAMETER_BEGIN + counter_type + SPACE + counter +  EQUAL + str(begin) +\
           EOS + SPACE + counter + LESS + str(end) + EOS + SPACE + counter + INCREMENT
    for_l += PARAMETER_END + SPACE + BODY_BEGIN + prefix
    for_l += for_body
    for_l += prefix + BODY_END + EOL
    return for_l


def generate_while(cond, body, k, prefix=SEPARATER):
    w_str = "while "
    w_str += PARAMETER_BEGIN + cond + PARAMETER_END + SPACE + BODY_BEGIN + EOL
    for b in body:
        w_str += prefix*(k+1) + b + EOL
    w_str += prefix*k + BODY_END + EOL*2
    return w_str


def generate_if(condition, body, else_body, k, prefix=SEPARATER):
    if_str =  "if "
    if_str += PARAMETER_BEGIN + condition + PARAMETER_END + SPACE + BODY_BEGIN +\
        EOL
    for b in body:
        if_str +=  prefix*(k+1) + b + EOL
    if_str += prefix*k + BODY_END + EOL

    if else_body != "":
        if_str += prefix*k + "else " + BODY_BEGIN + EOL
        for e_b in else_body:
            if_str += prefix*(k+1) +\
                e_b + EOL
        if_str += prefix*k + BODY_END + EOL

    return if_str


def params_values(s, arr2):
    if s in arr2:
        data = arr2[arr2.index(s)+1]
        values = data.split()
        return values
    else:
        return 0


def extraction(arr):
    arr1 = []
    arr2 = []
    layers_order = params_values("Network Structure", arr)
    nn_in_data_size_conv_values = params_values("nn_in_data_size_conv", arr)
    nn_in_number_conv_values = params_values("nn_in_number_conv", arr)
    nn_group_conv_values = params_values("nn_group_conv", arr)
    nn_channel_size_conv_values = params_values("nn_channel_size_conv", arr)
    nn_out_number_conv_values = params_values("nn_out_number_conv", arr)
    nn_padding_conv_values = params_values("nn_padding_conv", arr)
    nn_stride_conv_values = params_values("nn_stride_conv", arr)
    nn_bias_conv_values = params_values("nn_bias_conv", arr)

    nn_in_data_size_pooling_values = params_values("nn_in_data_size_pooling", arr)
    nn_channel_size_pooling_values = params_values("nn_channel_size_pooling", arr)
    nn_padding_pooling_values = params_values("nn_padding_pooling", arr)
    nn_stride_pooling_values = params_values("nn_stride_pooling", arr)
    nn_in_number_pooling_values = params_values("nn_in_number_pooling", arr)

    nn_in_number_fc_values = params_values("nn_in_number_fc", arr)
    nn_in_data_size_fc_values = params_values("nn_in_data_size_fc", arr)
    nn_channel_size_fc_values = params_values("nn_channel_size_fc", arr)
    nn_out_number_fc_values = params_values("nn_out_number_fc", arr)

    nn_local_size_lrn_values = params_values("nn_local_size_lrn", arr)

    nn_in_number_batch_norm_values = params_values("nn_in_number_batch_norm", arr)
    nn_in_number_scale_values = params_values("nn_in_number_scale", arr)

    nn_in_number_eltwise_values = params_values("nn_in_number_eltwise", arr)
    nn_input_size_eltwise_values = params_values("nn_input_size_eltwise", arr)

    nn_in_number_concat_values = params_values("nn_in_number_concat", arr)
    nn_input_size_concat_values = params_values("nn_input_size_concat", arr)

    conv_cut_flag_values = params_values("conv_cut_flag", arr)
    pool_cut_flag_values = params_values("pool_cut_flag", arr)
    fc_cut_flag_values = params_values("fc_cut_flag", arr)

    arr1.append(layers_order)
    arr2.append("layers_order")

    n = len(nn_in_data_size_conv_values)
    if nn_in_number_fc_values != 0:
        n = n + len(nn_in_number_fc_values)
    arr1.append(str(n))
    arr2.append("n")

    arr1.append(nn_in_number_conv_values)
    arr2.append("nn_in_number_conv")
    arr1.append(nn_in_data_size_conv_values)
    arr2.append("nn_in_data_size_conv")
    arr1.append(nn_channel_size_conv_values)
    arr2.append("nn_channel_size_conv")
    arr1.append(nn_padding_conv_values)
    arr2.append("nn_padding_conv")
    arr1.append(nn_stride_conv_values)
    arr2.append("nn_stride_conv")
    arr1.append(nn_out_number_conv_values)
    arr2.append("nn_out_number_conv")
    arr1.append(nn_group_conv_values)
    arr2.append("nn_group_conv")
    arr1.append(nn_local_size_lrn_values)
    arr2.append("nn_local_size_lrn")
    arr1.append(nn_in_data_size_pooling_values)
    arr2.append("nn_in_data_size_pooling")
    arr1.append(nn_channel_size_pooling_values)
    arr2.append("nn_channel_size_pooling")
    arr1.append(nn_padding_pooling_values)
    arr2.append("nn_padding_pooling")
    arr1.append(nn_stride_pooling_values)
    arr2.append("nn_stride_pooling")
    arr1.append(nn_in_number_pooling_values)
    arr2.append("nn_in_number_pooling")
    arr1.append(nn_in_number_fc_values)
    arr2.append("nn_in_number_fc")
    arr1.append(nn_in_data_size_fc_values)
    arr2.append("nn_in_data_size_fc")
    arr1.append(nn_channel_size_fc_values)
    arr2.append("nn_channel_size_fc")
    arr1.append(nn_out_number_fc_values)
    arr2.append("nn_out_number_fc")
    arr1.append(nn_in_number_batch_norm_values)
    arr2.append("nn_in_number_batch_norm")
    arr1.append(nn_in_number_scale_values)
    arr2.append("nn_in_number_scale")
    arr1.append(nn_in_number_eltwise_values)
    arr2.append("nn_in_number_eltwise")
    arr1.append(nn_input_size_eltwise_values)
    arr2.append("nn_input_size_eltwise")
    arr1.append(nn_in_number_concat_values)
    arr2.append("nn_in_number_concat")
    arr1.append(nn_input_size_concat_values)
    arr2.append("nn_input_size_concat")
    arr1.append(conv_cut_flag_values)
    arr2.append("conv_cut_flag")
    arr1.append(pool_cut_flag_values)
    arr2.append("pool_cut_flag")
    arr1.append(fc_cut_flag_values)
    arr2.append("fc_cut_flag")

    val = str(int(nn_in_number_conv_values[0])) + " * " + \
	      str(int(nn_in_data_size_conv_values[0])) + " * " + \
	      str(int(nn_in_data_size_conv_values[0]))
    arr1.append(val)
    arr2.append("in_data_mem_size")

    val = ""
    for i in range(len(nn_in_number_conv_values)):
        val += str(int(nn_in_number_conv_values[i]) * int(nn_out_number_conv_values[i]) /\
			int(nn_group_conv_values[i])*int(nn_channel_size_conv_values[i])*\
			int(nn_channel_size_conv_values[i]))
        if (i+1) != len(nn_in_number_conv_values):
            val += " + "
    arr1.append(val)
    arr2.append("conv_weight_size")

    val = ""
    if nn_bias_conv_values != 0:
        for i, v in enumerate(nn_bias_conv_values):
            val += v
            if (i+1) != len(nn_bias_conv_values):
                val += " + "
        arr1.append(val)
        arr2.append("conv_bias_size")

    val = ""
    if nn_in_number_fc_values != 0:
        for j in range(len(nn_in_number_fc_values)):
            val += str(int(nn_in_number_fc_values[j])*int(nn_out_number_fc_values[j])*int(nn_channel_size_fc_values[j])*int(nn_channel_size_fc_values[j]))
            if (j+1) != len(nn_in_number_fc_values):
                val += " + "
        arr1.append(val)
        arr2.append("fc_weight_size")

    val = ""
    if nn_out_number_fc_values != 0:
        for i, out in enumerate(nn_out_number_fc_values):
            val += out
            if (i+1) != len(nn_out_number_fc_values):
                val += " + "
        arr1.append(val)
        arr2.append("fc_bias_size")

    val = ""
    if nn_out_number_fc_values != 0:
        arr1.append(str(nn_out_number_fc_values[len(nn_out_number_fc_values)-1]))
        arr2.append("fc_out_size")
    else:
        arr1.append(str(nn_in_number_pooling_values[len(nn_in_number_pooling_values)-1]))
        arr2.append("out_size")

    val = ""
    if nn_in_number_batch_norm_values != 0:
        for i, v in enumerate(nn_in_number_batch_norm_values):
            val += v
            if (i+1) != len(nn_in_number_batch_norm_values):
                val += " + "
        arr1.append(val)
        arr2.append("nn_batch_norm_size")

    val = ""
    if nn_in_number_scale_values != 0:
        for i, v in enumerate(nn_in_number_scale_values):
            val += v
            if (i+1) != len(nn_in_number_scale_values):
                val += " + "
        arr1.append(val)
        arr2.append("nn_scale_size")

    val = ""
    if nn_in_number_eltwise_values != 0:
        arr1.append(val)
        arr2.append("nn_in_number_eltwise_size")

    val = ""
    if nn_input_size_eltwise_values != 0:
        arr1.append(val)
        arr2.append("nn_input_size_eltwise_size")

    val = ""
    if nn_in_number_concat_values != 0:
        arr1.append(val)
        arr2.append("nn_in_number_concat_size")

    val = ""
    if nn_input_size_concat_values != 0:
        arr1.append(val)
        arr2.append("nn_input_size_concat_size")

    maximum = []
    for l in range(len(nn_in_data_size_conv_values)):
        out = (int(nn_in_data_size_conv_values[l]) + int(nn_padding_conv_values[l]) * 2 -\
                      int(nn_channel_size_conv_values[l]))/int(nn_stride_conv_values[l]) + 1;
        val = out * out * int(nn_out_number_conv_values[l])
        maximum.append(val)
    for l1 in range(len(nn_in_data_size_pooling_values)):
        out = (int(nn_in_data_size_pooling_values[l1]) + int(nn_padding_pooling_values[l1])*2 -\
                      int(nn_channel_size_pooling_values[l1]))/int(nn_stride_pooling_values[l1]) + 1
        val = int(out) * int(out) * int(nn_in_number_pooling_values[l1])
        maximum.append(val)
    if nn_in_number_fc_values != 0:
        for l2 in range(len(nn_in_number_fc_values)):
            val = int(nn_in_number_fc_values[l2]) * int(nn_channel_size_fc_values[l2]) *\
			      int(nn_channel_size_fc_values[l2])
            maximum.append(val)
    maxim = max(maximum)
    arr1.append(str(maxim))
    arr2.append("maximum")

    return arr1, arr2


