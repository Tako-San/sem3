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

  struct pollfd fds = {pipefd[1], POLLOUT};

  int counter;

  for (counter = 0; poll(&fds, 1, 42) > 0; ++counter)
    write(pipefd[1], "x", 1);

  printf("Pipe size: %d bytes\n", counter + 4095);

  return 0;
}

