#ifndef SEM3_DRAW_H
#define SEM3_DRAW_H

#include <GL/glut.h>

#define WIDTH 1920
#define HEIGHT 1080

#define FW 1920
#define FH 1080

#define ZoomX 1
#define ZoomY 1

typedef unsigned char Byte;

typedef struct Borders
{
  int start;
  int stop;
} Borders;

Byte clamp( Byte num, Byte min, Byte max );

void pupi( int x, int y, Byte r, Byte g, Byte b );

void on_keyboard( Byte key, int x, int y );

void fill_frame( );

void * fill_frame_thread( void * arg );

void on_display( );

void init( int * argc, char ** argv );

#endif //SEM3_DRAW_H
