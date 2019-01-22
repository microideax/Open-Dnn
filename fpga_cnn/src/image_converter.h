//Move all the image pre-processing functions here to simplify the main function.
//re-construct the array data of images into 3d
//TODO: Move all the image pre-processing functions into this header.

#ifndef _CONVERTE_IMAGE_H_
#define _CONVERTE_IMAGE_H_

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iterator>
#include "data_type.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include "stb_image/stb_image_resize.h"
#include "stb_image/stb_image_write.h"

using namespace std;

void convert_image(const std::string& imagefilename,
                   double minv,
                   double maxv,
                   int w,
                   int h,
                   std_vec_t& data) {
    // load
    int input_w, input_h, comp;
    stbi_uc* input_pixels = stbi_load(imagefilename.c_str(), &input_w, &input_h, &comp, 1);

    if (!input_pixels) {
        // cannot open, or it's not an image
        cout << "stbi_load failed";
        return;
    }

    // resize
    std::vector<uint8_t> resized(w * h);
    uint8_t* resized_pixels = &(resized[0]);
    int input_stride_in_bytes = input_w;
    if (!stbir_resize_uint8(input_pixels, input_w, input_h, input_stride_in_bytes, resized_pixels, w, h, w, 1)) {
        cout << "stbir_resize_uint8 failed";
        stbi_image_free(input_pixels);
        return;
    }
    stbi_image_free(input_pixels);
    // mnist dataset is "white on black", so negate required

//    std::transform(resized.begin(), resized.end(), std::back_inserter(data),
//                   [=](uint8_t c) { return (255 - c) * (maxv - minv) / 255.0 + minv; });


    for (unsigned int i = 0; i < resized.size(); i++){
    data.push_back((255 - resized[i]) * (maxv - minv) / 255.0 + minv);
    }

    cout << data.size() << endl;

}


std_tensor_t in_2_3D(std_vec_t& data_in) {
    std_tensor_t data_out;
	data_out.push_back(data_in);
	return data_out;
}

//re-construct input array to 2d matrix
std_tensor_t_3d in_2_2D_conv(const int input_size, std_tensor_t in) {
	std_vec_t in_data; //original input data
	std_vec_t vec1;    //input row vector
    std_tensor_t vec2;
	std_tensor_t_3d in_data2D;
	for (uint j = 0; j < in.size(); j++) {
		in_data = in[j];
		for (uint i = 0; i < in_data.size(); i++)//
		{
			if (in_data.size() < uint (input_size * input_size)) {//
				if (i == 0 || (i - 1) / input_size == i / input_size) {//
					vec1.push_back(in_data[i]);//
					if (i == in_data.size() - 1) {//
						vec2.push_back(vec1);//
						vec1.clear();
					}
					if (i != 0 && i % (input_size*input_size) == 0) {
						in_data2D.push_back(vec2);
						vec2.clear();
					}
				}
				else {
					vec2.push_back(vec1);
					vec1.clear();
					vec1.push_back(in_data[i]);//
					if (i != 0 && i % (input_size*input_size) == 0) {
						in_data2D.push_back(vec2);
						vec2.clear();
					}
				}
			}
			else {//只有一个输入
				if (i == 0 || (i - 1) / input_size == i / input_size) {//将原本的一维输入按图片大小转换成二维
					vec1.push_back(in_data[i]);//放入输入map每个像素的输入值构成输入行向量
					if (i == in_data.size() - 1) {//最后一个输入行向量
						vec2.push_back(vec1);//放入输入行向量构成二维输入tensor
						vec1.clear();
						in_data2D.push_back(vec2);
						vec2.clear();
					}
				}
				else {
					vec2.push_back(vec1);
					vec1.clear();
					vec1.push_back(in_data[i]);//放入输入行向量第一个元素
				}
			}
		}
	}
	return in_data2D;
}


// convert std_tensor_3d to static_tensor_3d
// the output of image_converter should be static vector/tensor/tensor_3d
//void std_2_static(tensor_t_3d& in_data2D, std_tensor_t_3d& in_data2D_temp) {
//	for (uint i = 0; i < in_data2D_temp.size(); i++) {
//		tensor_t tmp;
//		in_data2D.push_back(tmp);
//		for (uint j = 0; j < in_data2D_temp[i].size(); j++) {
//			vec_t tmp1;
//			in_data2D[i].push_back(tmp1);
//			for (uint k = 0; k < in_data2D_temp[i][j].size(); k++) {
//				in_data2D[i][j].push_back(in_data2D_temp[i][j][k]);
//			}
//		}
//	}
//}

/*
bool save_image(const std::string& imagefilename, const image<>& img) {
// no scaling, save at original size
int stride_bytes = img.width();
int ret = stbi_write_png( imagefilename.c_str(), img.width(), img.height(), 1, &(img.at(0, 0)), stride_bytes);
return (ret != 0);
}
*/

#endif
