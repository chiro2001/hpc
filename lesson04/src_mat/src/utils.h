#include <time.h>

// #define DEBUG

#ifdef DEBUG
#define pdebug(...) printf(__VA_ARGS__)
#else
#define pdebug(...)
#endif

#define eps 1e-4

double time_delta(struct timespec time1, struct timespec time2);
