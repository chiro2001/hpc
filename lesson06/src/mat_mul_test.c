#include "utils.h"

int main(int argc, char **argv) {
  int size = 0, rank = 0;
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int processor_name_length = 0;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Get_processor_name(processor_name, &processor_name_length);
  if (rank == 0)
    pdebug("Running on %s, total %d slot(s).\n", processor_name, size);
  // 处理一下参数
  int K = 4;
  int task_start = 0;
  int task_end = MAT_TASK_SIZE;
  if (argc <= 1) {
    // pdebug("使用默认: K = %d\n", K);
  } else {
    K = atoi(argv[1]);
    if (K < 0) {
      pdebug("Return Slot numbers: %d\n", size);
      MPI_Finalize();
      return size;
    } else if (K == 0) {
      printf("%s 0\t\t: help\n"
             "%s K [start] [end]\t: Run task from start to end\n"
             "%s K [-id]\t\t: Run task id\n"
             "%s -1\t\t: Get slots size\n",
             argv[0], argv[0], argv[0], argv[0]);
      MPI_Finalize();
      return 0;
    }
    if (argc <= 2) {
      // PINT(task_start);
    } else {
      task_start = atoi(argv[2]);
      if (argc <= 3) {
        // pass
      } else {
        task_end = atoi(argv[3]) + 1;
        task_end = task_end < MAT_TASK_SIZE ? task_end : MAT_TASK_SIZE;
        task_end = task_end > task_start ? task_end : task_start + 1;
        PINT(task_end);
      }
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
      A_g->data[x][y] = (double)(rand() % 4) + 1.0;
      B_g->data[x][y] = (double)(rand() % 4) + 1.0;
    }
  }

  TaskInfo **tasks = malloc(sizeof(TaskInfo *) * (MAT_TASK_SIZE + 1));
  int tasks_tail = 0;

  int processor_number = sysconf(_SC_NPROCESSORS_ONLN);
  if (task_start >= 0) {
    if (rank == 0) {
      TaskInfo *task_info_start = task_find_by_id(task_start);
      if (!task_info_start) {
        printf("Unkown task_id: %d\n", task_start);
        MPI_Finalize();
        return 1;
      }
      pdebug("Running with %d cores.\n", processor_number);
      pdebug("使用: K = %d\n", K);
      pdebug("使用: task_start = %d [%s]\n", task_start, task_info_start->name);
      pdebug("使用: task_end-1 = %d [%s]\n", task_end - 1,
             task_find_by_id(task_end - 1)->name);
      // 生成需要执行的 tasks。
      for (int i = task_start; i < task_end; i++) {
        TaskInfo *task = task_find_by_id(i);
        if (!task)
          continue;
        tasks[tasks_tail++] = task;
      }
    } else {
      for (int i = task_start; i < task_end; i++) {
        TaskInfo *task = task_find_by_id(i);
        if (!task)
          continue;
        if (!task->is_mpi)
          continue;
        tasks[tasks_tail++] = task;
      }
    }
  } else {
    // task_start < 0 表示指定只运行某一 TASK
    int task_id = -task_start;
    TaskInfo *task = task_find_by_id(task_id);
    if (!task) {
      printf("Unkown task_id: %d\n", task_id);
      MPI_Finalize();
      return 1;
    }
    if (rank == 0) {
      tasks[tasks_tail++] = task;
    } else {
      if (task->is_mpi) {
        tasks[tasks_tail++] = task;
      }
    }
  }

  // 执行 tasks
  for (int i = 0; i < tasks_tail; i++) {
    do_calc(M, N, &tasks[i]->C, tasks[i], processor_number);
  }

  if (rank != 0) {
    // pdebug("Slot %d calc done.\n", rank);
    // system("sleep 3");
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
    for (int i = 0; i < tasks_tail; i++) {
      TaskInfo *task = tasks[i];
      if (i == 0 && mat_native_timeout) {
        fprintf(fp, "%s: %lf\n", task->name, -task->time);
      } else {
        fprintf(fp, "%s: %lf\n", task->name, task->time);
        // pdebug("%s: %lf\n", task->name, task->time);
      }
    }
    fclose(fp);
  }
  if (rank == 0) {
    TaskInfo *native = NULL;
    for (int i = 0; i < tasks_tail; i++)
      if (tasks[i]->task_id == 0) {
        native = tasks[i];
        break;
      }
    if (native != NULL && tasks_tail >= 2 && !mat_native_timeout) {
      pdebug("校验...\n");
      for (int i = 0; i < tasks_tail; i++)
        tasks[i]->checked = 1;
      int is_ok_all = 1;
      for (int x = 0; x < native->C->w; x++) {
        for (int y = 0; y < native->C->h; y++) {
          for (int k = 0; k < tasks_tail; k++) {
            TaskInfo *task = tasks[k];
            if (task->task_id == 0)
              continue;
            if (task->checked) {
              if (fabs(native->C->data[x][y] - task->C->data[x][y]) >= eps) {
                printf("K = %4d; %s 计算错误! (%d, %d): [%lf, %lf]\n", K,
                       task->name, x, y, native->C->data[x][y],
                       task->C->data[x][y]);
                task->checked = 0;
                is_ok_all = 0;
              }
            }
          }
        }
      }
      if (!is_ok_all) {
        pdebug("[0]\t参考: \n");
        mat_print(native->C);
        for (int k = 0; k < tasks_tail; k++) {
          if (!tasks[k]->checked) {
            pdebug("[%d]\t%s: \n", tasks[k]->task_id, tasks[k]->name);
            mat_print(tasks[k]->C);
          }
        }
      }
    }
  }
  pdebug("DONE.\n");
  MPI_Finalize();
  return 0;
}