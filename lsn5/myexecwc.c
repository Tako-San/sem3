#include <stdio.h>
#include <ctype.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <sys/wait.h>

#define MAX_MEM_SIZE 4096

#define SHORT_OPTS "w"

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

unsigned read_opts(int ac, char ** av);
void do_wc( int fd );


#define ERR_CHECK(what)    \
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

  unsigned opts = read_opts(ac, av);

  if (opts == UNKNOWN)
    exit(EXIT_FAILURE);
  else if (opts & WC)
  {
    ERR_CHECK(pipe(pipefd));

    pid_t cpid = fork();
    ERR_CHECK(cpid);

    if(cpid == 0)
    {
      close(STDOUT_FILENO);
      dup(pipefd[1]);

      close(pipefd[0]);
      close(pipefd[1]);

      execvp(av[0 + optind], av + optind);
      perror(av[0 + optind]);
      exit(EXIT_FAILURE);
    }
    else
    {
      ERR_CHECK(wait(NULL));
      close(pipefd[1]);
      do_wc(pipefd[0]);
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


void do_wc( int fd )
{
  char buf[MAX_MEM_SIZE];
  ssize_t r_num = 0;

  unsigned bytes = 0,
  words = 0,
  lines = 0;

  do
  {
    r_num = read(fd, buf, MAX_MEM_SIZE);
    if (r_num < 0)
    {
      perror("r_num");
      return;
    }

    bytes += r_num;

    for (int i = 0, flag = 0; i < r_num; ++i)
    {
      if (!isspace(buf[i]) && flag == 0)
      {
        ++words;
        flag = 1;
      }
      else if (isspace(buf[i]))
      {
        flag = 0;

        if (buf[i] == '\n')
          ++lines;
      }
    }

  } while (r_num > 0);

  printf("%7u %7u %7u\n", lines, words, bytes);
}

unsigned read_opts(int ac, char ** av)
{
  unsigned cur_opt = 0,
           opts    = 0x0u;

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


#undef ERR_CHECK

