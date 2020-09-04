#include <unistd.h>

#define ST_OUT 1

int main()
{
    char phrase[] = "Hello, world\n";
    size_t str_len = sizeof(phrase);

    write(ST_OUT, phrase, str_len);
    return 0;
}
