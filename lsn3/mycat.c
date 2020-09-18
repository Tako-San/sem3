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
#define MAX_ACCES    0777

void from_command_line()
{
    char * buffer = (char *)calloc(MAX_MEM_SIZE, sizeof(char));
    ssize_t num = 1;

    while (num != 0)
    {
        num = read(STDIN_FILENO, buffer, MAX_MEM_SIZE);
        write(STDOUT_FILENO, buffer, num);
    }

    free(buffer);
}

void from_file( int ac, char ** av )
{
    assert(ac > 0);

    char * buffer = (char *)calloc(MAX_MEM_SIZE, sizeof(char));
    ssize_t num = 1;

    for ( int i = 1; i < ac; ++i )
    {
        FILE * fd = open(av[i], O_RDONLY, MAX_ACCES);

        num = read(fd, buffer, MAX_MEM_SIZE);
        write(STDOUT_FILENO, buffer, num );

        close(fd);
    }
}

int main( int argc, char ** argv )
{
    if (argc == 1)
        from_command_line();
    else
        from_file(argc, argv);

    return 0;
}

