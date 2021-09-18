#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cblas.h"
#include "mat.h"

// #define DEBUG

#ifdef DEBUG
#define pdebug(...) printf(__VA_ARGS__)
#else
#define pdebug(...)
#endif

int main(int argc, char** argv) {
  const enum CBLAS_ORDER Order = CblasRowMajor;
  const enum CBLAS_TRANSPOSE TransA = CblasNoTrans;
  const enum CBLAS_TRANSPOSE TransB = CblasNoTrans;
  // 处理一下参数
  int K = 128;
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
      A->data[x][y] = sin((double)(rand() % M) / M);
      B->data[x][y] = cos((double)(rand() % M) / M);
    }
  }
  // 初始化一下 C，不把初始化时间计入计算时间
  pdebug("初始化 C\n");
  Mat* C[3] = {NULL, NULL, NULL};
  for (int i = 0; i < 3; i++) {
    C[i] = mat_create(M, M);
    mat_data_init(C[i]);
  }
  // 计算乘法，开始计时
  clock_t start, end;
  double result_1, result_2, result_3;
  pdebug("简单矩阵计算: 开始计时\n");
  start = clock();
  mat_mul(A, B, C[0]);
  end = clock();
  result_1 = (double)(end - start) / CLOCKS_PER_SEC;
  pdebug("简单矩阵: 计算用时: %lfs\n", result_1);

  pdebug("多线程矩阵计算: 开始计时\n");
  start = clock();
  mat_mul_threaded(A, B, C[1]);
  end = clock();
  result_2 = (double)(end - start) / CLOCKS_PER_SEC;
  pdebug("多线程矩阵: 计算用时: %lfs\n", result_2);

  pdebug("OpenBLAS: 开始计时\n");
  start = clock();
  // C = alpha * AB + beta * C
  cblas_dgemm(Order, TransA, TransB, M, N, K, 1, A->content, lda, B->content,
              ldb, 0, C[2]->content, ldc);
  end = clock();
  result_3 = (double)(end - start) / CLOCKS_PER_SEC;
  pdebug("OpenBLAS: 计算用时: %lfs\n", result_3);

  // 结果写入文件
  FILE* fp = fopen("results.txt", "w");
  if (!fp) {
    perror("Cannot open results.txt!!\n");
    return 1;
  }
  fprintf(fp, "%lf\n%lf\n%lf\n", result_1, result_2, result_3);
  pdebug("校验...\n");
  int is_ok[2] = {1, 1};
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