#define _GNU_SOURCE

#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <poll.h>

int count = 1;
int flag  = 0;


typedef struct thread_info
{
  pthread_t thread_id;
  int thread_num;
  char * argv_string;
} thread_info;

int pipefd[2] = {};
char buf[66000];

void * thread_writer( void * input )
{
  while(write(pipefd[1], buf, count) > 0)
  {
    flag = 1;
    read(pipefd[0], buf, count);
    flag = 0;
    ++count;
  }

  return 0;
}

void * thread_observer( void * input )
{
  while (usleep(2000000))
    if (flag == 0)
      break;
  return 0;
}

int main( )
{
  if (pipe2(pipefd, 0) < 0)
  {
    perror("Pipe open error\n");
    exit(1);
  }

  thread_info tinfo[2];

  pthread_create(&tinfo[0].thread_id, NULL, thread_writer, NULL);
  pthread_create(&tinfo[1].thread_id, NULL, thread_observer, NULL);

  pthread_join(tinfo[1].thread_id, NULL);

  printf("Pipe size: %d bytes\n", count);

  return 0;
}


