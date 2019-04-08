#ifndef _MAX_POOL_ACC_H_
#define _MAX_POOL_ACC_H_

#include <iostream>
#include <fstream>
#include "activation_functions.h"
using namespace std;

template <typename Itf, typename Tparam, typename pT, typename pW, typename pG, int pTn, int pTr, int pTc, int pS_max, int pK_max>
class max_pool_acc {

private:
	int pool_layer_number;

public:
	max_pool_acc() : pool_layer_number(0) {pool_layer_number = 0;};

    // Tn << 32 && N << 32
    void in_buf_load_512(
            ap_fixed<16,10> buf[][(pTr - 1) * pS_max + pK_max][(pTc - 1) * pS_max + pK_max],
            ap_int<512>* i_data,
            int in_offset,
            int n, int r, int c, int S, int K, int P,
            int R, int C, int N, int R_IN, int C_IN, int TR, int TC)
    {
    	ap_int<512> data_tmp = 0;
        // valid data portion
        for (int j = r * S - P; j < r * S + TR - P; j++)
        {
            for (int k = c * S - P; k < c * S + TC - P; k++)
            {
#pragma HLS PIPELINE
                for (int i = 0; i < pTn; i+=32)
                {
#pragma HLS UNROLL
                    if ((i + n >= N) || j < 0 || j >= (R_IN - 2 * P) || k < 0 || k >= (C_IN - 2 * P))
                    {
                        for (int wr = 0; wr < pTn; wr++)
                        {
#pragma HLS UNROLL
                            buf[i + wr][j - r * S + P][k - c * S + P] = pT(0);
                        }
                    }
                    else
                    {
                        data_tmp = *(i_data + in_offset + (i + n)/32 * (R_IN - 2 * P) * (C_IN - 2 * P) + j * (R_IN - 2 * P) + k);
                        for (int wr = 0; wr < pTn; wr++)
                        {
#pragma HLS UNROLL
                            buf[wr][j - r * S + P][k - c * S + P].range(15,0) = data_tmp.range((wr + 1) * 16 - 1, (wr) * 16);
                        }
                    }
                }
            }
        }
    }

    // Max pooling computation kernel
    void pool_engine(pT in_buf[][(pTr-1)*pS_max + pK_max][(pTc-1)*pS_max + pK_max],
                     pG out_buf[][pTr][pTc],
                     int S, int n, int r, int c, int K, int R, int C, int TR, int TC) {
        if (n >= 0) {
            for (int i = 0; i < K; i++) {
                for (int j = 0; j < K; j++) {
                    for (int tr = 0; tr < pTr && tr + r < R && (S * tr + i) < TR; tr++) {
                        for (int tc = 0; tc < pTc && tc + c < C && (S * tc + j) < TC; tc++) {
#pragma HLS PIPELINE
                            for (int tn = 0; tn < pTn; tn++) {
#pragma HLS UNROLL
                                out_buf[tn][tr][tc] = (i == 0 && j == 0) ? in_buf[tn][S * tr][S * tc]
                                                                         : ((out_buf[tn][tr][tc]
                                                                         > in_buf[tn][S * tr + i][S * tc + j])
                                                                         ? out_buf[tn][tr][tc]
                                                                         : in_buf[tn][S * tr + i][S * tc + j]);
                            }
                        }
                    }
                }
            }
        }
    }

    // Ouput out_buf data to output interface
    void output_res_512(pG out_buf[][pTr][pTc],
                        Itf *out_data,
                        int out_offset,
                        int n, int r, int c, int N, int R, int C, bool act) {

        Itf out_tmp = 0;
        Itf ex_out_tmp = 0;
        pG tmp = 0;
        pG tmp_outbuf = 0;
        if (n >= 0) {
            for (int j = r; j < r + pTr && j < R; j++)
            {
                for (int k = c; k < c + pTc && k < C; k++)
                {
#pragma HLS PIPELINE
                    for (int i = 0; i < pTn; i += 32)
                    {
                    	if(i < N - n)
                    	{
                    		for (int wr = 0; wr < (pTn<32?pTn:32); wr++)
                    		{
#pragma HLS UNROLL
                    			tmp_outbuf = RELU(out_buf[i + wr][j - r][k - c]);
                    			out_tmp.range(16 * (wr + 1) - 1, 16 * wr) = tmp_outbuf.range(15,0);
                    		}
                    		*(out_data + out_offset + ((i + n)/32)*R*C + j*C + k) = out_tmp;
                    	}
                    }
                }
            }
        }
    }
///////////////////////------------------conv accelerator----------------//////////////////////////
	void max_pool_layer_mbuf(
            int R_IN,    // input Row
            int C_IN,    // input column
            int N,       //input feature number
            int K,       //input kernel size
            int R,       // output Row
            int C,       // output column
            int S,       // stride size
            int P,       // padding size
            int act,     // activation function bit (1-- with act, 0--without act)
            int i_offset,
            int o_offset,
            Itf *i_data,
            Itf *o_data)
	{

        int TR=0;
        int TC=0;
        ap_fixed<16,10> in_buf_0[pTn][(pTr-1)*pS_max + pK_max][(pTc-1)*pS_max + pK_max];
        ap_fixed<16,10> out_buf_0[pTn][pTr][pTc];

#pragma HLS ARRAY_PARTITION variable=in_buf_0 complete dim=1
#pragma HLS ARRAY_PARTITION variable=out_buf_0 complete dim=1



        for(int r = 0; r < R; r += pTr)
        {
            for (int c = 0; c < C; c += pTc)
            {
                TR = ((r * S + (pTr - 1) * S + K) > R_IN ? (R_IN - r * S) : ((pTr - 1) * S + K));
                TC = ((c * S + (pTc - 1) * S + K) > C_IN ? (C_IN - c * S) : ((pTc - 1) * S + K));
                for (int n = 0; n < N ; n += pTn)
                {
                    in_buf_load_512(in_buf_0, i_data, i_offset, n, r, c, S, K, P, R, C, N, R_IN, C_IN, TR, TC);
                    pool_engine(in_buf_0, out_buf_0, S, n, r, c, K, R, C, TR, TC);
                    output_res_512(out_buf_0, o_data, o_offset, n, r, c, N, R, C, act);
                }
            }
        }
    }
};
#endif
