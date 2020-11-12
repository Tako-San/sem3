#include <math.h>
#include <time.h>
#include <complex.h>
#include "draw.h"


#define ITER_NUM 100


#define mod_sq( z ) ((creal(z) * creal(z)) + (cimag(z) * cimag(z)))

void mandel( int x, int y )
{
  double doub_x = 4 * (double)x / FW - 2;
  double doub_y = 4 * (double)y / FH - 2;

  double complex c = doub_x + I * doub_y;
  double complex z = 0 + I * 0;

  for (int k = 0; k < ITER_NUM; ++k)
  {
    z = z * z + c * sin(clock() / CLOCKS_PER_SEC);
    if (mod_sq(z) > 4)
    {
      pupi(x, y, k * k, 0, k);
      return;
    }
  }
  pupi(x, y, 255, 255, 255);
}

#undef mod_sq