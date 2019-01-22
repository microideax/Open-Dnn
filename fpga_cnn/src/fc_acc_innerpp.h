#ifndef _FC_ACC_H_
#define _FC_ACC_H_

#include <iostream>
#include <fstream>
#include "config.h"
#include "activation_functions.h"

#if _C_DEBUG_MODE_
#include <algorithm>
#endif

using namespace std;

template <typename Itf, typename Tparam, typename T, typename W, typename G, int iTm, int iTn, int iS_max, int iK_max>
class fc_acc
{

  private:
    int fc_layer_number;

  public:
    fc_acc() : fc_layer_number(0) { // construction function with parameter checking
    	if (iTm < 32 || iTn < 32){
    		if(iTm < 32) cout << "FC ACC: iTm is invalid, please check the iTm value to make sure it is >= 32 !!!" << endl;
    		if(iTn < 32) cout << "FC ACC: iTn is invalid, please check the iTm value to make sure it is >= 32 !!!" << endl;
    	} else {
    		cout << "FC ACC: fc_acc is valid!" << endl;
    	}
    }

    ////------------------------------C++ debugging functions---------------------------------------////


    ////-----------------------------Accelerator Functions---------------------------------------////
    // Load bias data
    void b_buf_load_512(W buf[], Itf *fc_layer_bias, int fc_b_offset, int m, int M) {
    	Itf data_temp = 0;
        for (int i = 0; i < iTm; i+=32){
#pragma HLS PIPELINE
        	data_temp = *(fc_layer_bias + fc_b_offset + (m+i)/32);
        	cout << "index of bias memory : " << fc_b_offset + (m+i)/32 << endl;
        	for (int wr = 0; wr < 32; wr++){
#pragma HLS UNROLL
        		if (i+wr < iTm) {
        			if (i+wr < M){
        				buf[i+wr].range(15,0) = data_temp.range((wr+1)*16-1, wr*16);
//        				cout << "bias buffer[" << i + wr <<"] = "<< buf[i+wr] << endl;
        			} else {
        				buf[i+wr].range(15,0) = 0;
        			}
        		}
        	}
        }
    }

    // Load input data
    void in_buf_load_512(T buf[iTn],
                     Itf *fc_in_data,
                     int fc_i_offset, int n, int N) {
    	Itf data_temp = 0;
        for (int i = 0; i < iTn; i+=32) {
#pragma HLS PIPELINE
        	data_temp = *(fc_in_data + fc_i_offset + (n+i)/32);
//        	cout << "index of in data memory : " << fc_i_offset + (n+i)/32 << endl;
            for (int wr = 0; wr < 32; wr++) {
#pragma HLS UNROLL
            	if(i + wr < iTn){
            		if(i+wr < N ){
            			buf[i+wr].range(15,0) = data_temp.range((wr+1)*16-1, wr*16);
//            			cout << "data_buffer["<<i+wr <<"] = "<< buf[i+wr] << endl;
            		} else {
            			buf[i+wr].range(15,0) = 0;
            		}
            	}
            }
        }
    }

    // Load weights to weight buffer
    void w_buf_load_512(W buf[iTn],
                     Itf *fc_in_weight,
                     int fc_w_offset, int n, int N) {
    	Itf data_temp = 0;
        int index = 0;
        for (int i = 0; i < iTn; i+=32) {
#pragma HLS PIPELINE
        	data_temp = *(fc_in_weight + fc_w_offset + (n+i)/32);
//        	cout << "index of weight memory : " << fc_w_offset + (n+i)/32 << endl;
            for (int wr = 0; wr < 32; wr++) {
#pragma HLS UNROLL
                index = i + wr;
            	if(i + wr < iTn){
            		if(i+wr < N){
            			buf[index].range(15,0) = data_temp.range((wr+1)*16-1, wr*16);
//            			cout << "weight_buffer[" << i + wr << "] = "<< buf[index] << endl;
            		} else {
            			buf[index].range(15,0) = 0;
            		}
            	}
            }
        }
    }

    // fc computation engine
    void fc_engine(T in_buf[], W w_buf[], W b_buf[], G out_buf[], int n) {
//    	cout << "Calling fc_engine !!!" << endl;
#pragma HLS PIPELINE
            for (int tm = 0; tm < iTm; tm++) {
#pragma HLS UNROLL
            	for (int tn = 0; tn < iTn; tn++) {
#pragma HLS UNROLL
//            		cout << "fc_engine core processing !" << endl;
            		if (tn == 0 && n == 0){
            			out_buf[tm] = b_buf[tm] + w_buf[tn]*in_buf[tn];
//            			cout <<"tn, n = " << tn << "," << n << "out_buf = "<< out_buf[tm] <<" " << w_buf[tn] << " * " << in_buf[tn] << endl;
            		} else {
                        out_buf[tm] = out_buf[tm] + w_buf[tn] * in_buf[tn];
//                        cout << "out_buf = "<< out_buf[tm] <<" " << w_buf[tn] << " * " << in_buf[tn] << endl;
                    }
            	}
            }
    	}

