#include "mat.h"
#include <mpi.h>

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

#include "utils.h"

// Author: Chiro2001
// 代码来自大一上的 C 语言实验课的作品

// 标注是否使用快速crop模式
// #define USE_FAST_CROP
// 但是现在发现基于指针的fast模式比不用更加慢
// malloc牛逼

// @prog: 创建图像矩阵
// @args: 图像宽, 高
// @rets: 矩阵指针
Mat *mat_create(int width, int height, int aligned) {
  Mat *mat = NULL;
  mat = malloc(sizeof(Mat));
  memset(mat, 0, sizeof(Mat));
  assert(mat);
  mat->w = width;
  mat->h = height;
  mat->aligned = aligned;
  mat->data = NULL;
  mat->content_real = NULL;
  mat->content = NULL;
  return mat;
}

Mat *mat_make_index(Mat *mat) {
  // 尝试手动对齐 32 位
  mat->content =
      (double *)((void *)mat->content_real + (size_t)(mat->content_real) % 32);
  // PINT((int)((size_t)mat->content - (size_t)mat->content_real));
  if (mat->data)
    free(mat->data);
  mat->data = malloc(sizeof(CHIMAT_TYPE *) * mat->w);
  assert(mat->data);
  // printf("content @ [%p, %p]\n", mat->content, mat->content + mat->w *
  // mat->h); 注意，索引也要对齐
  if (mat->aligned)
    for (int i = 0; i < mat->h; i++) {
      // mat->data[i] = mat->content + (i * (mat->w + (4 - mat->w % 4)));
      mat->data[i] = mat->content + (i * ((mat->w / 4 + 1) * 4));
      // printf("mat->data[i] - mat->content = %ld\n",
      //        mat->data[i] - mat->content);
    }
  else
    for (int i = 0; i < mat->h; i++)
      mat->data[i] = mat->content + i * mat->w;
  return mat;
}

