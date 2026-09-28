#include <stdio.h>     // printf
#include <sys/types.h> // pid_t
#include <unistd.h>    // fork, getpid, getppid
int main() {
  int value = fork();
  if (value == 0) {
    int pid_value = getpid();
    printf("%d", execl("/ bin / ls", "-alh"));

  }

  else {
    int new_pid = getppid();
    printf("%d", execl("/ bin / ls", "-a"));
  }
}
