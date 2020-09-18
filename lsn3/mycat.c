#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#include <assert.h>

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_MEM_SIZE 100
#define MAX_ACCESS   0777

void from_command_line()
{
    char * buffer = (char *)calloc(MAX_MEM_SIZE, sizeof(char));
    ssize_t r_num = 1,
            w_num = 1;

    while (r_num != 0)
    {
        r_num = read(STDIN_FILENO, buffer, MAX_MEM_SIZE);

        if (r_num < 0)
            perror("stdin");

        w_num = write(STDOUT_FILENO, buffer, r_num);
        if (w_num < 0)
            perror("stdout");
    }

    free(buffer);
}

void from_file( int ac, char ** av )
{

#define halt()    \
    close(fd);    \
    continue      \

    assert(ac > 0);

    char * buffer = (char *)calloc(MAX_MEM_SIZE, sizeof(char));

    for ( int i = 1; i < ac; ++i )
    {
        int fd = open(av[i], O_RDONLY, MAX_ACCESS);

        if (fd < 0)
        {
            perror(av[i]);
            halt();
        }

        ssize_t r_num = read(fd, buffer, MAX_MEM_SIZE);
        if (r_num < 0)
        {
            perror(av[i]);
            halt();
        }

        ssize_t w_code = write(STDOUT_FILENO, buffer, r_num );
        if (w_code < 0)
        {
            perror("stdout");
            halt();
        }

        close(fd);
    }

    free(buffer);

#undef halt
}

int main( int argc, char ** argv )
{
    if (argc == 1)
        from_command_line();
    else
        from_file(argc, argv);

    return 0;
}

