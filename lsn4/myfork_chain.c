#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

bool ac_check(int ac)
{
    if (ac > 2)
    {
        printf("Too many arguments\n");
        return false;
    }
    else if (ac < 2)
    {
        printf("Too few arguments\n");
        return false;
    }
    return true;
}

int main(int ac, char ** av)
{
    if (!ac_check(ac))
        return 1;

    unsigned N = strtol(av[1], NULL, 10);

    printf("PARENT: %d\n", getpid());
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("");
        return 3;
    }

    for (unsigned i = 0; i < N && pid == 0; ++i)
    {
        printf("CHILD:  %d %d\n", getpid(), getppid());
        pid = fork();
        if (pid < 0)
        {
            perror("");
            return 3;
        }
    }

    wait(NULL);

    return 0;
}