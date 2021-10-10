//
// Created by Chiro on 2021/10/10.
//

#ifndef MAT_MUL_TEST_TASK_H
#define MAT_MUL_TEST_TASK_H

#include "mat.h"

typedef struct {
  int task_id;
  char name[64];
  int aligned;
  int is_mpi;
  double time;
  Mat *C;
  int checked;
} TaskInfo;

#define MAT_TASK_SIZE 12

extern TaskInfo tasks_info[MAT_TASK_SIZE];

TaskInfo *task_find_by_id(int task_id);
void do_calc(int M, int N, Mat **C, TaskInfo *task, int processor_number);

extern Mat *A_g, *B_g;

#endif // MAT_MUL_TEST_TASK_H
