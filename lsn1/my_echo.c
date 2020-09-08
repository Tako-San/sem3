#include <stdio.h>
#include <string.h>

void echo( int ac, char ** av )
{
    int no_nl = 0, i = 1;

    if (ac < 2)
    {
        printf("\n");
        return;
    }

    if (strcmp(av[i], "-n") == 0)
    {
        no_nl = 1;
       	++i;
    }

    for ( ; i < ac; i++)
        printf("%s%s", av[i], (i == ac - 1) ? "" : " ");

    printf("%s", no_nl ? "" : "\n");
}

int main( int ac, char ** av )
{
    echo(ac, av);
    return 0;
}

