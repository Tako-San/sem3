#include <stdio.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>

#include <unistd.h>
#include <time.h>

typedef struct timespec timespec;

void print_time( timespec bgn, timespec end)
{
  long long t_n = (end.tv_sec - bgn.tv_sec) * 1e9 + end.tv_nsec - bgn.tv_nsec;
  double t = t_n / 1e9;
  printf("dt: %.06lfs\n", t);
}

#define ERR_CHECK(what) \
  if (what < 0)         \
  {                     \
    perror(#what);      \
    return errno;       \
  }                     \

int main( int ac, char ** av )
{
  if (ac < 2)
  {
    printf("Too few arguments\n");
    return 1;
  }

  struct timespec t_bgn = {}, t_end = {};

  clock_gettime(CLOCK_MONOTONIC, &t_bgn);

  pid_t pid = fork();

  ERR_CHECK(pid);
  if(pid == 0)
  {
    execvp(av[1], av + 1);
    return 0;
  }

  ERR_CHECK(wait(NULL));

  clock_gettime(CLOCK_MONOTONIC, &t_end);

  print_time(t_bgn, t_end);

  return 0;
}

#undef ERR_CHECK
