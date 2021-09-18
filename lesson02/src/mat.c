#include "mat.h"

#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Author: Chiro2001
// 代码来自大一上的 C 语言实验课的作品

// 标注是否使用快速crop模式
// #define USE_FAST_CROP
// 但是现在发现基于指针的fast模式比不用更加慢
// malloc牛逼

// @prog: 创建图像矩阵
// @args: 图像宽, 高
// @rets: 矩阵指针
Mat* mat_create(int width, int height) {
  Mat* mat = NULL;
  mat = malloc(sizeof(Mat));
  memset(mat, sizeof(Mat), 0);
  assert(mat);
  mat->w = width;
  mat->h = height;
  return mat;
}

// @prog: 初始化矩阵的数据
// @args: 矩阵指针
// @rets: 矩阵指针
Mat* mat_data_init(Mat* mat) {
  if (!mat) {
    return NULL;
  }
#ifdef USE_CONTIGROUS_MEM
  // 直接申请一整块空间罢
  mat->content = malloc(sizeof(CHIMAT_TYPE) * (mat->w * mat->h));
  assert(mat->content);
  memset(mat->content, sizeof(CHIMAT_TYPE) * (mat->h * mat->w), 0);
  // 然后做个索引
  mat->data = malloc(sizeof(CHIMAT_TYPE*) * mat->w);
  assert(mat->data);
  for (int i = 0; i < mat->h; i++)
    mat->data[i] = mat->content + i * mat->w;
  return mat;
#else
  mat->content = NULL;
  mat->data = malloc(sizeof(CHIMAT_TYPE*) * mat->w);
  assert(mat->data);
  for (int i = 0; i < mat->w; i++) {
    mat->data[i] = malloc(sizeof(CHIMAT_TYPE) * mat->h);
    assert(mat->data[i]);
    memset(mat->data[i], sizeof(CHIMAT_TYPE) * mat->h, 0);
  }
  return mat;
#endif
}

// @prog: 快速初始化矩阵的数据(只初始化一个维度，创建矩阵的引用)
// @args: 矩阵指针
// @rets: 矩阵指针
Mat* mat_data_init_fast(Mat* mat) {
  mat->data = malloc(sizeof(CHIMAT_TYPE*) * mat->w);
  assert(mat->data);
  memset(mat->data, sizeof(CHIMAT_TYPE*) * mat->w, 0);
  return mat;
}

// @prog: 释放矩阵的内存
// @args: 矩阵指针
// @rets: void
void mat_free(Mat* mat) {
  for (int i = 0; i < mat->w; i++) {
    free(mat->data[i]);
  }
  free(mat->data);
  free(mat);
}

// @prog: 释放被快速初始化的矩阵的数据
// @args: 矩阵指针
// @rets: void
void mat_free_fast(Mat* mat) {
  free(mat->data);
  free(mat);
}

// @prog: 深度拷贝一个矩阵
// @args: 矩阵指针
// @rets: 新的矩阵指针
Mat* mat_clone(Mat* mat) {
  Mat* dist = mat_create(mat->w, mat->h);
  mat_data_init(dist);
  for (int i = 0; i < mat->w; i++)
    for (int j = 0; j < mat->h; j++) dist->data[i][j] = mat->data[i][j];
  return dist;
}

// @prog: 从矩阵中选区建立新的矩阵
// @args: 矩阵指针, Rect(x1, y1, x2, y2)
// @rets: 矩阵指针
Mat* mat_crop(Mat* mat, int x1, int y1, int x2, int y2) {
  if (x1 > x2 || y1 > y2 || !mat) return NULL;
  int w = x2 - x1, h = y2 - y1;
  Mat* dist = mat_create(w, h);
#ifndef USE_FAST_CROP
  mat_data_init(dist);
  for (int i = 0; i < w; i++)
    for (int j = 0; j < h; j++) dist->data[i][j] = mat->data[i + x1][j + y1];
#else
  mat_data_init_fast(dist);
  for (int i = 0; i < w; i++) dist->data[i] = mat->data[i + x1] + y1;
#endif  // USE_FAST_CROP
  // 打印调试信息会使程序运行效率下降(惨不忍睹)
  // printf("\tcrop: (%d, %d, %d, %d)\n", x1, y1, x2, y2);
  return dist;
}

// @prog: 矩阵外扩展0，生成新的矩阵
// @args: 矩阵指针, 扩展像素宽度
// @rets: 矩阵指针
Mat* mat_padding_around(Mat* mat, int padding) {
  if (!mat) return NULL;
  int w = mat->w + padding * 2, h = mat->h + padding * 2;
  Mat* dist = mat_create(w, h);
  mat_data_init(dist);
  for (int i = 0; i < mat->w; i++)
    for (int j = 0; j < mat->h; j++)
      dist->data[i + padding][j + padding] = mat->data[i][j];
  return dist;
}

// @prog: 计算矩阵(3x3大小)卷积
// @args: 矩阵指针, 卷积核
// @rets: 像素值
CHIMAT_TYPE mat_kernel_calc(Mat* mat, double kernel[3][3]) {
  CHIMAT_TYPE c = 0.;
  if (mat->w != 3 || mat->h != 3) {
    printf("#ERR");
    return c;
  }
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++) {
      c += (uint8_t)(kernel[i][j] * mat->data[i][j]);
    }
  return c;
}

// @prog: 对矩阵卷积操作
// @args: 矩阵指针, 卷积核
// @rets: 新的矩阵指针
Mat* mat_conv(Mat* mat, double kernel[3][3]) {
  // 先扩大边界
  Mat* pad = mat_padding_around(mat, 1);
  Mat* dist = mat_clone(mat);
  for (int i = 0; i < mat->w; i++) {
    for (int j = 0; j < mat->h; j++) {
      Mat* crop = mat_crop(pad, i, j, i + 3, j + 3);
      dist->data[i][j] = mat_kernel_calc(crop, kernel);
#ifndef USE_FAST_CROP
      mat_free(crop);
#else
      mat_free_fast(crop);
#endif  // USE_FAST_CROP
        // printf("#1.3 (%d, %d) %d %d %d\n", i, j,
        // dist->data[i][j].red,dist->data[i][j].green, dist->data[i][j].blue);
    }
  }
  return dist;
}

// @prog: 执行矩阵乘法，返回一个新的矩阵作为运算结果，或者直接在目的矩阵操作
// @args: a * b -> c
// @rets: c
Mat* mat_mul(Mat *a, Mat *b, Mat *c_source) {
  // 检查是否合法
  if (a->w != b->h) {
    return NULL;
  }
  // 可以在外部提供源
  Mat *c = NULL;
  if (c_source != NULL) {
    c = c_source;
  } else {
    c = mat_create(a->h, b->w);
    mat_data_init(c);
  }
  int k = a->w;
  for (int x = 0; x < c->h; x++) {
    for (int y = 0; y < c->w; y++) {
      double sum = 0;
      for (int i = 0; i < k; i++)
        sum += a->data[x][i] * b->data[i][y];
      c->data[x][y] = sum;
    }
  }
  return c;
}