// Baseline data type define for the entire design
// TODO:

#ifndef _DATA_TYPE_H_
#define _DATA_TYPE_H_

#include<vector>
//#include<string>
//#include <ap_fixed.h>

using namespace std;

typedef unsigned int uint;
typedef uint cnn_size_t;
/*
typedef ap_fixed<64, 32> cnn_data_64;
typedef ap_fixed<32, 16> cnn_data_32;
typedef ap_fixed<16, 8>  cnn_data_16;
typedef ap_fixed<8, 4>   cnn_data_8;
typedef ap_fixed<4, 2>   cnn_data_4;
*/
typedef std::vector<float, std::allocator<float> > std_vec_t;
typedef std::vector<std_vec_t> std_tensor_t;
typedef std::vector<std_tensor_t> std_tensor_t_3d;


#endif
