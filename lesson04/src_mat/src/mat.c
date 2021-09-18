#include "mat.h"

#include <assert.h>
#include <malloc.h>
// #include <pmmintrin.h>
#include <immintrin.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
  memset(mat, 0, sizeof(Mat));
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
  memset(mat->content, 0, sizeof(CHIMAT_TYPE) * (mat->h * mat->w));
  // 然后做个索引
  mat->data = malloc(sizeof(CHIMAT_TYPE*) * mat->w);
  assert(mat->data);
  for (int i = 0; i < mat->h; i++) mat->data[i] = mat->content + i * mat->w;
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
  memset(mat->data, 0, sizeof(CHIMAT_TYPE*) * mat->w);
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
Mat* mat_mul(Mat* a, Mat* b, Mat* c_source) {
  // 检查是否合法
  if (a->w != b->h) {
    return NULL;
  }
  // 可以在外部提供源
  Mat* c = NULL;
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
      for (int i = 0; i < k; i++) sum += a->data[x][i] * b->data[i][y];
      c->data[x][y] = sum;
    }
  }
  return c;
}

// @prog:
// 多线程执行矩阵乘法，返回一个新的矩阵作为运算结果，或者直接在目的矩阵操作
// @args: a * b -> c
// @rets: c
int** mat_task_list = NULL;
int* mat_task_data = NULL;
int mat_task_tail = 0;
pthread_mutex_t mat_task_mutex = PTHREAD_MUTEX_INITIALIZER;

void mat_mul_cell(mat_mul_thread_t* thread_data) {
  Mat* a = thread_data->a;
  Mat* b = thread_data->b;
  Mat* c = thread_data->c;
  int id = thread_data->id;
  // printf("Thread-%2d started!\n", thread_data->id);
  free(thread_data);
  int k = a->w;
  int index = 0;

  size_t block_size = 4;
  size_t block_count = k / block_size;
  size_t remain_count = k % block_size;
  __m256d sum_split, sum_block, sum_2;
  __m256d load_a, load_b;
  __m256d vector_a[512], vector_b[512];
  // double vector_a[512], vector_b[512];
  printf("T-%2d prepare for while, block_count = %d\n", id, (int)block_count);
  while (1) {
    pthread_mutex_lock(&mat_task_mutex);
    if (mat_task_tail == 0) {
      pthread_mutex_unlock(&mat_task_mutex);
      break;
    }
    index = --mat_task_tail;
    // printf("got index: %d\n", index);
    pthread_mutex_unlock(&mat_task_mutex);
    double sum = 0;
    sum_split = _mm256_setzero_pd();
    sum_block = _mm256_setzero_pd();

    int x = mat_task_list[index][0], y = mat_task_list[index][1];
    // printf("Thread-%2d (%2d, %2d) finished.\n", id, x, y);
    // for (int i = 0; i < k; i += 4) {
    //   // 手动循环展开试试
    //   sum += a->data[x][i] * b->data[i][y];
    //   sum += a->data[x][i + 1] * b->data[i + 1][y];
    //   sum += a->data[x][i + 2] * b->data[i + 2][y];
    //   sum += a->data[x][i + 3] * b->data[i + 3][y];
    // }

    // for (int i = 0; i < k; i++) {
    //   sum += a->data[x][i] * b->data[i][y];
    // }
    // c->data[x][y] = sum;

    // 首先拷贝成 vector
    for (int i = 0; i < block_size; i++) {
      double* p = (double*)(&vector_b[i]);
      p[0] = b->data[i + 0][y];
      p[1] = b->data[i + 1][y];
      p[2] = b->data[i + 2][y];
      p[3] = b->data[i + 3][y];
      p = (double*)(&vector_a[i]);
      p[0] = a->data[x][i + 0];
      p[1] = a->data[x][i + 1];
      p[2] = a->data[x][i + 2];
      p[3] = a->data[x][i + 3];
    }
    // 使用 AVX 指令集
    for (int i = 0; i < block_count; i++) {
      load_a = _mm256_load_pd((double*)(&vector_a[i]));
      load_b = _mm256_load_pd((double*)(&vector_b[i]));
      // printf("T-%2d debug #3.\n", id);
      // load_a = _mm256_load_pd(a->data[x] + i);
      sum_split = _mm256_mul_pd(load_a, load_b);
      // printf("%lf * %lf ==? %lf\n", ((double*)(&load_a))[0],
      //        ((double*)(&load_b))[0], ((double*)(&sum_split))[0]);
      // exit(0);
      sum_2 = _mm256_add_pd(sum_split, sum_block);
      // printf("%lf + %lf ==? %lf %3s\n", ((double*)(&sum_split))[0],
      //        ((double*)(&sum_block))[0], ((double*)(&sum_2))[0],
      //        fabs((((double*)(&sum_split))[0] + ((double*)(&sum_block))[0]) -
      //             ((double*)(&sum_2))[0]) < 1e-4
      //            ? "YES"
      //            : "NO");
      sum_block = sum_2;

      // if (fabs(((double*)(&sum_block))[0]) > 1e-4) exit(0);

      // printf("calced: %lf\n", *((double*)(&sum_block)));
    }
    double* p = (double*)(&sum_block);
    sum = p[0] + p[1] + p[2] + p[3];
    for (int i = 0; i < remain_count; i++)
      sum += a->data[x][i + block_size * block_count] *
             b->data[i + block_size * block_count][y];
    c->data[x][y] = sum;
    // printf("c->data[x][y] = %lf\n", c->data[x][y]);
  }
}

void my_assert(void* x, int line) {
  if (!x) {
    printf("ERROR!");
    exit(1);
  }
}

Mat* mat_mul_threaded(Mat* a, Mat* b, Mat* c_source) {
  // 检查是否合法
  if (a->w != b->h) {
    return NULL;
  }
  // 可以在外部提供源
  Mat* c = NULL;
  if (c_source != NULL) {
    c = c_source;
  } else {
    c = mat_create(a->h, b->w);
    mat_data_init(c);
  }
  // int k = a->w;
  // 初始化任务列表和线程池
  mat_task_list = malloc(sizeof(int*) * (a->h * b->w));
  my_assert(mat_task_list, __LINE__);
  mat_task_data = malloc(sizeof(int) * (a->h * b->w) * 2);
  my_assert(mat_task_data, __LINE__);
  mat_task_tail = a->h * b->w;
  // int processor_number = sysconf(_SC_NPROCESSORS_ONLN);
  int processor_number = 1;
  // printf("processor_number = %d\n", processor_number);
  pthread_t* pool = malloc(sizeof(pthread_t) * processor_number);
  my_assert(pool, __LINE__);
  // 初始化任务
  for (int x = 0; x < c->h; x++) {
    for (int y = 0; y < c->w; y++) {
      mat_task_list[x * c->w + y] = &mat_task_data[(x * c->w + y) * 2];
      mat_task_list[x * c->w + y][0] = x;
      mat_task_list[x * c->w + y][1] = y;
    }
  }
  // 填满线程池
  int ret = 0;
  for (int i = 0; i < processor_number; i++) {
    mat_mul_thread_t* thread_data = malloc(sizeof(mat_mul_thread_t));
    thread_data->a = a;
    thread_data->b = b;
    thread_data->c = c;
    thread_data->id = i;
    ret = pthread_create(&pool[i], NULL, (void* (*)(void*))mat_mul_cell,
                         thread_data);
    if (ret) {
      printf("Cannot create thread!\n");
    }
  }
  // 等待所有线程执行完毕
  for (int i = 0; i < processor_number; i++) {
    pthread_join(pool[i], NULL);
  }
  return c;
}