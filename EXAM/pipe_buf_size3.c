#define _GNU_SOURCE

#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>


typedef struct thread_info
{
  pthread_t thread_id;
  int thread_num;
  char * argv_string;
} thread_info;

int pipefd[2] = {};

void * thread_writer( void * input )
{
  while(write(pipefd[0], "x", 1) == 1)
    printf("w\n");
  return 0;
}

void * thread_reader( void * input )
{
  int count;
  for (count = 0; read(pipefd[1], NULL, 1) > 0; ++count)
    printf("r\n");
  return (void *)count;
}

int main( )
{
  if (pipe2(pipefd, 0) < 0)
  {
    perror("Pipe open error\n");
    exit(1);
  }

  int counter;

  thread_info tinfo[2];

  pthread_create(&tinfo[0].thread_id, NULL, thread_writer, NULL);
  pthread_create(&tinfo[1].thread_id, NULL, thread_reader, NULL);

  int * ans;
  pthread_join(tinfo[0].thread_id, NULL);
  pthread_join(tinfo[1].thread_id, (void **)&ans);

  printf("Pipe size: %d bytes\n", *ans);

  return 0;
}


