#define _GNU_SOURCE

#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

int main( int ac, char ** av )
{
  int pipefd[2] = {};

  if (pipe2(pipefd, 0) < 0)
  {
    perror("Pipe open error");
    exit(1);
  }

  return 0;
}

