//
// Created by Chiro on 2021/10/10.
//

#include "utils.h"

TaskInfo tasks_info[] = {{0, "$Native$", 0, 0},
                         {1, "$OpenMP_{Native}$", 0, 0},
                         {2, "$SIMD$", 1, 0},
                         {3, "$SIMD_{Unrolling}$", 1, 0},
                         {4, "$SIMD_{Thearded}$", 1, 0},
                         {5, "$SIMD_{Unrolling Thearded}$", 1, 0},
                         {6, "$OpenMP_{SIMD}$", 1, 0},
                         {7, "$OpenMP_{Unrolling SIMD}$", 1, 0},
                         {8, "$OpenBLAS$", 0, 0},
                         {9, "$MPI_{Native}$", 1, 1},
                         {10, "$MPI_{SIMD}$", 1, 1},
                         {11, "$MPI_{Unrolling SIMD}$", 1, 1}};

TaskInfo *task_find_by_id(int task_id) {
  for (int i = 0; i < MAT_TASK_SIZE; i++) {
    if (tasks_info[i].task_id == task_id) return &tasks_info[i];
  }
  return NULL;
}

Mat *A_g, *B_g;

void do_calc(int M, int N, Mat **C, TaskInfo *task, int processor_number) {
  struct timespec start, end;
  Mat *A = NULL;
  Mat *B = NULL;
  int size = 0, rank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0 && (!A_g || !B_g)) return;
  int task_id = task->task_id;
  int aligned = task->aligned;
  A = mat_create(M, N, aligned);
  B = mat_create(N, M, aligned);
  mat_data_init(A);
  mat_data_init(B);

  if (rank == 0) {
    *C = mat_create(A->w, B->h, aligned);
    mat_data_init(*C);
    for (int x = 0; x < M; x++) {
      for (int y = 0; y < N; y++) {
        if (rank == 0) {
          A->data[x][y] = A_g->data[x][y];
          B->data[x][y] = B_g->data[x][y];
        } else {
          A->data[x][y] = 0;
          B->data[x][y] = 0;
        }
      }
    }
  }

  if (rank == 0) pdebug("[%d]\t%s 计算: 开始计时\n", task->task_id, task->name);
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
    mat_mul_mpi_all(A, B, *C, 0, 1);
  } else if (task_id == 10) {
    mat_mul_mpi_all(A, B, *C, 0, 0);
  } else if (task_id == 11) {
    mat_mul_mpi_all(A, B, *C, 1, 0);
  }
  clock_gettime(CLOCK_REALTIME, &end);
  task->time = time_delta(start, end);
  if (rank == 0) pdebug("\t%s: 计算用时: %3.3lfs\n", task->name, task->time);
}
