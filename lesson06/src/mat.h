#ifndef CHIMAT_H
#define CHIMAT_H

#include <immintrin.h>
#include <stdint.h>

// Author: Chiro2001

// 是否使用连续的内存
#define USE_CONTIGROUS_MEM
#define CHIMAT_TYPE double
// #define CHIMAT_TYPE __attribute((vector_size(32))) double

#define MAT_PRINT_WIDTH 5
#define MAT_PRINT_FORMAT "\t%2.2lf"

typedef struct {
  int w;
  int h;
  CHIMAT_TYPE **data;
  CHIMAT_TYPE *content;
  CHIMAT_TYPE *content_real;
  int aligned;
} Mat;

typedef struct {
  int id;
  Mat *a;
  Mat *b;
  Mat *c;
  int unrolling;
  int single;
  int single_index;
} mat_mul_thread_t;

Mat *mat_create(int width, int height, int aligned);
Mat *mat_data_init(Mat *mat);
// Mat* mat_data_init(Mat* mat, int align);
Mat *mat_data_init_fast(Mat *mat);
void mat_free(Mat *mat);
void mat_free_fast(Mat *mat);
Mat *mat_clone(Mat *mat);
Mat *mat_crop(Mat *mat, int x1, int y1, int x2, int y2);
Mat *mat_padding_around(Mat *mat, int padding);
Mat *mat_conv(Mat *mat, double kernel[3][3]);
Mat *mat_mul(Mat *a, Mat *b, Mat *c_source);

void mat_print(Mat *a);

// Mat* mat_mul_threaded(Mat* a, Mat* b, Mat* c_source, int threaded);
Mat *mat_mul_threaded(Mat *a, Mat *b, Mat *c, int processor_number,
                      int unrolling);
void mat_mul_cell(mat_mul_thread_t *thread_data);

Mat *mat_mul_openmp_native(Mat *a, Mat *b, Mat *c);
Mat *mat_mul_openmp(Mat *a, Mat *b, Mat *c, int unrolling);

double mat_cell_do_mul(double *p_a, double *p_b, int unrolling, int k);
Mat *mat_mul_mpi(Mat *a, Mat *b, Mat *c, int unrolling);

extern double mat_native_time_limit;
extern int mat_native_timeout;
double mat_openmp_time_limit;
int mat_openmp_timeout;

#endif