#include "utils.h"
#include <malloc.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv) {
  int size = 0, rank = 0;
  const int block = 10;
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int processor_name_length = 0;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Get_processor_name(processor_name, &processor_name_length);
  double *data = NULL, *recv = NULL, *sum_all;
  data = (double *)malloc(sizeof(double) * size * block);
  recv = (double *)malloc(sizeof(double) * block);
  sum_all = (double *)malloc(sizeof(double) * size);
  if (rank == 0) {
    // 初始化数据
    srand(time(NULL));
    // srand(128);
    for (int i = 0; i < size * block; i++)
      data[i] = (double)rand() / 65535;
    // for (int i = 0; i < size * block; i++) data[i] = 1.0;
    printf("Running %d processes on %s, sending inital data...\n", size,
           processor_name);
  }
  // 对数据进行一个射的发和收的接
  MPI_Scatter(data, block, MPI_DOUBLE, recv, block, MPI_DOUBLE, 0,
              MPI_COMM_WORLD);
  double sum = 0;
  // root slot 同样参与运算
  // ~~if (rank != 0) {~~
  // 算出一个结果：求和，然后把结果返回到 root 线程
  for (int i = 0; i < block; i++)
    sum += recv[i];
  // 接收
  MPI_Gather(&sum, 1, MPI_DOUBLE, sum_all, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  if (rank == 0) {
    // 最终求和
    double sum_finally = 0;
    for (int i = 0; i < size; i++) {
      sum_finally += sum_all[i];
    }
    // Native 求和，检查是否一致
    double sum_native = 0;
    for (int i = 0; i < size * block; i++) {
      sum_native += data[i];
    }
    PD(sum_finally);
    PD(sum_native);
    if (sum_native - sum_finally < 1e-15) {
      puts("Data checked.");
    }
  }
  MPI_Finalize();
  return 0;
}