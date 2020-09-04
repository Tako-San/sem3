#include <stdio.h>
#include <string.h>

void echo(int ac, char ** av);

int main(int ac, char ** av)
{
    echo(ac, av);
    return 0;
}

void echo(int ac, char ** av)
{
    int no_nl = 0;
    int i = 1;
    char endl[] = " ";

    if(ac < 2)
        return;

    if(!strcmp(av[i], "-n"))
        no_nl = 1, ++i;

    for( ; i < ac; i++)
    {
        (i == ac - 1 && no_nl) ? strcpy(endl, "") : strcpy(endl, "\n");
        printf("%s%s", av[i], endl);
    }
}