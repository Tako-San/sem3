#define _GNU_SOURCE

#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <poll.h>

int main( )
{
  int pipefd[2] = {};

  if (pipe2(pipefd, 0) < 0)
  {
    perror("Pipe open error\n");
    exit(1);
  }

  struct pollfd fds = {pipefd[1]};

  int counter;

  for (counter = 0; poll(&fds, 1, -1); ++counter)
    write(pipefd[1], "x", 1);

  printf("Pipe size: %d bytes\n", counter);

  return 0;
}

