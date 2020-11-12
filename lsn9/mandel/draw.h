#ifndef SEM3_DRAW_H
#define SEM3_DRAW_H

#include <GL/glut.h>

#define WIDTH 1920
#define HEIGHT 1080

#define FW 200
#define FH 200

#define ZoomX 5
#define ZoomY 5

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
