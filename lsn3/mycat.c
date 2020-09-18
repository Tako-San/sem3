#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>


#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <stdbool.h>

#define MAX_MEM_SIZE 4096
#define MAX_ACCESS   0777

bool read_n_write_to_stdout( int fd_in, char * buffer )
{
    ssize_t r_num, w_num;
    // TODO: добавить цикл
    r_num = read(fd_in, buffer, MAX_MEM_SIZE);
    if (r_num < 0)
        return false; // TODO: добавить вывод через perror

    w_num = write(STDOUT_FILENO, buffer, r_num);
    if (w_num < 0)
    {
        perror("stdout");
        return false;
    } // TODO: обёртка над write

    return (w_num > 0 && r_num > 0);
}


int main( int argc, char ** argv )
{
    char buf[MAX_MEM_SIZE];


    //
    if (argc == 1)
    {
        while (read_n_write_to_stdout(STDIN_FILENO, buf));

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

        read_n_write_to_stdout(fd, buf);
        close(fd);  // TODO: catch errors in close
    }

    return 0;
}

