#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>

void hello_handler( int arg )
{
  printf("Hello\n\n");
}

void resize_handler( int arg )
{
  struct winsize wdow;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &wdow);
  printf("cols = %d\nrows = %d\n\n", wdow.ws_col, wdow.ws_row);
}

int main( )
{
  signal(SIGINT, hello_handler);
  signal(SIGWINCH, resize_handler);
  while(pause());
  return 0;
}