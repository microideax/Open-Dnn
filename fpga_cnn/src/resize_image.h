
#ifndef _RESIZE_IMAGE_H_
#define _RESIZE_IMAGE_H_

#include <iostream>
template<int channels, int width, int height, int size>
void resize_image(float(&x)[channels][height][width], int h, int w, float(&y)[channels][size][size]) {
	//(1)
	int w0 = w;
	int h0 = h;
	int w1 = size;
	int h1 = size;
	float fw = float(w0) / (w1);
	float fh = float(h0) / (h1);

	float image_max = 0.0;
	float image_min = x[0][0][0];
	for (int i = 0; i < channels; i++) {
		for (int j = 0; j < height; j++) {
			for (int k = 0; k < width; k++) {
				if (x[i][j][k]>image_max)
					image_max = x[i][j][k];
				if (x[i][j][k]<image_min)
					image_min = x[i][j][k];
			}
		}
	}
	
	for (int i = 0; i < channels; i++) {
		for (int j = 0; j < height; j++) {
			for (int k = 0; k < width; k++) {
				x[i][j][k] = (x[i][j][k] - image_min) / (image_max - image_min);
			}
		}
	}

	for (int i = 0; i < channels; i++) {
		for (int j = 0; j < h1; j++) {
			for (int k = 0; k < w1; k++) {
				//(2)
				float x0 = (float)((k + 0.5) * fw - 0.5);
				float y0 = (float)((j + 0.5) * fh - 0.5);

				int x1 = int(x0);
				int x2 = x1 + 1;
				int y1 = int(y0);
				int y2 = y1 + 1;
				//(3)
				float fx1 = x0 - x1;
				float fx2 = 1.0f - fx1;
				float fy1 = y0 - y1;
				float fy2 = 1.0f - fy1;

				float s1 = fx1*fy1;
				float s2 = fx2*fy1;
				float s3 = fx2*fy2;
				float s4 = fx1*fy2;

				y[i][j][k] = (x[i][y2][x2] * s1 + x[i][y2][x1] * s2 + x[i][y1][x1] * s3 + x[i][y1][x2] * s4) 
					* (image_max - image_min) + image_min;
			}
		}
	}

	
	cout << "image_max: " << image_max << endl;
	cout << "image_min: " << image_min << endl;

	//ofstream in_data_crop;
	//in_data_crop.open("in_data_crop.txt", ios::app);
	//for (int i = 0; i < channels; i++) {
	//	//in_data_crop << "channel " << i << "****************" << endl;
	//	for (int j = 0; j < size; j++) {
	//		//in_data_crop << "row " << j << "****************" << endl;
	//		for (int k = 0; k < size; k++) {
	//			in_data_crop << y[i][j][k] << " ";
	//		}
	//		in_data_crop << endl;
	//	}
	//	in_data_crop << endl;
	//}
	//in_data_crop.close();
}

#endif 
