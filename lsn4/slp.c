#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_NUM 50

bool ac_check(int ac)
{
    if (ac < 2)
    {
        printf("Too few arguments\n");
        return false;
    }

    return true;
}

int main( int ac, char ** av)
{
    if (!ac_check(ac))
        return 1;

    for (int i = 1; i < ac; ++i)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("");
            return 1;
        }
        else if(pid == 0)
        {
            int num = strtol(av[i], NULL, 10);
            usleep(1000 * num);
            printf("%d ", num);
            return 0;
        }
    }

    for (int i = 1; i < ac; ++i)
    {
        int state = 0;
        if (wait(&state) < 0)
        {
            perror("");
            return 2;
        }
    }

    printf("\n");

    return 0;
}
