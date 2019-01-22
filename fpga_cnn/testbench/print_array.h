//
// Created by yaochen on 22/10/18.
//

#ifndef _PRINT_ARRAY_H_
#define _PRINT_ARRAY_H_

#include <string>
#include "config.h"

using namespace std;

int print_array_3d(string array_name, int channel, int r_dim, int c_dim, data_type_itf *array){

    ap_fixed<16,10> print_tmp = 0;
    cout << array_name << endl;

    for (int ch = 0; ch < channel; ch++) {
        cout << "print output channel: " << ch << endl;
        for (int j = 0; j < r_dim; j++) {
            for (int i = 0; i < c_dim; i++) {
                print_tmp.range(15,0) = array[j*r_dim + i].range(ch*16+15, ch*16);
#if _HLS_MODE_
                cout << print_tmp << " ";
#else
                cout << print_tmp << " ";
#endif
            }
            cout << endl;
        }
        cout << endl;
    }
    cout << endl;
    cout << endl;

    return 0;
}

int squeeze_input(string array_name,
                  int channel,
                  int r_dim,
                  int c_dim,
                  data_type_o *i_array,
                  data_type_itf *o_array,
                  bool dis_enable) {

    ap_fixed<16,10> print_tmp = 0;
    cout << array_name << endl;

    for (int ch = 0; ch < channel; ch++) {
        for (int i = 0; i < r_dim; i++) {
            for (int j = 0; j < c_dim; j++) {
                for (int wd = 0; wd <= ch && wd < 32; wd++) {

                    o_array[i * r_dim + j].range((wd + 1) * 16 - 1, wd * 16) = i_array[i * r_dim + j].range(15, 0);
#if _C_DEBUG_MODE_
                    if (dis_enable) {cout << setw(3) << i_array[i * 28 + j] << " ";}
#else
                    if (dis_enable) {cout << i_array[i * 28 + j] << " ";}
#endif
                }
            }
            if (dis_enable) {cout << endl;}
        }
        if (dis_enable) {cout << endl;}
    }
    if (dis_enable) {cout << endl;}

    return 0;
}

int squeeze_weight(string array_name,
                   int i_channel,
                   int o_channel,
                   int kernel_size,
                   data_type_w *i_data,
                   data_type_itf *o_array,
                   bool dis_enable) {

    ap_fixed<16,10> print_tmp = 0;
    cout << array_name << endl;

    if (dis_enable) {
        cout << "Printing squeezed weight data ----------------------------- " << endl;
    }

    for (int i = 0; i < i_channel; i++){
        for (int j = 0; j < o_channel; j++) {
            for (int k1 = 0; k1 < kernel_size; k1++) {
                for (int k2 = 0; k2 < kernel_size; k2++) {
                    data_type_w w = *(i_data + i * o_channel * kernel_size * kernel_size +
                                      j * kernel_size * kernel_size + k1 * kernel_size + k2);
//                    for(int ch = 0; ch < 32 && ch < i_channel; ch++){
//                        ap_fixed<16,10> w =
//                                i_data[ch*o_channel*kernel_size*kernel_size + j*kernel_size*kernel_size + k1*kernel_size + k2];
                    o_array[i / 32 * o_channel * kernel_size * kernel_size + j * kernel_size * kernel_size +
                            k1 * kernel_size + k2].range(i * 16 + 15, i * 16) = w.range(15, 0);
                    if (dis_enable) { cout << w << " "; }
                }
            }if (dis_enable) { cout << endl; }
        }if (dis_enable) {cout << endl;}
    }
    if (dis_enable) {cout << endl;}

    if (dis_enable) {
        cout << "Finished printing squeezed weight data ----------------------------- " << endl;
    }

    return 0;

}


#endif //_PRINT_ARRAY_H_
