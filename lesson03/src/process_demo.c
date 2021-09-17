#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int glob = 6;
int main() {
  pid_t pid;
  int x = 1;

  pid = fork();
  if (pid < 0) {
    perror("Error: ");
  } else if (pid == 0) {
    sleep(1);
		glob = glob + 2;
    x = x + 2;
    printf("child : glob(%p)=%d, x(%p)=%d\n", &glob, glob, &x, x);
  } else {
    sleep(2);
		glob = glob + 1;
    x = x + 1;
    printf("parent: glob(%p)=%d, x(%p)=%d\n", &glob, glob, &x, x);
  }
}

/*
parent: glob(0x555555558010)=7, x(0x7fffffffd580)=2
child : glob(0x555555558010)=8, x(0x7fffffffd580)=3
*/

/*
child : glob(0x555555558010)=8, x(0x7fffffffd580)=3
parent: glob(0x555555558010)=7, x(0x7fffffffd580)=2
*/