#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define MAX_MEM_SIZE 4096
#define MAX_ACCESS   0777

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


int main( int argc, char ** argv )
{
    char buf[MAX_MEM_SIZE];


    if (argc == 1 || !strcmp(argv[1], "-"))
    {
        read_n_write(STDIN_FILENO, STDOUT_FILENO, buf);
        return 0;
    }


    for (int i = 1; i < argc; ++i)
    {
        int fd = open(argv[i], O_RDONLY, MAX_ACCESS);

        if (fd < 0)
        {
            perror(argv[i]);
            continue;
        }

        read_n_write(fd, STDOUT_FILENO, buf);

        if (close(fd) < 0)
            perror(argv[i]);
    }

    return 0;

}

