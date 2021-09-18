#include "mat.h"

#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Author: Chiro2001
// �������Դ�һ�ϵ� C ����ʵ��ε���Ʒ

// ��ע�Ƿ�ʹ�ÿ���cropģʽ
// #define USE_FAST_CROP
// �������ڷ��ֻ���ָ���fastģʽ�Ȳ��ø�����
// mallocţ��

// @prog: ����ͼ�����
// @args: ͼ���, ��
// @rets: ����ָ��
Mat* mat_create(int width, int height) {
  Mat* mat = NULL;
  mat = malloc(sizeof(Mat));
  memset(mat, sizeof(Mat), 0);
  assert(mat);
  mat->w = width;
  mat->h = height;
  return mat;
}

// @prog: ��ʼ�����������
// @args: ����ָ��
// @rets: ����ָ��
Mat* mat_data_init(Mat* mat) {
  if (!mat) {
    return NULL;
  }
#ifdef USE_CONTIGROUS_MEM
  // ֱ������һ����ռ��
  mat->content = malloc(sizeof(CHIMAT_TYPE) * (mat->w * mat->h));
  assert(mat->content);
  memset(mat->content, sizeof(CHIMAT_TYPE) * (mat->h * mat->w), 0);
  // Ȼ����������
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

// @prog: ���ٳ�ʼ�����������(ֻ��ʼ��һ��ά�ȣ��������������)
// @args: ����ָ��
// @rets: ����ָ��
Mat* mat_data_init_fast(Mat* mat) {
  mat->data = malloc(sizeof(CHIMAT_TYPE*) * mat->w);
  assert(mat->data);
  memset(mat->data, sizeof(CHIMAT_TYPE*) * mat->w, 0);
  return mat;
}

// @prog: �ͷž�����ڴ�
// @args: ����ָ��
// @rets: void
void mat_free(Mat* mat) {
  for (int i = 0; i < mat->w; i++) {
    free(mat->data[i]);
  }
  free(mat->data);
  free(mat);
}

// @prog: �ͷű����ٳ�ʼ���ľ��������
// @args: ����ָ��
// @rets: void
void mat_free_fast(Mat* mat) {
  free(mat->data);
  free(mat);
}

// @prog: ��ȿ���һ������
// @args: ����ָ��
// @rets: �µľ���ָ��
Mat* mat_clone(Mat* mat) {
  Mat* dist = mat_create(mat->w, mat->h);
  mat_data_init(dist);
  for (int i = 0; i < mat->w; i++)
    for (int j = 0; j < mat->h; j++) dist->data[i][j] = mat->data[i][j];
  return dist;
}

// @prog: �Ӿ�����ѡ�������µľ���
// @args: ����ָ��, Rect(x1, y1, x2, y2)
// @rets: ����ָ��
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
  // ��ӡ������Ϣ��ʹ��������Ч���½�(�Ҳ��̶�)
  // printf("\tcrop: (%d, %d, %d, %d)\n", x1, y1, x2, y2);
  return dist;
}

// @prog: ��������չ0�������µľ���
// @args: ����ָ��, ��չ���ؿ��
// @rets: ����ָ��
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

// @prog: �������(3x3��С)���
// @args: ����ָ��, �����
// @rets: ����ֵ
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

// @prog: �Ծ���������
// @args: ����ָ��, �����
// @rets: �µľ���ָ��
Mat* mat_conv(Mat* mat, double kernel[3][3]) {
  // ������߽�
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

// @prog: ִ�о���˷�������һ���µľ�����Ϊ������������ֱ����Ŀ�ľ������
// @args: a * b -> c
// @rets: c
Mat* mat_mul(Mat *a, Mat *b, Mat *c_source) {
  // ����Ƿ�Ϸ�
  if (a->w != b->h) {
    return NULL;
  }
  // �������ⲿ�ṩԴ
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