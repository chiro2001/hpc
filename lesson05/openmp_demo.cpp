#include <cstdio>
#include <omp.h>

int main(int argc, char **argv) {
  // #pragma omp parallel
  // printf("Hello, OpenMP!\n");
  #pragma omp parallel
  #pragma omp for
  for (int i = 0; i < 100; i++) {
      printf("count = %d\n", i);
  }
  return 0;
}
