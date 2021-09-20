#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cblas.h"
#include "mat.h"
#include "utils.h"

Mat *A_g, *B_g;

void do_calc(int task_id, int M, int N, Mat** C, const char* task_name,
             double* results, int result_tail) {
  struct timespec start, end;
  Mat* A = NULL;
  Mat* B = NULL;
  const int aligns[] = {0, 1, 1, 0};
  int aligned = aligns[task_id];
  A = mat_create(M, N, aligned);
  B = mat_create(N, M, aligned);
  *C = mat_create(A->w, B->h, aligned);
  mat_data_init(A);
  mat_data_init(B);
  mat_data_init(*C);
  for (int x = 0; x < M; x++) {
    for (int y = 0; y < N; y++) {
      // A->data[x][y] = (double)(rand() % 40) / 40;
      // B->data[x][y] = (double)(rand() % 40) / 40;
      // A->data[x][y] = 1;
      // B->data[x][y] = 1;
      A->data[x][y] = A_g->data[x][y];
      B->data[x][y] = B_g->data[x][y];
    }
  }
  pdebug("%10s计算: 开始计时\n", task_name);
  clock_gettime(CLOCK_REALTIME, &start);
  if (task_id == 0) {
    mat_mul(A, B, *C);
    if (mat_native_timeout) {
      pdebug("%s 超时！\n", task_name);
    }
  } else if (task_id == 1) {
    mat_mul_threaded(A, B, *C, 0);
  } else if (task_id == 2) {
    mat_mul_threaded(A, B, *C, 1);
  } else if (task_id == 3) {
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, A->w, B->h, A->h, 1,
                A->content, A->w, B->content, B->h, 0, (*C)->content, (*C)->h);
  }
  clock_gettime(CLOCK_REALTIME, &end);
  results[result_tail] = time_delta(start, end);
  pdebug("%10s: 计算用时: %3.3lfs\n", task_name, results[result_tail]);
}

int main(int argc, char** argv) {
  // 处理一下参数
  int K = 5;
  if (argc <= 1) {
    pdebug("使用默认: K = %d\n", K);
  } else {
    K = atoi(argv[1]);
    pdebug("使用: K = %d\n", K);
  }
  mat_native_time_limit = 150.0;
  int M = K, N = K;
  srand(time(NULL));

  A_g = mat_create(N, M, 0);
  B_g = mat_create(M, N, 0);
  mat_data_init(A_g);
  mat_data_init(B_g);
  for (int x = 0; x < M; x++) {
    for (int y = 0; y < N; y++) {
      // A->data[x][y] = (double)(rand() % 40) / 40;
      // B->data[x][y] = (double)(rand() % 40) / 40;
      // A->data[x][y] = 1;
      // B->data[x][y] = 1;
      A_g->data[x][y] = (double)(rand() % 4);
      B_g->data[x][y] = (double)(rand() % 4);
    }
  }

  Mat* C[32] = {NULL};
  int result_tail = 0;
  double results[32];

  // const char task_names[][64] = {"简单矩阵", "单线程矩阵优化",
  // "多线程矩阵优化", "OpenBLAS"};
  const char task_names[][64] = {"Native", "SIMD", "Threaded SIMD", "OpenBLAS"};

  const int task_number = 4;
  const int task_start = 0;

  for (int i = task_start; i < task_number; i++) {
    do_calc(i, M, N, &C[result_tail], task_names[i], results, result_tail);
    result_tail++;
  }

  // 结果写入文件
  FILE* fp = fopen("results.txt", "w");
  if (!fp) {
    printf("Cannot open results.txt!!\n");
    return 1;
  }
  // fprintf(fp, "%lf\n%lf\n%lf\n", result_1, result_2, result_3);
  for (int i = task_start; i < result_tail; i++) {
    if (i == 0 && mat_native_timeout) {
      fprintf(fp, "%s: %lf\n", task_names[i], -results[i]);
    } else {
      fprintf(fp, "%s: %lf\n", task_names[i], results[i]);
    }
  }
  fclose(fp);
  if (task_start == 0 && !mat_native_timeout) {
    pdebug("校验...\n");
    // mat_print(C[0]);
    // for (int k = 1; k < result_tail; k++) {
    //   printf("%s: \n", task_names[k]);
    //   mat_print(C[k]);
    // }
    int is_ok[32];
    for (int i = 0; i < result_tail; i++) {
      is_ok[i] = 1;
    }
    for (int x = 0; x < C[0]->w; x++) {
      for (int y = 0; y < C[0]->h; y++) {
        for (int k = 1; k < result_tail; k++) {
          if (is_ok[k]) {
            if (fabs(C[0]->data[x][y] - C[k]->data[x][y]) >= eps) {
              printf("K = %4d; %s计算错误! (%d, %d): [%lf, %lf]\n", K,
                     task_names[k], x, y, C[0]->data[x][y], C[k]->data[x][y]);
              is_ok[k] = 0;
            }
          }
        }
      }
    }
  }
  pdebug("DONE.\n");
  return 0;
}