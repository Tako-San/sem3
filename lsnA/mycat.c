#include <stdio.h>

int main( int argc, char ** argv )
{
  return 0;
}

/*
 * задача - написать mycat.
 * чтение должно выполнятся в одном процессе
 * запись в другом
 * передачу данных организовать посредством сигналов
 *
 * man:
 *   signal
 *   sigaction
 *   sigsuspend
 *   sigprocmask
 *   kill
 *   sigaddset/sigemptyset
 */