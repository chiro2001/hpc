#include <omp.h>

#include <cstdio>

int main(int argc, char **argv) {
#pragma omp parallel
  {
#pragma omp for
    for (int i = 0; i < 64; i++) printf("i = %d\n", i);
  }
  return 0;
}