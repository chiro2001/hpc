#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cblas.h"
#include "mat.h"

#define DEBUG

#ifdef DEBUG
#define pdebug(...) printf(__VA_ARGS__)
#else
#define pdebug(...)
#endif

double time_delta(struct timespec time1, struct timespec time2) {
  return ((double)(time2.tv_sec - time1.tv_sec) * 1000 +
          (double)(time2.tv_nsec - time1.tv_nsec) / 1000000) /
         1000;
}

void do_calc(int task_id, Mat* A, Mat* B, Mat* C, const char* task_name,
             double* results, int result_tail) {
  struct timespec start, end;
  pdebug("%10s计算: 开始计时\n", task_name);
  clock_gettime(CLOCK_REALTIME, &start);
  if (task_id == 0) {
    mat_mul(A, B, C);
  } else if (task_id == 1) {
    mat_mul_threaded(A, B, C, 0);
  } else if (task_id == 2) {
    mat_mul_threaded(A, B, C, 0);
  } else if (task_id == 3) {
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, A->w, B->h, A->h, 1,
                A->content, A->w, B->content, B->h, 0, C->content, C->h);
  }
  clock_gettime(CLOCK_REALTIME, &end);
  results[result_tail] = time_delta(start, end);
  pdebug("%10s: 计算用时: %3.3lfs\n", task_name, results[result_tail]);
}

int main(int argc, char** argv) {
  const enum CBLAS_ORDER Order = CblasRowMajor;
  const enum CBLAS_TRANSPOSE TransA = CblasNoTrans;
  const enum CBLAS_TRANSPOSE TransB = CblasNoTrans;
  // 处理一下参数
  int K = 4;
  if (argc <= 1) {
    pdebug("使用默认: K = %d\n", K);
  } else {
    K = atoi(argv[1]);
    pdebug("使用: K = %d\n", K);
  }
  int M = K, N = K;
  int lda = K;  // A的列
  int ldb = N;  // B的列
  int ldc = N;  // C的列
  pdebug("初始化两个矩阵\n");
  srand(time(NULL));
  Mat* A = mat_create(M, N);
  mat_data_init(A);
  Mat* B = mat_create(N, M);
  mat_data_init(B);
  for (int x = 0; x < M; x++) {
    for (int y = 0; y < N; y++) {
      // A->data[x][y] = sin((double)(x + y) / 360 + (rand() % 512) / 128);
      // B->data[x][y] = cos((double)(x * 3 + y * 2 + rand() % 512) / 360);
      // A->data[x][y] = (double)1 / (x + 1) + 100;
      // B->data[x][y] = (double)1 / (y + 1) + 1;
      // A->data[x][y] = sin((double)(rand() % M) / M);
      // B->data[x][y] = cos((double)(rand() % M) / M);
      A->data[x][y] = rand() % 10;
      B->data[x][y] = rand() % 10;
      // A->data[x][y] = x % 10;
      // B->data[x][y] = y % 10;
    }
  }
  // printf("A:\n");
  // mat_print(A);
  // printf("B:\n");
  // mat_print(B);
  // 初始化一下 C，不把初始化时间计入计算时间
  // pdebug("初始化 C\n");
  Mat* C[32] = {NULL};
  // for (int i = 0; i < 3; i++) {
  //   C[i] = mat_create(M, M);
  //   mat_data_init(C[i]);
  // }
  // 计算乘法，开始计时
  // clock_t start, end;
  struct timespec start, end;
  int result_tail = 0;
  double results[32];

  const char task_names[][64] = {"简单矩阵", "单线程矩阵优化", "多线程矩阵优化",
                                 "OpenBLAS"};
  const int task_number = 4;

  for (int i = 0; i < task_number; i++) {
    C[result_tail] = mat_create(M, M);
    mat_data_init(C[result_tail]);
    do_calc(i, A, B, C[result_tail], task_names[i], results, result_tail);
    result_tail++;
  }

  // C[result_tail] = mat_create(M, M);
  // mat_data_init(C[result_tail]);
  // pdebug("简单矩阵计算: 开始计时\n");
  // clock_gettime(CLOCK_REALTIME, &start);
  // mat_mul(A, B, C[result_tail]);
  // clock_gettime(CLOCK_REALTIME, &end);
  // results[result_tail++] = time_delta(start, end);
  // pdebug("简单矩阵: 计算用时: %lfs\n", results[result_tail - 1]);

  // C[result_tail] = mat_create(M, M);
  // mat_data_init(C[result_tail]);
  // pdebug("单线程矩阵优化计算: 开始计时\n");
  // clock_gettime(CLOCK_REALTIME, &start);
  // mat_mul_threaded(A, B, C[result_tail], 0);
  // clock_gettime(CLOCK_REALTIME, &end);
  // results[result_tail++] = time_delta(start, end);
  // pdebug("单线程矩阵优化: 计算用时: %lfs\n", results[result_tail - 1]);

  // C[result_tail] = mat_create(M, M);
  // mat_data_init(C[result_tail]);
  // pdebug("多线程矩阵优化计算: 开始计时\n");
  // clock_gettime(CLOCK_REALTIME, &start);
  // mat_mul_threaded(A, B, C[result_tail], 1);
  // clock_gettime(CLOCK_REALTIME, &end);
  // results[result_tail++] = time_delta(start, end);
  // pdebug("多线程矩阵优化: 计算用时: %lfs\n", results[result_tail - 1]);

  // C[result_tail] = mat_create(M, M);
  // mat_data_init(C[result_tail]);
  // pdebug("OpenBLAS: 开始计时\n");
  // clock_gettime(CLOCK_REALTIME, &start);
  // // C = alpha * AB + beta * C
  // cblas_dgemm(Order, TransA, TransB, M, N, K, 1, A->content, lda, B->content,
  //             ldb, 0, C[result_tail]->content, ldc);
  // clock_gettime(CLOCK_REALTIME, &end);
  // results[result_tail++] = time_delta(start, end);
  // pdebug("OpenBLAS: 计算用时: %lfs\n", results[result_tail - 1]);

  // 结果写入文件
  FILE* fp = fopen("results.txt", "w");
  if (!fp) {
    perror("Cannot open results.txt!!\n");
    return 1;
  }
  // fprintf(fp, "%lf\n%lf\n%lf\n", result_1, result_2, result_3);
  for (int i = 0; i < result_tail; i++) {
    fprintf(fp, "%s: %lf\n", task_names[i], results[i]);
  }
  fclose(fp);
  pdebug("校验...\n");
  int is_ok[32];
  for (int i = 0; i < result_tail; i++) {
    is_ok[i] = 1;
  }
  double eps = 1e-4;
  for (int i = 0; i < C[0]->w * C[0]->h; i++) {
    if (is_ok[0]) {
      if (fabs(C[0]->content[i] - C[1]->content[i]) >= eps) {
        printf("K = %4d; 多线程计算错误! (%d): [%lf, %lf]\n", K, i,
               C[0]->content[i], C[1]->content[i]);
        is_ok[0] = 0;
      }
    }
    if (is_ok[1]) {
      if (fabs(C[0]->content[i] - C[2]->content[i]) >= eps) {
        printf("K = %4d; OpenBLAS计算错误! (%d): [%lf, %lf]\n", K, i,
               C[0]->content[i], C[1]->content[i]);
        is_ok[1] = 0;
      }
    }
  }
  pdebug("DONE.\n");
  return 0;
}