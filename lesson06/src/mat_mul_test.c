#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "cblas.h"
#include "mat.h"
#include "utils.h"

Mat *A_g, *B_g;

void do_calc(int task_id, int M, int N, Mat **C, const char *task_name,
             double *results, int result_tail, int processor_number) {
  struct timespec start, end;
  Mat *A = NULL;
  Mat *B = NULL;
  int size = 0, rank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  const int aligns[] = {0, 0, 1, 1, 1, 1, 1, 1, 0, 1};
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
  if (rank == 0)
    pdebug("[%d]\t%s 计算: 开始计时\n", task_id, task_name);
  clock_gettime(CLOCK_REALTIME, &start);
  if (task_id == 0) {
    mat_mul(A, B, *C);
#ifdef USE_TIMEOUT_NATIVE
    if (mat_native_timeout) {
      pdebug("\t%s 超时！\n", task_name);
    }
#endif
  } else if (task_id == 1) {
    mat_mul_openmp_native(A, B, *C);
#ifdef USE_TIMEOUT_OPENMP
    if (mat_openmp_timeout) {
      pdebug("\t%s 超时！\n", task_name);
    }
#endif
  } else if (task_id == 2) {
    mat_mul_threaded(A, B, *C, 1, 0);
  } else if (task_id == 3) {
    mat_mul_threaded(A, B, *C, 1, 1);
  } else if (task_id == 4) {
    mat_mul_threaded(A, B, *C, processor_number, 0);
  } else if (task_id == 5) {
    mat_mul_threaded(A, B, *C, processor_number, 1);
  } else if (task_id == 6) {
    mat_mul_openmp(A, B, *C, 0);
  } else if (task_id == 7) {
    mat_mul_openmp(A, B, *C, 1);
  } else if (task_id == 8) {
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, A->w, B->h, A->h, 1,
                A->content, A->w, B->content, B->h, 0, (*C)->content, (*C)->h);
  } else if (task_id == 9) {
    mat_mul_mpi(A, B, *C, 0);
  }
  clock_gettime(CLOCK_REALTIME, &end);
  results[result_tail] = time_delta(start, end);
  if (rank == 0)
    pdebug("\t%s: 计算用时: %3.3lfs\n", task_name, results[result_tail]);
}

int main(int argc, char **argv) {
  int size = 0, rank = 0;
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int processor_name_length = 0;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Get_processor_name(processor_name, &processor_name_length);
  if (rank != 0) {
    pdebug("Slot %d\n", rank);
    // MPI_Finalize();
    // return 0;
  } else
    pdebug("Running on %s, total %d slot(s).\n", processor_name, size);
  // 处理一下参数
  int K = 4;
  const int task_number = 10;
  int task_start = 0;
  const int task_mpi_start = 9;
  if (argc <= 1) {
    // pdebug("使用默认: K = %d\n", K);
  } else {
    K = atoi(argv[1]);
    if (argc <= 2) {
      // PINT(task_start);
    } else {
      task_start = atoi(argv[2]);
    }
  }
  mat_native_time_limit = 10.0;
  mat_openmp_time_limit = 10.0;
  int M = K, N = K;
  srand(time(NULL) + rank);

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

  Mat *C[32] = {NULL};
  double results[32];

  const char task_names[][64] = {"Native",        "OpenMP",
                                 "SIMD",          "Unrolling SIMD",
                                 "Threaded SIMD", "Unrolling Threaded SIMD",
                                 "OpenMP SIMD",   "OpenMP Unrolling SIMD",
                                 "OpenBLAS",      "MPI",
                                 "MPI SIMD",      "MPI Unrolling SIMD"};

  int processor_number = sysconf(_SC_NPROCESSORS_ONLN);
  if (rank == 0) {
    pdebug("Running with %d cores.\n", processor_number);
    pdebug("使用: K = %d\n", K);
    pdebug("使用: task_start = %d [%s]\n", task_start, task_names[task_start]);
    for (int i = task_start; i < task_number; i++) {
      do_calc(i, M, N, &C[i], task_names[i], results, i, processor_number);
    }
  } else {
    for (int i = task_mpi_start; i < task_number; i++) {
      do_calc(i, M, N, &C[i], task_names[i], results, i, processor_number);
    }
    pdebug("Slot %d calc done.\n", rank);
    MPI_Finalize();
    return 0;
  }

  // 结果写入文件
  FILE *fp = NULL;
  if (rank == 0) {
    fp = fopen("results.txt", "w");
    if (!fp) {
      printf("Cannot open results.txt!!\n");
      return 1;
    }
  }
  if (fp) {
    for (int i = task_start; i < task_number; i++) {
      if (i == 0 && mat_native_timeout) {
        fprintf(fp, "%s: %lf\n", task_names[i], -results[i]);
      } else {
        fprintf(fp, "%s: %lf\n", task_names[i], results[i]);
        // printf("%s: %lf\n", task_names[i], results[i]);
      }
    }
    fclose(fp);
  }
  if (rank == 0) {
    if (task_start == 0 && !mat_native_timeout) {
      pdebug("校验...\n");
      int is_ok[32];
      for (int i = 0; i < task_number; i++) {
        is_ok[i] = 1;
      }
      for (int x = 0; x < C[0]->w; x++) {
        for (int y = 0; y < C[0]->h; y++) {
          for (int k = task_start; k < task_number; k++) {
            if (is_ok[k]) {
              if (fabs(C[0]->data[x][y] - C[k]->data[x][y]) >= eps) {
                printf("K = %4d; %s 计算错误! (%d, %d): [%lf, %lf]\n", K,
                       task_names[k], x, y, C[0]->data[x][y], C[k]->data[x][y]);
                is_ok[k] = 0;
              }
            }
          }
        }
      }
      int is_ok_all = 1;
      for (int i = task_start; i < task_number; i++)
        if (!is_ok[i])
          is_ok_all = 0;
      // if (!is_ok_all) {
      //   puts("[0]\t参考: ");
      //   mat_print(C[0]);
      //   for (int k = task_start; k < task_number; k++) {
      //     if (!is_ok[k]) {
      //       printf("[%d]\t%s: \n", k, task_names[k]);
      //       mat_print(C[k]);
      //     }
      //   }
      // }
    }
  }
  pdebug("DONE.\n");
  MPI_Finalize();
  return 0;
}