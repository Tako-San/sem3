#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

void border( int sig )
{
    printf("\e[H\e[J");

    struct winsize wsz;
    ioctl(0, TIOCGWINSZ, &wsz);

    for (int i = 0; i < wsz.ws_col; i++)
        putchar('*');

    for (int i = 1; i < wsz.ws_row - 2; i++)
    {
        putchar('*');
        for (int j = 1; j < wsz.ws_col - 1; j++)
            putchar(' ');
        putchar('*');
    }

    for (int i = 0; i < wsz.ws_col - 1; i++)
        putchar('*');
}

int main( )
{
    border(0);
    signal(SIGWINCH, border);
    while (pause());
}

