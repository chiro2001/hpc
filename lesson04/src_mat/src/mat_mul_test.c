#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cblas.h"
#include "mat.h"

int main(int argc, char** argv) {
  const enum CBLAS_ORDER Order = CblasRowMajor;
  const enum CBLAS_TRANSPOSE TransA = CblasNoTrans;
  const enum CBLAS_TRANSPOSE TransB = CblasNoTrans;
  // 处理一下参数
  int K = 128;
  if (argc <= 1) {
    printf("使用默认: K = %d\n", K);
  } else {
    K = atoi(argv[1]);
    printf("使用: K = %d\n", K);
  }
  int M = K, N = K;
  int lda = K;  // A的列
  int ldb = N;  // B的列
  int ldc = N;  // C的列
  printf("初始化两个矩阵\n");
  Mat* A = mat_create(M, N);
  mat_data_init(A);
  Mat* B = mat_create(N, M);
  mat_data_init(B);
  for (int x = 0; x < M; x++) {
    for (int y = 0; y < N; y++) {
      A->data[x][y] = sin(x + y);
      B->data[x][y] = cos(x + y * 2);
    }
  }
  // 初始化一下 C，不把初始化时间计入计算时间
  printf("初始化 C\n");
  Mat* C = mat_create(M, M);
  mat_data_init(C);
  // 计算乘法，开始计时
  clock_t start, end;
  double result_1, result_2;
  printf("简单矩阵计算: 开始计时\n");
  start = clock();
  mat_mul(A, B, C);
  end = clock();
  result_1 = (double)(end - start) / CLOCKS_PER_SEC;
  printf("简单矩阵: 计算用时: %lfs\n", result_1);

  printf("OpenBLAS: 开始计时\n");
  start = clock();
  // C = alpha * AB + beta * C
  cblas_dgemm(Order, TransA, TransB, M, N, K, 1, A->content, lda, B->content,
              ldb, 0, C->content, ldc);
  end = clock();
  result_2 = (double)(end - start) / CLOCKS_PER_SEC;
  printf("OpenBLAS: 计算用时: %lfs\n", result_2);

  // 结果写入文件
  FILE *fp = fopen("results.txt", "w");
  if (!fp) {
    perror("Cannot open results.txt!!\n");
    return 1;
  }
  fprintf(fp, "%lf\n%lf\n", result_1, result_2);
  printf("DONE.\n");
  return 0;
}