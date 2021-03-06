#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <errno.h>
#include <assert.h>
#define __USE_GNU
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>

#define MAX_ACCESS 0777

#define CHECK(cond, err_str) \
  do                           \
  {                         \
    if (!(cond))                 \
    {                           \
      printf("ASSERTION FAILED: %s\n", #cond);                           \
      return process_error(err_str);     \
    }                           \
  } while(0)

#define CHECK_DF(expr, err_str) CHECK((expr) >= 0, err_str)

const int time_wait = 200;

__inline int process_error( char *str_err ) __attribute__((always_inline));

int process_error( char *str_err )
{
  perror(str_err);
  return errno;
}

int MyWrite( int fd, const void *buffer, size_t buf_size )
{
  size_t bytes_written = 0;

  while (bytes_written < buf_size)
  {
    int write_ret = write(fd, buffer, buf_size - bytes_written);

    if (write_ret < 0)
    {
      if (errno == EAGAIN)
        break;
      return process_error("Error with writing in file");
    }

    bytes_written += write_ret;
    buffer += write_ret;
  }

  return 0;
}


int GetFlength( int fd )
{
  struct stat buf;
  return fstat(fd, &buf) == -1 ? -1 : (int)buf.st_size;
}

ssize_t MyRead( int fd, void **buffer )
{
  assert(buffer != NULL);
  assert(*buffer == NULL);

  ssize_t file_size = GetFlength(fd);
  CHECK_DF(file_size, "error in getting length of a file");
  size_t bytes_read = 0;

  int read_ret = 0;

  printf("size = %ld\n", file_size);
  *buffer = calloc(file_size, sizeof(**buffer));
  CHECK(*buffer != NULL, "error with memory allocation");

  while (bytes_read < file_size)
  {
    read_ret = read(fd, *buffer + bytes_read, file_size - bytes_read);

    if (read_ret < 0)
    {
      if (errno == EAGAIN)
        break;
      return process_error("Error with reading from file");
    }

    bytes_read += read_ret;
  }

  return file_size;
}




typedef struct tagPipePair
{
  int to_read[2];
  int to_write[2];
} PPair;


int forks_num = 0;

int ChildWork( const PPair *p_pair, int num )
{
  int fd_in  = p_pair->to_write[0],
    fd_out = p_pair->to_read[1];

  void *buf = NULL;

  printf("Ch%d: Reading bytes from %d to %d\n", num, fd_in, fd_out);

  ssize_t bytes_read = MyRead(fd_in, &buf);
  CHECK_DF(bytes_read, "reading error");
  printf("Ch%d: Read %ld bytes from %d\n", num, bytes_read, fd_in);

  if (bytes_read == 0)
    return 0;


  CHECK_DF(MyWrite(fd_out, buf, (size_t)bytes_read), "writing error");

  printf("Ch%d: Wrote %ld bytes to %d\n", num, bytes_read, fd_out);

  free(buf);



  CHECK_DF(close(fd_in), "closing error for fd_in");
  CHECK_DF(close(fd_out), "closing error for fd_out");
  if (num != 0)
    CHECK_DF(close(p_pair->to_write[1]), "closing error for write end of to write");
  if (num != forks_num - 1)
    CHECK_DF(close(p_pair->to_read[0]), "closing error for read end of to read");
  return 0;
}

int CloseAll( PPair *p_prs )
{
  for (int i = 0; i < forks_num; ++i)
  {
    if (i != 0)
      CHECK_DF(close(p_prs[i].to_write[1]), "closing error");
    if (i != forks_num - 1)
      CHECK_DF(close(p_prs[i].to_read[0]), "closing erroro");

    CHECK_DF(close(p_prs[i].to_read[1]), "close to read[1] error");
    CHECK_DF(close(p_prs[i].to_write[0]), "close to write[0] error");
  }
}

int ParentWork( int epoll_fd )
{
  while (1)
  {
    struct epoll_event evs[forks_num];

    int wret = epoll_wait(epoll_fd, evs, forks_num, time_wait);
    CHECK_DF(wret, "wait error");

    if (!wret)
      break;

    for (int i = 0; i < wret; ++i)
    {
      if (evs[i].events & EPOLLERR)
        return printf("Spme errors with file\n");

      if (evs[i].events & EPOLLIN)
      {
        PPair *p_pair = (PPair *)evs[i].data.ptr;
        int fd_in = p_pair->to_read[0];
        int fd_out = (p_pair + 1)->to_write[1];

        void *buf = NULL;


        ssize_t bytes_read = MyRead(fd_in, &buf);
        CHECK_DF(bytes_read, "Error in reading");

        CHECK_DF(MyWrite(fd_out, buf, bytes_read), "Write error");

        free(buf);
      }
    }

  }

  return 0;
}

void Pdump( PPair *ppfds )
{
  printf("-----------------------------------------------\n");
  for (int i = 0; i < forks_num; ++i)
  {
    printf("#%d Parent's READ pipe:\n", i);
    printf("%d == [0], %d == [1]\n", ppfds[i].to_read[0], ppfds[i].to_read[1]);
    printf("#%d Parent's WRITE pipe:\n", i);
    printf("%d == [0], %d == [1]\n", ppfds[i].to_write[0], ppfds[i].to_write[1]);
  }
  printf("-----------------------------------------------\n");
}

int main( int argc, char *argv[] )
{
  if (argc != 4)
  {
    printf("USAGE: %s INPUT OUTPUT N_OF_PROC\n", argv[0]);
    return 0;
  }

  forks_num = atoi(argv[3]);
  if (forks_num <= 0)
  {
    printf("Incorrect forks amount: %d\n", forks_num);
    return 0;
  }

  PPair pipe_fds[forks_num];

  int epoll_fd = epoll_create1(0);
  CHECK_DF(epoll_fd, "epoll creating error");

  for (int i = 0; i < forks_num; ++i)
  {
    if (i != 0)
      pipe(pipe_fds[i].to_write);

    if (i != forks_num - 1)
    {
      struct epoll_event ev = {.events = EPOLLIN, .data = pipe_fds + i};

      pipe(pipe_fds[i].to_read);

      epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipe_fds[i].to_read[0], &ev);
    }
  }

  Pdump(pipe_fds);

  for (int i = 0; i < forks_num; ++i)
  {
    pid_t cpid = fork();

    CHECK_DF(cpid, "fork error");

    if (cpid == 0)
    {
      if (i == 0)
      {
        int fd_in = open(argv[1], O_RDONLY, MAX_ACCESS);
        CHECK_DF(fd_in, argv[1]);

        pipe_fds[i].to_write[0] = fd_in;
      }
      if (i == forks_num - 1)
      {
        int fd_out = open(argv[2], O_WRONLY | O_CREAT, MAX_ACCESS);
        CHECK_DF(fd_out, argv[2]);

        pipe_fds[i].to_read[1] = fd_out;
      }

      // Do the child work
      return ChildWork(pipe_fds + i, i);
    }
  }

  ParentWork(epoll_fd);

  CloseAll(pipe_fds);

  for (int i = 0; i < forks_num; ++i)
    wait(NULL);

  CHECK_DF(close(epoll_fd), "epoll closing error");
  return 0;
}