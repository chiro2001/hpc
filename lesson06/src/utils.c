
#include "utils.h"

double time_delta(struct timespec time1, struct timespec time2) {
  return ((double)(time2.tv_sec - time1.tv_sec) * 1000 +
          (double)(time2.tv_nsec - time1.tv_nsec) / 1000000) /
         1000;
}
