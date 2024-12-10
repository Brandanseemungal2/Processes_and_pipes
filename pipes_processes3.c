#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/**
 * Executes the command "cat scores | grep Lakers".  In this quick-and-dirty
 * implementation the parent doesn't wait for the child to finish and
 * so the command prompt may reappear before the child terminates.
 *
 */

int main(int argc, char **argv)
{
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <grep_pattern>\n", argv[0]);
    return 1;
  }

  int pipe1[2], pipe2[2];
  char *cat_args[] = {"cat", "scores", NULL};
  char *grep_args[] = {"grep", "Lakers", NULL};
  char *sort_args[] = {"sort", NULL};

  if (pipe(pipe1) == 1 || pipe(pipe2) == -1) {
    perror("Pipe Failed");
    return 1;
  }

  pid_t p1 = fork();
  if (p1 < 0) {
    perror("Fork Failed");
    return 1;
  }

  if (p1 == 0) {
    dup2(pipe1[0], 0);
    dup2(pipe2[1], 1);
    close(pipe1[1]);
    close(pipe2[0]);
    execvp("grep", grep_args);
    perror("Exec Failed");
    exit(1);
  }

  pid_t p2 = fork();
  if (p2 < 0) {
    perror("Fork Failed");
    return 1;
  }

  if (p2 == 0) {
    dup2(pipe2[0], 0);
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[1]);
    execvp("sort", sort_args);
    perror("Exec Failed");
    exit(1);
  }

  dup2(pipe1[1], 1);
  close(pipe1[0]);
  close(pipe2[0]);
  close(pipe2[1]);
  execvp("cat", cat_args);
  perror("Exec Failed");
  return 1;
}
