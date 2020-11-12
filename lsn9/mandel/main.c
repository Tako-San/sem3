#include <pthread.h>
#include <GL/glut.h>
#include "draw.h"

typedef struct thread_info
{
  pthread_t thread_id;
  int thread_num;
  char * argv_string;
} thread_info;

int main( int argc, char ** argv )
{
  init(&argc, argv);

  int m = 10;
  thread_info tinfo[m];
  Borders b[m];

  for (int i = 0; i < m; ++i)
  {
    b[i].start = i * FH / m;
    b[i].stop = b[i].start + FH / m;
    pthread_create(&tinfo[i].thread_id, NULL, fill_frame_thread, &b[i]);
  }

  for (int i = 0; i < m; ++i)
  {
    int * res;
    pthread_join(tinfo[i].thread_id, (void *)&res);
  }

  // fill_frame();

  glutDisplayFunc(on_display);

  glutMainLoop();
  return 0;
}

