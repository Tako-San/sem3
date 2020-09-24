#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <unistd.h>
#include <getopt.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define MAX_MEM_SIZE 4096
#define MAX_ACCESS   0777

#define SHORT_OPTS   "fi"

enum FLAGS
{
    INTERACTIVE = 0x1,
    FORCE = 0x2,
};

enum OP_CODE
{
    END         = -1,
    UNKNOWN     = -2
};

struct option OPTS[] =
        {
            {"force", 0, NULL, 'f'},
            {"interactive", 0, NULL, 'i'},
            {0, 0, 0, 0}
        };


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

int read_opts(int ac, char ** av)
{
    int cur_opt = 0,
        opts   = 0x0;

    while (true)
    {
        cur_opt = getopt_long(ac, av, SHORT_OPTS, OPTS, NULL);

        switch (cur_opt)
        {
            case '?':
                printf("Unknown key");
                return UNKNOWN;
            case 'i':
                printf("i\n");
                opts |= INTERACTIVE;
                break;
            case 'f':
                printf("f\n");
                opts |= FORCE;
                break;
            case END:
                return opts;
            default:
                printf("Unknown key: -%c\n", cur_opt);
                return UNKNOWN;
        }
    }
}

bool make_copy(int opts, char * file_src, char * file_dst)
{
    char buf[MAX_MEM_SIZE];

    int dst_oflag = O_WRONLY | O_CREAT;
    dst_oflag |= (opts & FORCE) == 0 ? O_EXCL : 0x0;

    int src = open(file_src, O_RDONLY, MAX_ACCESS);
    if (src < 0)
    {
        perror(file_src);
        return false;
    }

    int dst = open(file_dst, dst_oflag, MAX_ACCESS);
    if (dst < 0)
    {
        close(src);
        perror(file_dst);
        return false;
    }

    read_n_write(src, dst, buf);

    close(src);
    close(dst);

    return true;
}

int main( int argc, char ** argv )
{
    if (argc < 2)
    {
        printf("Not enough arguments\n");
        return 1;
    }

    int opts = read_opts(argc, argv);

    if (opts == UNKNOWN)
    {
        printf("Wrong key\n");
        return 1;
    }

    if (!make_copy(opts, argv[0 + optind], argv[1 + optind]))
        return 1;

    return 0;
}

