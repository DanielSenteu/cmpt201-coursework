#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void) {
  char *message_line = NULL;
  size_t count = 0;

  printf("Please Enter Some Text \n");
  getline(&message_line, &count, stdin);

  char *current;
  char *stripped_lines = strtok_r(message_line, " ", &current);

  while (stripped_lines != NULL) {
    printf("%s \n", stripped_lines);
    stripped_lines = strtok_r(NULL, " ", &current);
  }

  free(message_line);
  return 0;
}
