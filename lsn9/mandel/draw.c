#include "draw.h"
#include "mandel.h"

Byte frame[FH][FW][3] = {};

Byte clamp( Byte num, Byte min, Byte max )
{
  if (num < min)
    return min;
  if (num > max)
    return max;
  return num;
}

void pupi( int x, int y, Byte r, Byte g, Byte b )
{
  r = clamp(r, 0, 255);
  g = clamp(g, 0, 255);
  b = clamp(b, 0, 255);

  // x = clamp(x, 0, FW);
  // y = clamp(y, 0, FH);

  frame[y][x][0] = b;
  frame[y][x][1] = g;
  frame[y][x][2] = r;
}

void fill_frame( )
{
  for (int i = 0; i < FH; ++i)
    for (int j = 0; j < FW; ++j)
      mandel(j, i);
}

void * fill_frame_thread( void * arg )
{
  Borders * b = (Borders *)arg;

  for (int i = b->start; i < b->stop; ++i)
    for (int j = 0; j < FW; ++j)
      mandel(j, i);
}

void on_display( )
{
  glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  glColor3b(0, 0, 0);
  glLineWidth(1);

  glRasterPos2f(-1, -1);

  glPixelZoom(ZoomX, ZoomY);

  glDrawPixels(FW, FH, GL_BGR_EXT, GL_UNSIGNED_BYTE, frame);

  glFinish();
  glutPostRedisplay();
  glutSwapBuffers();
}

void on_keyboard( Byte key, int x, int y )
{
  if (key == 27)
    exit(0);
}


void init( int * argc, char ** argv )
{
  glutInit(argc, argv);
  glutInitWindowSize(WIDTH, HEIGHT);
  glutInitWindowPosition(0, 0);
  glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);

  glutCreateWindow("Mandel");

  glutKeyboardFunc(on_keyboard);
}