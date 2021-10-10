#include <math.h>
#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "cblas.h"
#include "mat.h"
#include "task.h"

#define DEBUG

#ifdef DEBUG
#define pdebug(...) printf(__VA_ARGS__)
#else
#define pdebug(...)
#endif

#define eps 1e-4

// #define USE_TIMEOUT_NATIVE
// #define USE_TIMEOUT_OPENMP

double time_delta(struct timespec time1, struct timespec time2);

#define PINT(x) pdebug(#x "\t= %d\n", (int)(x))
#define PD(x) pdebug(#x "\t= %lf\n", (double)(x))
#define PD4(x)                                                                 \
  printf("\t" #x "\t= %2.2lf, %2.2lf, %2.2lf, %2.2lf\n",                       \
         ((double *)(&(x)))[0], ((double *)(&(x)))[1], ((double *)(&(x)))[2],  \
         ((double *)(&(x)))[3])
