#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define ERR_CHECK(what) \
    if (what < 0)       \
    {                   \
        perror(#what);  \
        return 1;       \
    }                   \


int main( int ac, char ** av)
{
    if (ac < 2)
    {
        printf("Too few arguments\n");
        return 0;
    }

    for (int i = 1; i < ac; ++i)
    {
        pid_t pid = fork();
        ERR_CHECK(pid);

        if(pid == 0)
        {
            int num = strtol(av[i], NULL, 10);
            usleep(1000 * num);
            printf("%d ", num);
            return 0;
        }
    }

    for (int i = 1; i < ac; ++i)
        ERR_CHECK(wait(NULL));

    printf("\n");

    return 0;
}

#undef ERR_CHECK