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
    char end_l[2] = " ";

    if(!strcmp(av[i], "-n"))
    {
        no_nl = 1;
        i++;
    }

    for( ; i < ac; i++)
    {
        if(i == ac - 1)
            if (no_nl)
                strcpy(end_l, "");
            else
                strcpy(end_l, "\n");

        printf("%s%s", av[i], end_l);
    }
}