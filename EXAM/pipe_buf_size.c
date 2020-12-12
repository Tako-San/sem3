#define _GNU_SOURCE

#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

int main( )
{
  int pipefd[2] = {};

  if (pipe2(pipefd, 0) < 0)
  {
    perror("Pipe open error\n");
    exit(1);
  }

  int counter;
  char x = 'x';

  for (counter = 0; write(pipefd[0], &x, 1) > 0; ++counter);

  printf("Pipe size: %d\n", counter);

  return 0;
}

