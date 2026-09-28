#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {

  for (int i = 0; i < 5; i++) {
    printf("sleeping \n");
    sleep(1);
    fork();
  }

  printf("done \n");
}