// @prog: 初始化矩阵的数据
// @args: 矩阵指针
// @rets: 矩阵指针
Mat *mat_data_init(Mat *mat) {
  if (!mat) {
    return NULL;
  }
#ifdef USE_CONTIGROUS_MEM
  // 直接申请一整块空间罢
  // size_t mem_count = ((mat->w + (mat->w % 4)) * (mat->w + (mat->w % 4)) + 4);
  size_t mem_count = ((mat->w / 4 + 1) * 4) * ((mat->h / 4 + 1) * 4) + 4;
  // PINT(mem_count);
  mat->content_real = malloc(sizeof(CHIMAT_TYPE) * mem_count);
  assert(mat->content_real);
  memset(mat->content_real, 0, sizeof(CHIMAT_TYPE) * mem_count);
  // 做个索引
  mat_make_index(mat);
  return mat;
#else
  mat->content = NULL;
  mat->data = malloc(sizeof(CHIMAT_TYPE *) * mat->w);
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
Mat *mat_data_init_fast(Mat *mat) {
  mat->data = malloc(sizeof(CHIMAT_TYPE *) * mat->w);
  assert(mat->data);
  memset(mat->data, 0, sizeof(CHIMAT_TYPE *) * mat->w);
  return mat;
}

// @prog: 释放矩阵的内存
// @args: 矩阵指针
// @rets: void
void mat_free(Mat *mat) {
  // for (int i = 0; i < mat->w; i++) {
  //   free(mat->data[i]);
  // }
  free(mat->content_real);
  free(mat->data);
  free(mat);
  mat->content_real = NULL;
  mat->content = NULL;
  mat->data = NULL;
}

// @prog: 释放被快速初始化的矩阵的数据
// @args: 矩阵指针
// @rets: void
void mat_free_fast(Mat *mat) {
  free(mat->data);
  free(mat);
  mat->content_real = NULL;
  mat->content = NULL;
  mat->data = NULL;
}

// @prog: 深度拷贝一个矩阵
// @args: 矩阵指针
// @rets: 新的矩阵指针
Mat *mat_clone(Mat *mat) {
  Mat *dist = mat_create(mat->w, mat->h, mat->aligned);
  mat_data_init(dist);
  for (int i = 0; i < mat->w; i++)
    for (int j = 0; j < mat->h; j++)
      dist->data[i][j] = mat->data[i][j];
  return dist;
}

// @prog: 浅拷贝一个矩阵
// @args: 矩阵指针
// @rets: 新的矩阵指针
Mat *mat_clone_fast(Mat *mat) {
  Mat *dist = mat_create(mat->w, mat->h, mat->aligned);
  mat_data_init_fast(dist);
  // 更新索引
  for (int i = 0; i < mat->w; i++)
    dist->data[i] = mat->data[i];
  return dist;
}

// @prog: 从矩阵中选区建立新的矩阵
// @args: 矩阵指针, Rect(x1, y1, x2, y2)
// @rets: 矩阵指针
Mat *mat_crop(Mat *mat, int x1, int y1, int x2, int y2) {
  if (x1 > x2 || y1 > y2 || !mat)
    return NULL;
  int w = x2 - x1, h = y2 - y1;
  Mat *dist = mat_create(w, h, mat->aligned);
#ifndef USE_FAST_CROP
  mat_data_init(dist);
  for (int i = 0; i < w; i++)
    for (int j = 0; j < h; j++)
      dist->data[i][j] = mat->data[i + x1][j + y1];
#else
  mat_data_init_fast(dist);
  for (int i = 0; i < w; i++)
    dist->data[i] = mat->data[i + x1] + y1;
#endif // USE_FAST_CROP
  // 打印调试信息会使程序运行效率下降(惨不忍睹)
  // printf("\tcrop: (%d, %d, %d, %d)\n", x1, y1, x2, y2);
  return dist;
}

// @prog: 矩阵外扩展0，生成新的矩阵
// @args: 矩阵指针, 扩展像素宽度
// @rets: 矩阵指针
Mat *mat_padding_around(Mat *mat, int padding) {
  if (!mat)
    return NULL;
  int w = mat->w + padding * 2, h = mat->h + padding * 2;
  Mat *dist = mat_create(w, h, mat->aligned);
  mat_data_init(dist);
  for (int i = 0; i < mat->w; i++)
    for (int j = 0; j < mat->h; j++)
      dist->data[i + padding][j + padding] = mat->data[i][j];
  return dist;
}

// @prog: 计算矩阵(3x3大小)卷积
// @args: 矩阵指针, 卷积核
// @rets: 像素值
CHIMAT_TYPE mat_kernel_calc(Mat *mat, double kernel[3][3]) {
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
Mat *mat_conv(Mat *mat, double kernel[3][3]) {
  // 先扩大边界
  Mat *pad = mat_padding_around(mat, 1);
  Mat *dist = mat_clone(mat);
  for (int i = 0; i < mat->w; i++) {
    for (int j = 0; j < mat->h; j++) {
      Mat *crop = mat_crop(pad, i, j, i + 3, j + 3);
      dist->data[i][j] = mat_kernel_calc(crop, kernel);
#ifndef USE_FAST_CROP
      mat_free(crop);
#else
      mat_free_fast(crop);
#endif // USE_FAST_CROP
       // printf("#1.3 (%d, %d) %d %d %d\n", i, j,
       // dist->data[i][j].red,dist->data[i][j].green, dist->data[i][j].blue);
    }
  }
  return dist;
}

// @prog: 执行矩阵乘法，返回一个新的矩阵作为运算结果，或者直接在目的矩阵操作
// @args: a * b -> c
// @rets: c
Mat *mat_mul(Mat *a, Mat *b, Mat *c) {
  // 检查是否合法
  if ((!a || !b || !c) || (a->w != b->h))
    return NULL;
  // 计时，超时的话就直接返回
  int k = a->w;
#ifdef USE_TIMEOUT_NATIVE
  struct timespec start, end;
  clock_gettime(CLOCK_REALTIME, &start);
#endif
  for (int x = 0; x < c->w; x++) {
    for (int y = 0; y < c->h; y++) {
      double sum = 0;
      for (int i = 0; i < k; i++)
        sum += a->data[x][i] * b->data[i][y];
      c->data[x][y] = sum;
    }
#ifdef USE_TIMEOUT_NATIVE
    if ((c->h >= 256 || c->w >= 256) && mat_native_time_limit > eps) {
      clock_gettime(CLOCK_REALTIME, &end);
      double delta = time_delta(start, end);
      if (delta > mat_native_time_limit) {
        mat_native_timeout = 1;
        return c;
      }
    }
#endif
  }
  return c;
}

// @prog: 用 OpenMP 执行矩阵乘法
// @args: a * b -> c
// @rets: c
Mat *mat_mul_openmp_native(Mat *a, Mat *b, Mat *c) {
  // 检查是否合法
  if ((!a || !b || !c) || (a->w != b->h))
    return NULL;
  // 计时，超时的话就直接返回
  int k = a->w;
#ifdef USE_TIMEOUT_OPENMP
  struct timespec start, end;
  clock_gettime(CLOCK_REALTIME, &start);
#endif
#pragma omp parallel for
  for (int x = 0; x < c->w; x++) {
    for (int y = 0; y < c->h; y++) {
      double sum = 0;
      for (int i = 0; i < k; i++)
        sum += a->data[x][i] * b->data[i][y];
      c->data[x][y] = sum;
    }
#ifdef USE_TIMEOUT_OPENMP
    if ((c->h >= 256 || c->w >= 256) && mat_openmp_time_limit > eps) {
      clock_gettime(CLOCK_REALTIME, &end);
      double delta = time_delta(start, end);
      if (delta > mat_openmp_time_limit) {
        mat_native_timeout = 1;
        return c;
      }
    }
#endif
  }
  return c;
}

// @prog: 对矩阵进行一个印的打
// @args: a
void mat_print(Mat *a) {
  const char format_all[] = "Mat([%s], shape=(%d, %d))";
  // const char format_line[] = "    [%s], \n";
  char *content = malloc(sizeof(char) * a->w * a->h * (MAT_PRINT_WIDTH + 10) +
                         sizeof(char) * a->h * (4 + 5 + 10));
  // char* buf[512];
  memcpy(content, format_all, sizeof(char) * 4);
  int last = 4;
  for (int x = 0; x < a->h; x++) {
    if (x != 0)
      strcpy(content + last, "     [");
    else
      strcpy(content + last, "[[");
    last += strlen(content + last);
    for (int y = 0; y < a->w; y++) {
      if (y != a->w - 1) {
        sprintf(content + last, MAT_PRINT_FORMAT ", ", a->data[x][y]);
      } else {
        sprintf(content + last, MAT_PRINT_FORMAT, a->data[x][y]);
      }
      last += strlen(content + last);
    }
    if (x != a->h - 1) {
      strcpy(content + last, "], \n");
    } else {
      strcpy(content + last, "]]");
    }
    last += strlen(content + last);
  }
  sprintf(content + last, format_all + 8, a->w, a->h);
  pdebug("%s\n", content);
  free(content);
}

// @prog: 对矩阵进行一个置的转
// @args: a
// @rets: a^T
Mat *mat_transpose(Mat *a) {
  if (!a)
    return NULL;
  Mat *t = mat_create(a->h, a->w, a->aligned);
  mat_data_init(t);
  for (int x = 0; x < t->w; x++) {
    for (int y = 0; y < t->h; y++) {
      t->data[x][y] = a->data[y][x];
    }
  }
  return t;
}

// @prog:
// 多线程执行矩阵乘法，返回一个新的矩阵作为运算结果，或者直接在目的矩阵操作
// @args: a * b -> c
// @rets: c
int **mat_task_list = NULL;
int *mat_task_data = NULL;
int mat_task_tail = 0;
pthread_mutex_t mat_task_mutex = PTHREAD_MUTEX_INITIALIZER;

double mat_cell_do_mul(double *p_a, double *p_b, int k, int unrolling,
                       int native) {
  const size_t block_size = 4;
  size_t block_count = k / block_size;
  size_t remain_count = k % block_size;
  __m256d sum_sp, sum_bl;
  sum_bl = _mm256_setzero_pd();
  __m256d load_a, load_b;
  __m256d load_a_0, load_b_0, load_a_1, load_b_1, load_a_2, load_b_2, load_a_3,
      load_b_3;
  __m256d sum_sp_0, sum_sp_1, sum_sp_2, sum_sp_3, sum_sp_0_0, sum_sp_0_1,
      sum_sp_0_0_0;
  double sum;
  if (native) {
    sum = 0;
    for (int i = 0; i < k; i++)
      sum += p_a[i] * p_b[i];
    return sum;
  }
  // 使用 AVX 指令集
  if (!unrolling) {
    for (int i = 0; i < block_count; i++) {
      load_a = _mm256_load_pd(p_a);
      load_b = _mm256_load_pd(p_b);
      // PD4(load_a);
      // PD4(load_b);
      p_a += block_size;
      p_b += block_size;
      sum_sp = _mm256_mul_pd(load_a, load_b);
      sum_bl = _mm256_add_pd(sum_sp, sum_bl);
      // PD4(sum_bl);
    }
  } else {
    size_t block_remain = block_count % 4;
    // PINT(block_remain);
    // PINT(block_count / 4);
    for (int i = 0; i < block_count / 4; i++) {
      load_a_0 = _mm256_load_pd(p_a + 0);
      load_b_0 = _mm256_load_pd(p_b + 0);
      load_a_1 = _mm256_load_pd(p_a + 4);
      load_b_1 = _mm256_load_pd(p_b + 4);
      load_a_2 = _mm256_load_pd(p_a + 8);
      load_b_2 = _mm256_load_pd(p_b + 8);
      load_a_3 = _mm256_load_pd(p_a + 12);
      load_b_3 = _mm256_load_pd(p_b + 12);
      p_a += block_size * 4;
      p_b += block_size * 4;
      sum_sp_0 = _mm256_mul_pd(load_a_0, load_b_0);
      sum_sp_1 = _mm256_mul_pd(load_a_1, load_b_1);
      sum_sp_2 = _mm256_mul_pd(load_a_2, load_b_2);
      sum_sp_3 = _mm256_mul_pd(load_a_3, load_b_3);
      sum_sp_0_0 = _mm256_add_pd(sum_sp_0, sum_sp_1);
      sum_sp_0_1 = _mm256_add_pd(sum_sp_2, sum_sp_3);
      sum_sp_0_0_0 = _mm256_add_pd(sum_sp_0_0, sum_sp_0_1);
      sum_bl = _mm256_add_pd(sum_sp_0_0_0, sum_bl);
      // PD4(sum_bl);
    }
    for (int i = 0; i < block_remain; i++) {
      load_a = _mm256_load_pd(p_a);
      load_b = _mm256_load_pd(p_b);
      // PD4(load_a);
      // PD4(load_b);
      p_a += block_size;
      p_b += block_size;
      sum_sp = _mm256_mul_pd(load_a, load_b);
      sum_bl = _mm256_add_pd(sum_sp, sum_bl);
      // PD4(sum_bl);
    }
  }

  if (block_count > 0) {
    double *p = (double *)(&sum_bl);
    sum = p[0] + p[1] + p[2] + p[3];
  } else {
    sum = 0;
  }
  for (int i = 0; i < remain_count; i++) {
    // sum += a->data[x][i + block_size * block_count] *
    //        b->data[y][i + block_size * block_count];
    sum += (*p_a) * (*p_b);
    p_a += 1;
    p_b += 1;
  }
  return sum;
}

void mat_mul_cell(mat_mul_thread_t *thread_data) {
  Mat *a = thread_data->a;
  Mat *b = thread_data->b;
  Mat *c = thread_data->c;
  // int id = thread_data->id;
  int single = thread_data->single;
  int single_index = thread_data->single_index;
  int unrolling = thread_data->unrolling;
  if (!single)
    free(thread_data);
  int k = a->w;
  int index = 0;

  // 如果是只是运行单次就不需要再读取下一个任务了。
  while (1) {
    if (!single) {
      pthread_mutex_lock(&mat_task_mutex);
      if (mat_task_tail == 0) {
        pthread_mutex_unlock(&mat_task_mutex);
        break;
      }
      index = --mat_task_tail;
      pthread_mutex_unlock(&mat_task_mutex);
    } else {
      index = single_index;
    }

    int x = mat_task_list[index][0], y = mat_task_list[index][1];
    double *p_a = a->content + (x * ((a->w / 4 + 1) * 4)),
           *p_b = b->content + (y * ((b->w / 4 + 1) * 4));
    c->data[x][y] = mat_cell_do_mul(p_a, p_b, k, unrolling, 0);

    if (single)
      break;
  }
}

double mat_native_time_limit = 0;
int mat_native_timeout = 0;
double mat_openmp_time_limit = 0;
int mat_openmp_timeout = 0;

Mat *mat_mul_openmp(Mat *a, Mat *b, Mat *c, int unrolling) {
  // 检查是否合法
  if (a->w != b->h) {
    return NULL;
  }
  // 首先对 b 进行一个置的转
  Mat *t = mat_transpose(b);
  // puts("B^T:");
  // mat_print(t);
  // 初始化任务列表和线程池
  mat_task_tail = a->h * b->w;
  mat_task_list = malloc(sizeof(int *) * mat_task_tail);
  assert(mat_task_list);
  mat_task_data = malloc(sizeof(int) * mat_task_tail * 2);
  assert(mat_task_data);
  // 初始化任务
  for (int x = 0; x < c->h; x++) {
    for (int y = 0; y < c->w; y++) {
      mat_task_list[x * c->w + y] = &mat_task_data[(x * c->w + y) * 2];
      mat_task_list[x * c->w + y][0] = x;
      mat_task_list[x * c->w + y][1] = y;
    }
  }
  // 填满线程参数
  mat_mul_thread_t *thread_data =
      malloc(sizeof(mat_mul_thread_t) * mat_task_tail);
  for (int i = 0; i < mat_task_tail; i++) {
    thread_data[i].a = a;
    thread_data[i].b = t;
    thread_data[i].c = c;
    thread_data[i].id = i;
    thread_data[i].single = 1;
    thread_data[i].single_index = i;
    thread_data[i].unrolling = unrolling;
  }
#pragma omp parallel for
  for (int i = 0; i < mat_task_tail; i++) {
    mat_mul_cell(thread_data + i);
  }

  mat_free(t);
  return c;
}

Mat *mat_mul_threaded(Mat *a, Mat *b, Mat *c, int processor_number,
                      int unrolling) {
  // 检查是否合法
  if (a->w != b->h) {
    return NULL;
  }
  // 首先对 b 进行一个置的转
  Mat *t = mat_transpose(b);
  // puts("B^T:");
  // mat_print(t);
  // 初始化任务列表和线程池
  mat_task_tail = a->h * b->w;
  mat_task_list = malloc(sizeof(int *) * mat_task_tail);
  assert(mat_task_list);
  mat_task_data = malloc(sizeof(int) * mat_task_tail * 2);
  assert(mat_task_data);
  // printf("processor_number = %d\n", processor_number);
  pthread_t *pool = malloc(sizeof(pthread_t) * processor_number);
  assert(pool);
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
    mat_mul_thread_t *thread_data = malloc(sizeof(mat_mul_thread_t));
    thread_data->a = a;
    thread_data->b = t;
    thread_data->c = c;
    thread_data->id = i;
    thread_data->single = 0;
    thread_data->single_index = 0;
    thread_data->unrolling = unrolling;
    ret = pthread_create(&pool[i], NULL, (void *(*)(void *))mat_mul_cell,
                         thread_data);
    if (ret) {
      printf("Cannot create thread!\n");
    }
  }
  // 等待所有线程执行完毕
  for (int i = 0; i < processor_number; i++) {
    pthread_join(pool[i], NULL);
  }
  // mat_print(c);
  mat_free(t);
  return c;
}

Mat *mat_mul_mpi_all(Mat *a, Mat *b, Mat *c, int unrolling, int native) {
  // 检查是否合法
  if (a->w != b->h) {
    return NULL;
  }
  int rank = 0, size = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  // 首先对 b 进行一个置的转
  Mat *t = mat_transpose(b);
  // 同步内存信息
  if (rank == 0) {
    // 然后发送 a, b 数据到其他 slot
    for (int i = 1; i < size; i++) {
      // 按行发送数据，防止因为content_real不一致造成数据错误
      for (int x = 0; x < a->h; x++) {
        MPI_Send(a->data[x], a->w, MPI_DOUBLE, i, MPI_TAG_MAT_A,
                 MPI_COMM_WORLD);
        MPI_Send(t->data[x], a->w, MPI_DOUBLE, i, MPI_TAG_MAT_B,
                 MPI_COMM_WORLD);
      }
    }
  } else {
    for (int x = 0; x < a->h; x++) {
      MPI_Recv(a->data[x], a->w, MPI_DOUBLE, 0, MPI_TAG_MAT_A, MPI_COMM_WORLD,
               MPI_STATUSES_IGNORE);
      MPI_Recv(t->data[x], a->w, MPI_DOUBLE, 0, MPI_TAG_MAT_B, MPI_COMM_WORLD,
               MPI_STATUSES_IGNORE);
    }
  }
  // 初始化任务数据，进行数据分发
  double *sum_part = malloc(sizeof(double) * size);
  for (int x = rank; x < a->w - a->w % size; x += size) {
    for (int ys = 0; ys < b->h; ys += 1) {
      double sum =
          mat_cell_do_mul(a->data[x], t->data[ys], a->w, unrolling, native);
      MPI_Gather(&sum, 1, MPI_DOUBLE, sum_part, 1, MPI_DOUBLE, 0,
                 MPI_COMM_WORLD);
      if (rank == 0) {
        for (int i = 0; i < size; i++) {
          c->data[x + i][ys] = sum_part[i];
          // pdebug("DONE c[%d][%d]\n", x + i, ys);
        }
      }
    }
  }
  if (rank == 0)
    for (int xr = a->w - a->w % size; xr < a->w; xr++) {
      for (int ys = 0; ys < b->h; ys += 1) {
        c->data[xr][ys] =
            mat_cell_do_mul(a->data[xr], t->data[ys], a->w, unrolling, native);
        // pdebug("REMAIN c[%d][%d]\n", xr, ys);
      }
    }
  mat_free(t);
  free(sum_part);
  return c;
}