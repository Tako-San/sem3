#include <stdio.h>
#include <pthread.h>
#include <GL/glut.h>
#include <time.h>

#include "draw.h"

typedef struct thread_info
{
  pthread_t thread_id;
  int thread_num;
  char * argv_string;
} thread_info;


typedef struct timespec timespec;

void print_time( timespec bgn, timespec end)
{
  long long t_n = (end.tv_sec - bgn.tv_sec) * 1e9 + end.tv_nsec - bgn.tv_nsec;
  double t = t_n / 1e9;
  printf("dt: %.06lfs\n", t);
}

int main( int argc, char ** argv )
{
  if (argc < 2)
  {
    printf("USAGE: ./mandel THR_NUMclock_gettime(CLOCK_MONOTONIC, &t_end); \n");
    return 0;
  }

  int m = atoi(argv[1]);
  thread_info tinfo[m];
  Borders b[m];

  for (int i = 0; i < m; ++i)
  {
    b[i].start = i * FH / m;
    b[i].stop = b[i].start + FH / m;
  }

  timespec t_bgn = {}, t_end = {};

  clock_gettime(CLOCK_MONOTONIC, &t_bgn);

  for (int i = 0; i < m; ++i)
    pthread_create(&tinfo[i].thread_id, NULL, fill_frame_thread, &b[i]);

  for (int i = 0; i < m; ++i)
  {
    int * res;
    pthread_join(tinfo[i].thread_id, NULL);
  }

  clock_gettime(CLOCK_MONOTONIC, &t_end);
  print_time(t_bgn, t_end);

  // drawing
  init(&argc, argv);
  fill_frame();
  glutDisplayFunc(on_display);
  glutMainLoop();
  return 0;
}

