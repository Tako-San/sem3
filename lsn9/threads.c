#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>

unsigned long long count = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef void * (* algo) ( void * );

typedef struct thread_info
{
  pthread_t thread_id;
  int thread_num;
  char * argv_string;
} thread_info;

void * naive_alg( void * arg )
{
  int num = *((int *) arg);
  for (int i = 0; i < num; ++i)
  {
    ++count;
    unsigned long long tmp = 0;
    for (int j = 0; j < 10000; ++j)
      ++tmp;
  }

  return &count;
}

void * fine_alg( void * arg )
{
  int num = *((int *) arg);
  for (int i = 0; i < num; ++i)
  {
    pthread_mutex_lock(&mutex);
    ++count;
    pthread_mutex_unlock(&mutex);
  }

  return &count;
}

void * global_alg( void * arg )
{
  int num = *((int *) arg);
  pthread_mutex_lock(&mutex);

  for (int i = 0; i < num; ++i)
    ++count;

  pthread_mutex_unlock(&mutex);

  return &count;
}

int main( int ac, char ** av )
{
  if (ac != 4)
  {
    printf("usage: ./a.out [NUMBER] [THREADNUM] [ALGONUM]\n");
    return 0;
  }

  int n   = atoi(av[1]),
      m   = atoi(av[2]),
      alg = atoi(av[3]);

  assert(m > 0 && n > 0 && alg >= 0);

  int num = n / m;

  algo a_ptr = NULL;

  switch (alg)
  {
    case 0:
      a_ptr = naive_alg;
      break;
    case 1:
      a_ptr = fine_alg;
      break;
    case 2:
      a_ptr = global_alg;
      break;
    default:
      printf("bie\n");
      exit(1);
  }

  thread_info tinfo[m];

  for (int i = 0; i < m; ++i)
    pthread_create(&tinfo[i].thread_id, NULL, a_ptr, &num);

  for (int i = 0; i < m; ++i)
  {
    int * res;
    pthread_join(tinfo[i].thread_id, (void *)&res);
  }


  printf("need: %d\n", n);
  printf("have: %d\n", count);

  return 0;
}

