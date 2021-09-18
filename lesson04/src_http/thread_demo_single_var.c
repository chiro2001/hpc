#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int glob = 6;
void increse_num(int *arg);

int main() {
  int ret;
  pthread_t thrd1, thrd2, thrd3;
  int flag = 1;
  ret = pthread_create(&thrd1, NULL, (void *)increse_num, &flag);
  sleep(1);  //如果无sleep(1)结果会有什么不同？
  flag = 2;
  ret = pthread_create(&thrd2, NULL, (void *)increse_num, &flag);
  pthread_join(thrd1, NULL);
  pthread_join(thrd2, NULL);
  flag = 0;
  increse_num(&flag);  //如果放在pthread_join之前结果会怎样？
  return 0;
}

void increse_num(int *arg) {
  int flag = *arg;
  if (flag == 0) {
    glob = glob + 1;
    // sleep(10);  //如果都加了sleep，输出的结果有什么不同？
    printf("parent %d: glob=%d\n", flag, glob);
  } else if (flag == 1) {
    glob = glob + 2;
    // sleep(10);
    printf("child %d: glob=%d\n", flag, glob);
  } else {
    glob = glob + 3;
    printf("child %d: glob=%d\n", flag, glob);
  }
}
