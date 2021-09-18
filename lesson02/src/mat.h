#ifndef CHIMAT_H
#define CHIMAT_H

#include <stdint.h>

// Author: Chiro2001

// 是否使用连续的内存
#define USE_CONTIGROUS_MEM
#define CHIMAT_TYPE double

typedef struct {
  int w;
  int h;
  CHIMAT_TYPE** data;
  CHIMAT_TYPE* content;
} Mat;

Mat* mat_create(int width, int height);
Mat* mat_data_init(Mat* mat);
Mat* mat_data_init_fast(Mat* mat);
void mat_free(Mat* mat);
void mat_free_fast(Mat* mat);
Mat* mat_clone(Mat* mat);
Mat* mat_crop(Mat* mat, int x1, int y1, int x2, int y2);
Mat* mat_padding_around(Mat* mat, int padding);
Mat* mat_conv(Mat* mat, double kernel[3][3]);
Mat* mat_mul(Mat *a, Mat *b, Mat *c_source);

#endif