#include <time.h>

#define DEBUG

#ifdef DEBUG
#define pdebug(...) printf(__VA_ARGS__)
#else
#define pdebug(...)
#endif

#define eps 1e-4

double time_delta(struct timespec time1, struct timespec time2);

#define PINT(x) printf(#x "\t= %d\n", (int)x)
#define PD(x) printf(#x "\t= %lf\n", (double)x)
#define PD4(x)                                                               \
  printf("\t" #x "\t= %2.2lf, %2.2lf, %2.2lf, %2.2lf\n", ((double*)(&x))[0], \
         ((double*)(&x))[1], ((double*)(&x))[2], ((double*)(&x))[3])