    // fc output engine output_res(out_buf_0, fc_o_data, fc_o_offset, n, m, N, M, act);
    void output_res_512(G buf[], Itf* fc_o_data, int fc_o_offset,
    		int n, int m, int N, int M, bool act){
        Itf out_tmp = 0;
        G   tmp = 0;
        G   tmp_outbuf = 0;
        if (n >= N - iTn) {
           	for (int i = 0; i < iTm && i < M; i += 32) { // iTm should always greater than 32, otherwise this will not work
           		for (int wr = 0; wr < 32; wr++){
#pragma HLS PIPELINE
                    if(i + wr < M && i + wr < iTm) {
//                    	tmp_outbuf = RELU(buf[i + wr]);
                    	tmp_outbuf = buf[i + wr];
                        tmp.range(15, 0) = tmp_outbuf.range(15, 0);
                    } else {
                        tmp.range(15,0) = 0;
                    }
                    out_tmp.range(16 * (wr + 1) - 1, 16 * (wr)) = tmp.range(15,0);
//                    cout << "out_buffer[" << wr << "] = " << buf[wr] << endl;
                }
                *(fc_o_data + fc_o_offset + (m + i)/32) = out_tmp;
//                cout << "index of out memory : " << fc_o_offset + ((m+i)/32) << endl;
           	}
        }
	}


#if _LAYER_MODE_ // layer function with manual double buffer -- worked
    void fc_layer_acc_mbuf(
        int N,               //input feature number
        int M,               // output feature number
        int R_IN,
        int C_IN,
        int K,
        bool act,            // activation function bit (1-- with act, 0--without act)
        Itf *fc_layer_weights, //w[M][N][K][K]
        Itf *fc_layer_bias,    // b[M]
        int fc_w_offset,
        int fc_b_offset,
        int fc_i_offset,
        int fc_o_offset,
        Itf *fc_i_data, // in_data[N][(R-1)*S + K][(C-1)*S + K] --> [N][(R-1)*S + K - 2*P][(C-1)*S + K - 2*P]
        Itf *fc_o_data)
    { // out[M][R][C]

        /***************local data buffer groups******************************/
        T in_buf_0[iTn];
        W w_buf_0[iTn];
        W b_buf_0[iTm];
        G out_buf_0[iTm];

//        T in_buf_1[iTn];
//        W w_buf_1[iTn];
//        W b_buf_1[iTm];
//        G out_buf_1[iTm];

#if _HLS_MODE_
#pragma HLS ARRAY_PARTITION variable = in_buf_0 complete dim = 1
#pragma HLS ARRAY_PARTITION variable = w_buf_0 complete dim = 1
#pragma HLS ARRAY_PARTITION variable = b_buf_0 complete dim = 1
#pragma HLS ARRAY_PARTITION variable = out_buf_0 complete dim = 1
#endif

#if _C_DEBUG_MODE_
#if _KERNEL_DEBUG_
        cout << "Starting fc acc manual double buffer test ...." << endl;
        out_buf_reset(out_buf_0);
        b_buf_reset(b_buf_0);
        w_buf_reset(w_buf_0);
        cout << "Local buffer reset finised ..." << endl;
#endif
#endif

#pragma HLS DATAFLOW

        for (int m = 0; m < M; m += iTm)
        {

            for (int n = 0; n < N; n += iTn)
            {
//--------------------------Load input B W D in ping-pong manner-------------------------//
                b_buf_load_512(b_buf_0, fc_layer_bias, fc_b_offset, m, M);
                w_buf_load_512(w_buf_0, fc_layer_weights, fc_w_offset, n, N);
                in_buf_load_512(in_buf_0, fc_i_data, fc_i_offset, n, N);
//------------------------------compute buffered data -----------------------------------//
                fc_engine(in_buf_0, w_buf_0, b_buf_0, out_buf_0, n);
//---------------------------transfer output data----------------------------------------//
                output_res_512(out_buf_0, fc_o_data, fc_o_offset, n, m, N, M, act);
//                output_res_512(b_buf_0, fc_o_data, fc_o_offset, n, m, N, M, act);
            }
        }
    };
#endif
};

#endif
