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
    ssize_t w_num = 1;
    size_t to_wr = count;
    const char * cur_buf = buf;

    while (w_num > 0)
    {
        w_num = write(fd, cur_buf, to_wr);
        cur_buf += w_num;
        to_wr -= w_num;
    }

    return w_num;
}

bool read_n_write( int fd_in, int fd_out, char * buffer )
{
    ssize_t r_num = 1, w_num;

    while (r_num > 0)
    {
        r_num = read(fd_in, buffer, MAX_MEM_SIZE);
        if (r_num < 0)
        {
            perror("");
            return false;
        }

        w_num = safe_write(fd_out, buffer, r_num);
        if (w_num < 0)
        {
            perror("");
            return false;
        }
    }

    return (w_num > 0 && r_num > 0);
}


int main( int argc, char ** argv )
{
    char buf[MAX_MEM_SIZE];


    if (argc == 1)
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

