#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>

int main(int ac, char ** av)
{
    if (ac > 2)
    {
        printf("Too many arguments\n");
        return 1;
    }
    else if (ac < 2)
    {
        printf("Too few arguments\n");
        return 2;
    }

    unsigned N = strtol(av[1], NULL, 10);

    printf("PARENT: %d\n", getpid());


    for (unsigned i = 0; i < N; ++i)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("");
            return 3;
        }
        else if (pid == 0)
        {
            printf("CHILD:  %d %d\n", getpid(), getppid());
            return 0;
        }
    }

    return 0;
}