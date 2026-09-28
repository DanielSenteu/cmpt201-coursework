#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void run_command(char *cmd) {
  execl(cmd, cmd, (char *)NULL);
  perror("execl fail");
  free(cmd);
  exit(EXIT_FAILURE);
}

int main(void) {
  char *cmd = NULL;
  size_t size = 0;
  int count = 0;

  while (1) {
    printf("%d Enter a command path:\n", count + 1);
    ssize_t nread = getline(&cmd, &size, stdin);
    if (nread == -1) {
      perror("getline");
      break;
    }

    if (nread > 0 && cmd[nread - 1] == '\n')
      cmd[nread - 1] = '\0';

    if (cmd[0] == '\0')
      continue;

    pid_t child = fork();
    if (child == -1) {
      perror("fork");
      continue;
    }
    if (child == 0)
      run_command(cmd);

    int status = 0;
    if (waitpid(child, &status, 0) == -1) {
      perror("waitpid");
      free(cmd);
      exit(EXIT_FAILURE);
    }
    count++;
    if (WIFEXITED(status))
      printf("Child %d finished. (run number %d) \n", child, count);
  }
  free(cmd);
  return 0;
}
