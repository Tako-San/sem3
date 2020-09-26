#include <stdio.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

typedef struct timespec timespec;

bool ac_check(int ac)
{
    if (ac < 2)
    {
        printf("Too few arguments\n");
        return false;
    }

    return true;
}

void print_time( timespec bgn, timespec end)
{
    long long t_n = (end.tv_sec - bgn.tv_sec) * 1e9 + end.tv_nsec - bgn.tv_nsec;
    double t = t_n / 1e9;
    printf("dt: %.03lfs\n", t);
}


int main( int ac, char ** av )
{
    if (!ac_check(ac))
        return 1;

    struct timespec t_bgn = {},
                    t_end = {};

    clock_gettime(CLOCK_MONOTONIC, &t_bgn);

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("");
        return 1;
    }
    else if(pid == 0)
    {
        execvp(av[1], av + 1);
        return 0;
    }

    if (wait(NULL) < 0)
    {
        perror("");
        return 1;
    }

    clock_gettime(CLOCK_MONOTONIC, &t_end);

    print_time(t_bgn, t_end);
    return 0;
}