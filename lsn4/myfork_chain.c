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


#define FORK_N_CHECK()  \
    pid = fork();       \
    if (pid < 0)        \
    {                   \
        perror("");     \
        return 1;       \
    }                   \

int main(int ac, char ** av)
{
    if (!ac_check(ac))
        return 1;

    unsigned N = strtol(av[1], NULL, 10);

    pid_t pid;
    printf("PARENT: %d\n", getpid());
    FORK_N_CHECK();

    for (unsigned i = 0; i < N && pid == 0; ++i)
    {
        printf("CHILD:  %d %d\n", getpid(), getppid());
        FORK_N_CHECK();
    }

    wait(NULL);

    return 0;
}

#undef FORK_N_CHECK