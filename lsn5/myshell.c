/*#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_BUF_SZ 4096
#define DELIM " \n\t"

#define ERR_CHECK( cond, msg ) \
  if (cond)                    \
  {                            \
      perror(msg);             \
      return false;            \
  }                            \

int main( )
{
  printf("Hello World!\n");
  char buf[MAX_BUF_SZ];

  while (true)
  {
    printf("> ");

    ssize_t r_num = 0;

    //do
    //{
    r_num = read(STDIN_FILENO, buf, MAX_BUF_SZ);
    ERR_CHECK(r_num < 0, "");
    //} while (r_num > 0);

    int i = 0;
    char * words[MAX_BUF_SZ] = {};

    if (strtok(buf, DELIM) != NULL)
      for(char * tmp = buf; tmp != NULL; tmp = strtok(NULL, DELIM), ++i)
        words[i] = tmp;

    if (!strcmp(words[0], "stop"))
      break;

    execvp(words[0], words + 1);
  }

  return 0;
}*/


#include <stdio.h>
#include <ctype.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <sys/wait.h>

#define MAX_BUF_SZ 4096
#define DELIM " \n\t"

#define ERR_CHECK(what)    \
  if (what < 0)            \
  {                        \
      perror(#what);       \
      exit(EXIT_FAILURE);  \
  }                        \

ssize_t safe_write( int fd, const void * buf, size_t count );
bool read_n_write( int fd_in, int fd_out, char * buffer );

int main( )
{
  int pipefd[2];
  char buf[MAX_BUF_SZ];

  while (true)
  {
    ERR_CHECK(pipe(pipefd));

    //ssize_t r_num = read(STDIN_FILENO, buf, MAX_BUF_SZ);
    //ERR_CHECK(r_num < 0);

    int num = 0;
    do
    {
      num = scanf("%s", buf + num);
    } while (num);

    int i = 0;
    char * words[MAX_BUF_SZ] = {};

    if (strtok(buf, DELIM) != NULL)
      for(char * tmp = buf; tmp != NULL; tmp = strtok(NULL, DELIM), ++i)
        words[i] = tmp;

    if (!strcmp(words[0], "stop"))
      break;

    pid_t cpid = fork();
    ERR_CHECK(cpid);

    if(cpid == 0)
    {
      close(STDOUT_FILENO);
      dup(pipefd[1]);

      close(pipefd[0]);
      close(pipefd[1]);

      execvp(words[0], words);
      perror(words[0]);
      exit(EXIT_FAILURE);
    }
    else
    {
      ERR_CHECK(wait(NULL));
      close(pipefd[1]);

      read_n_write(pipefd[0], STDOUT_FILENO, buf);

      close(pipefd[0]);
    }
  }

  return 0;
}

#undef ERR_CHECK

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
    r_num = read(fd_in, buffer, MAX_BUF_SZ);
    ERR_CHECK(r_num < 0, "");

    w_num = safe_write(fd_out, buffer, r_num);
    ERR_CHECK(w_num < 0, "");

  } while (r_num > 0);

  return (w_num > 0 && r_num > 0);
#undef ERR_CHECK
}