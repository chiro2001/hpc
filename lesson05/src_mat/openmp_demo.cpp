#include <cstdio>
#include <omp.h>

int main(int argc, char **argv) {
  #pragma omp parallel
  printf("Hello, OpenMP!\n");
  return 0;
}