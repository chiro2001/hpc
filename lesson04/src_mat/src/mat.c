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
  memset(mat, 0, sizeof(Mat));
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
  mat->content_real = malloc(sizeof(CHIMAT_TYPE) * (mat->w * mat->h + 2));
  // assert(mat->content);
  // memset(mat->content, 0, sizeof(CHIMAT_TYPE) * (mat->h * mat->w + 2));
  // printf("\tMalloced at: %p, %p %% 32 = %lu\n", mat->content_real,
  //        mat->content_real, (size_t)(mat->content_real) % 32);
  // �����ֶ����� 32 λ
  mat->content =
      (double*)((void*)mat->content_real + (size_t)(mat->content_real) % 32);
  // printf("\tNow content at: %p, %% 32 = %lu\n", mat->content,
  //        (size_t)(mat->content) % 32);
  // Ȼ����������
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

// @prog: ���ٳ�ʼ�����������(ֻ��ʼ��һ��ά�ȣ��������������)
// @args: ����ָ��
// @rets: ����ָ��
Mat* mat_data_init_fast(Mat* mat) {
  mat->data = malloc(sizeof(CHIMAT_TYPE*) * mat->w);
  assert(mat->data);
  memset(mat->data, 0, sizeof(CHIMAT_TYPE*) * mat->w);
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

// @prog: ǳ����һ������
// @args: ����ָ��
// @rets: �µľ���ָ��
Mat* mat_clone_fast(Mat* mat) {
  Mat* dist = mat_create(mat->w, mat->h);
  mat_data_init_fast(dist);
  // ��������
  for (int i = 0; i < mat->w; i++) dist->data[i] = mat->data[i];
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
Mat* mat_mul(Mat* a, Mat* b, Mat* c_source) {
  // ����Ƿ�Ϸ�
  if (a->w != b->h) {
    return NULL;
  }
  // �������ⲿ�ṩԴ
  Mat* c = NULL;
  if (c_source != NULL) {
    c = c_source;
  } else {
    c = mat_create(a->h, b->w);
    mat_data_init(c);
  }
  int k = a->w;
  for (int x = 0; x < c->w; x++) {
    for (int y = 0; y < c->h; y++) {
      double sum = 0;
      for (int i = 0; i < k; i++) sum += a->data[x][i] * b->data[i][y];
      c->data[x][y] = sum;
    }
  }
  return c;
}

// @prog: �Ծ������һ���õ�ת
// @args: a
// @rets: a^T
Mat* mat_transpose(Mat* a) {
  if (!a) return NULL;
  Mat* t = mat_create(a->h, a->w);
  mat_data_init(t);
  for (int x = 0; x < t->w; x++) {
    for (int y = 0; y < t->h; y++) {
      t->data[x][y] = a->data[y][x];
    }
  }
  return t;
}

// @prog:
// ���߳�ִ�о���˷�������һ���µľ�����Ϊ������������ֱ����Ŀ�ľ������
// @args: a * b -> c
// @rets: c
int** mat_task_list = NULL;
int* mat_task_data = NULL;
int mat_task_tail = 0;
pthread_mutex_t mat_task_mutex = PTHREAD_MUTEX_INITIALIZER;

#define PINT(x) printf(#x "\t= %d\n", (int)x)
#define PD(x) printf(#x "\t= %lf\n", (double)x)
#define PD4(x)                                                               \
  printf("\t" #x "\t= %2.2lf, %2.2lf, %2.2lf, %2.2lf\n", ((double*)(&x))[0], \
         ((double*)(&x))[1], ((double*)(&x))[2], ((double*)(&x))[3])

void mat_mul_cell(mat_mul_thread_t* thread_data) {
  Mat* a = thread_data->a;
  Mat* b = thread_data->b;
  Mat* c = thread_data->c;
  int id = thread_data->id;
  // printf("Thread-%2d started!\n", thread_data->id);
  free(thread_data);
  int k = a->w;
  int index = 0;

  const size_t block_size = 4;
  size_t block_count = k / block_size;
  size_t remain_count = k % block_size;
  __m256d sum_sp, sum_bl, sum_2;
  __m256d load_a, load_b;
  // __m256d vector_a[512], vector_b[512];
  // double vector_a[512], vector_b[512];
  // printf("T-%2d prepare for while, block_count = %d\n", id, (int)block_count);
  // PINT(block_size);
  // PINT(remain_count);
  while (1) {
    pthread_mutex_lock(&mat_task_mutex);
    if (mat_task_tail == 0) {
      pthread_mutex_unlock(&mat_task_mutex);
      break;
    }
    index = --mat_task_tail;
    pthread_mutex_unlock(&mat_task_mutex);
    double sum = 0;
    sum_sp = _mm256_setzero_pd();
    sum_bl = _mm256_setzero_pd();
    sum_2 = _mm256_setzero_pd();

    int x = mat_task_list[index][0], y = mat_task_list[index][1];
    // printf("got index: %d @ (%d, %d)\n", index, x, y);

    // printf("Thread-%2d (%2d, %2d) finished.\n", id, x, y);
    // for (int i = 0; i < k; i += 4) {
    //   // �ֶ�ѭ��չ������
    //   sum += a->data[x][i] * b->data[i][y];
    //   sum += a->data[x][i + 1] * b->data[i + 1][y];
    //   sum += a->data[x][i + 2] * b->data[i + 2][y];
    //   sum += a->data[x][i + 3] * b->data[i + 3][y];
    // }

    // for (int i = 0; i < k; i++) {
    //   sum += a->data[x][i] * b->data[i][y];
    // }
    // c->data[x][y] = sum;

    // ���ȿ����� vector
    // for (int i = 0; i < block_size; i++) {
    //   // double* p = (double*)(&vector_b[i]);
    //   // p[0] = b->data[i + 0][y];
    //   // p[1] = b->data[i + 1][y];
    //   // p[2] = b->data[i + 2][y];
    //   // p[3] = b->data[i + 3][y];
    //   // p = (double*)(&vector_a[i]);
    //   // ���ڴ�λ�ÿ���
    //   memcpy(vector_a + i, a->data[x]);
    //   // p[0] = a->data[x][i + 0];
    //   // p[1] = a->data[x][i + 1];
    //   // p[2] = a->data[x][i + 2];
    //   // p[3] = a->data[x][i + 3];

    // }
    // ��Ϊ�Ѿ�ת���ˣ��ڴ�Ҳ�������ģ���ֱ�ӿ��� content ��
    // memcpy(vector_a, a->content, sizeof(double) * block_size * block_count);
    // memcpy(vector_b, b->content, sizeof(double) * block_size * block_count);
    // double *p_a = (double*)(&vector_a), *p_b = (double*)(&vector_b);
    double *p_a = a->content + x * a->w, *p_b = b->content + y * b->w;
    // ʹ�� AVX ָ�
    // printf("a->content = ");
    // for (int i = 0; i < a->w * a->h; i++) {
    //   printf("%2.2lf%s", a->content[i], i == (a->w * a->h - 1) ? "\n" : ",
    //   ");
    // }
    for (int i = 0; i < block_count; i++) {
      // load_a = _mm256_load_pd((double*)(&vector_a[i]));
      // load_b = _mm256_load_pd((double*)(&vector_b[i]));
      load_a = _mm256_load_pd(p_a);
      load_b = _mm256_load_pd(p_b);
      // load_a = _mm256_load_pd(a->content + i * block_size);
      // load_b = _mm256_load_pd(b->content + i * block_size);
      p_a += block_size;
      p_b += block_size;
      // p_a += 1;
      // p_b += 1;
      // PD4(vector_a[i]);
      // PD4(vector_b[i]);
      // printf("T-%2d debug #3.\n", id);
      // load_a = _mm256_load_pd(a->data[x] + i);
      sum_sp = _mm256_mul_pd(load_a, load_b);
      // sum_sp = _mm256_mul_pd(vector_a[i], vector_a[i]);
      // printf("%lf * %lf ==? %lf\n", ((double*)(&load_a))[0],
      //        ((double*)(&load_b))[0], ((double*)(&sum_sp))[0]);
      // exit(0);
      sum_2 = _mm256_add_pd(sum_sp, sum_bl);
      // printf("%lf + %lf ==? %lf %3s\n", ((double*)(&sum_sp))[0],
      //        ((double*)(&sum_bl))[0], ((double*)(&sum_2))[0],
      //        fabs((((double*)(&sum_sp))[0] + ((double*)(&sum_bl))[0]) -
      //             ((double*)(&sum_2))[0]) < 1e-4
      //            ? "YES"
      //            : "NO");

      // printf("==== loaded i = %d ====\n", i);
      // PD4(load_a);
      // PD4(load_b);
      // PD4(sum_sp);
      // PD4(sum_bl);
      // PD4(sum_2);
      // PD4(*p_a);
      // PD4(*p_b);

      sum_bl = sum_2;

      // if (fabs(((double*)(&sum_bl))[0]) > 1e-4) exit(0);

      // printf("calced: %lf\n", *((double*)(&sum_bl)));

      // load_a = _mm256_setzero_pd();
      // load_b = _mm256_setzero_pd();
      // puts("==== set zero ====");
      // PD4(load_a);
      // PD4(load_b);
    }
    double* p = (double*)(&sum_bl);
    sum = p[0] + p[1] + p[2] + p[3];
    for (int i = 0; i < remain_count; i++)
      sum += a->data[x][i + block_size * block_count] *
             //  b->data[i + block_size * block_count][y];
             b->data[y][i + block_size * block_count];
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

#define MAT_PRINT_WIDTH 5
#define MAT_PRINT_FORMAT "%2.2lf"

void mat_print(Mat* a) {
  const char format_all[] = "Mat([%s], shape=(%d, %d))";
  // const char format_line[] = "    [%s], \n";
  char* content = malloc(sizeof(char) * a->w * a->h * (MAT_PRINT_WIDTH + 3) +
                         sizeof(char) * a->h * (4 + 5));
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
  puts(content);
  free(content);
}

Mat* mat_mul_threaded(Mat* a, Mat* b, Mat* c_source, int threaded) {
  // ����Ƿ�Ϸ�
  if (a->w != b->h) {
    return NULL;
  }
  // �������ⲿ�ṩԴ
  Mat* c = NULL;
  if (c_source != NULL) {
    c = c_source;
  } else {
    c = mat_create(a->h, b->w);
    mat_data_init(c);
  }
  // ���ȶ� b ����һ���õ�ת
  Mat* t = mat_transpose(b);
  // puts("B^T:");
  // mat_print(t);
  // int k = a->w;
  // ��ʼ�������б���̳߳�
  mat_task_list = malloc(sizeof(int*) * (a->h * b->w));
  my_assert(mat_task_list, __LINE__);
  mat_task_data = malloc(sizeof(int) * (a->h * b->w) * 2);
  my_assert(mat_task_data, __LINE__);
  mat_task_tail = a->h * b->w;
  // int processor_number = sysconf(_SC_NPROCESSORS_ONLN);
  int processor_number = 1;
  if (threaded) processor_number = sysconf(_SC_NPROCESSORS_ONLN);
  // printf("processor_number = %d\n", processor_number);
  pthread_t* pool = malloc(sizeof(pthread_t) * processor_number);
  my_assert(pool, __LINE__);
  // ��ʼ������
  for (int x = 0; x < c->h; x++) {
    for (int y = 0; y < c->w; y++) {
      mat_task_list[x * c->w + y] = &mat_task_data[(x * c->w + y) * 2];
      mat_task_list[x * c->w + y][0] = x;
      mat_task_list[x * c->w + y][1] = y;
    }
  }
  // �����̳߳�
  int ret = 0;
  for (int i = 0; i < processor_number; i++) {
    mat_mul_thread_t* thread_data = malloc(sizeof(mat_mul_thread_t));
    thread_data->a = a;
    thread_data->b = t;
    thread_data->c = c;
    thread_data->id = i;
    ret = pthread_create(&pool[i], NULL, (void* (*)(void*))mat_mul_cell,
                         thread_data);
    if (ret) {
      printf("Cannot create thread!\n");
    }
  }
  // �ȴ������߳�ִ�����
  for (int i = 0; i < processor_number; i++) {
    pthread_join(pool[i], NULL);
  }
  return c;
}