#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/wait.h>

#define MAX_MEM_SIZE 4096
#define MAX_ACCESS   0777

#define SHORT_OPTS   "w"

enum FLAGS
{
    WC = 0x1
};

enum OP_CODE
{
    END         = -1,
    UNKNOWN     = -2
};

struct option OPTS[] =
        {
                {"wc", 0, NULL, 'w'},
                {0, 0, 0, 0}
        };

int read_opts(int ac, char ** av);

ssize_t safe_write( int fd, const void * buf, size_t count );
bool read_n_write( int fd_in, int fd_out, char * buffer );

#define ERR_CHECK(what)      \
    if (what < 0)            \
    {                        \
        perror(#what);       \
        exit(EXIT_FAILURE);  \
    }                        \

int main( int ac, char ** av )
{
    int pipefd[2];

    if (ac < 2)
    {
        fprintf(stderr, "Too few arguments\n");
        exit(EXIT_FAILURE);
    }

    int opts = read_opts(ac, av);

    if (opts == UNKNOWN)
        exit(EXIT_FAILURE);
    else if (opts & WC)
    {
        ERR_CHECK(pipe(pipefd));

        pid_t cpid = fork();
        ERR_CHECK(cpid);

        if(cpid == 0)
        {
            dup2(STDOUT_FILENO, pipefd[1]);
            close(pipefd[1]);
            close(pipefd[0]);

            execvp(av[0 + optind], av + optind);
            perror(av[0 + optind]);
            exit(EXIT_FAILURE);
        }
        else
        {
            ERR_CHECK(wait(NULL));
            char buf[MAX_MEM_SIZE];

            close(pipefd[1]);
            read_n_write(pipefd[0], STDOUT_FILENO, buf);
            close(pipefd[0]);
        }
    }
    else
    {
        execvp(av[0 + optind], av + optind);
        perror(av[0 + optind]);
        exit(EXIT_FAILURE);
    }


    return 0;
}

#undef ERR_CHECK

int read_opts(int ac, char ** av)
{
    int cur_opt = 0,
           opts = 0x0;

    while (true)
    {
        cur_opt = getopt_long(ac, av, SHORT_OPTS, OPTS, NULL);

        switch (cur_opt)
        {
            case '?':
                printf("Unknown key\n");
                return UNKNOWN;
            case 'w':
                opts |= WC;
                break;
            case END:
                return opts;
            default:
                return UNKNOWN;
        }
    }
}

ssize_t safe_write( int fd, const void * buf, size_t count )
{
    ssize_t w_num = 0;
    size_t to_wr = count;

    do
    {
        w_num = write(fd, buf, to_wr);
        buf += w_num;
        to_wr -= w_num;
    } while (w_num > 0);

    return w_num;
}

bool read_n_write( int fd_in, int fd_out, char * buffer )
{
#define ERR_CHECK( cond, msg ) \
    if (cond)                  \
    {                          \
        perror(msg);           \
        return false;          \
    }                          \

    ssize_t r_num = 0,
            w_num = 0;

    do
    {
        r_num = read(fd_in, buffer, MAX_MEM_SIZE);
        ERR_CHECK(r_num < 0, "");

        w_num = safe_write(fd_out, buffer, r_num);
        ERR_CHECK(w_num < 0, "");

    } while (r_num > 0);

    return (w_num > 0 && r_num > 0);
#undef ERR_CHECK
}

