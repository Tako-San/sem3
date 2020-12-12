#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/mman.h>
#include <mqueue.h>

#define MAX_ACCESS 0777
#define QUEUE_NAME "/shmque"
#define BUF_SIZE 4096

#define ERR_CHECK( cond )       \
if (!(cond))                    \
  return process_error(#cond)   \


#define ERR_CHECK_DF( expr ) ERR_CHECK((expr) != -1)


__inline int process_error( char * str_err ) __attribute__((always_inline));
int mq_read( int fd, char * ptr, size_t size );
int mq_write( int fd, char * ptr, size_t size );
int file_len( int fd );
int process_reader( int fin );
int process_writer( int fout );
int init( mqd_t * mqd, int * shm_id );
int my_close( mqd_t mqd, int shm );


int main( int ac, char ** av )
{
  if (ac != 3)
  {
    printf("USAGE: %s SRC DST\n", av[0]);
    return 0;
  }

  mqd_t mqd_id = 0;
  int shm_id = 0;
  ERR_CHECK(init(&mqd_id, &shm_id) == 0);

  int fin = open(av[1], O_RDONLY, MAX_ACCESS);
  ERR_CHECK_DF(fin);

  int fout = open(av[2], O_CREAT | O_WRONLY, MAX_ACCESS);
  ERR_CHECK_DF(fout);

  pid_t reader_pid = fork();
  if (reader_pid == 0)
    exit(process_reader(fin));

  pid_t writer_pid = fork();
  if (writer_pid == 0)
    exit(process_writer(fout));

  for (int i = 0; i < 2; ++i)
    wait(NULL);

  ERR_CHECK(my_close(mqd_id, shm_id) == 0);
  return 0;
}

int mq_read( int fd, char * ptr, size_t size )
{
  size_t read_sz;
  int sz = size;

  while (sz > 0)
  {
    read_sz= read(fd, ptr, sz);

    ERR_CHECK_DF(read_sz);
    if (read_sz== 0)
      return 0;

    ptr += read_sz;
    sz -= read_sz;
  }
  return 0;
}

int mq_write( int fd, char * ptr, size_t size )
{
  size_t written_sz;

  while (size > 0)
  {
    written_sz = write(fd, ptr, size);

    ERR_CHECK_DF(written_sz);
    if (written_sz == 0)
      return 0;

    ptr += written_sz;
    size -= written_sz;
  }
  return 0;
}

int file_len( int fd )
{
  struct stat buf;
  return fstat(fd, &buf) == -1 ? -1 : buf.st_size;
}

int process_reader( int fin )
{
  int length = file_len(fin);
  ERR_CHECK_DF(length);

  int shm_id = shm_open(QUEUE_NAME, O_RDWR, S_IRUSR | S_IWUSR);
  ERR_CHECK_DF(shm_id);
  ERR_CHECK_DF(ftruncate(shm_id, length));

  char * ptr = mmap(NULL, length, PROT_WRITE | PROT_READ, MAP_SHARED, shm_id, 0);
  ERR_CHECK(ptr != MAP_FAILED);

  mqd_t mqd = mq_open(QUEUE_NAME, O_RDWR);
  ERR_CHECK_DF(mqd);
  ERR_CHECK_DF(mq_read(fin, ptr, length));

  char msg_buf[BUF_SIZE];

  ERR_CHECK_DF(munmap(ptr, length));
  ERR_CHECK_DF(mq_send(mqd, msg_buf, BUF_SIZE, 0));
  ERR_CHECK_DF(close(fin));
  ERR_CHECK_DF(close(shm_id));
  return 0;
}

int process_writer( int fout )
{
  mqd_t mqd = mq_open(QUEUE_NAME, O_RDWR);
  ERR_CHECK_DF(mqd);

  int shm_id = shm_open(QUEUE_NAME, O_RDWR, S_IRUSR | S_IWUSR);
  ERR_CHECK_DF(shm_id);

  int length = file_len(shm_id);
  ERR_CHECK_DF(length);

  char msg_buf[BUF_SIZE];
  ERR_CHECK_DF(mq_receive(mqd, msg_buf, BUF_SIZE, NULL));

  char * ptr = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
  ERR_CHECK(ptr != MAP_FAILED);

  mq_write(fout, ptr, length);

  ERR_CHECK_DF(close(shm_id));
  ERR_CHECK_DF(munmap(ptr, length));
  ERR_CHECK_DF(close(fout));
  return 0;
}

int init( mqd_t * mqd, int * shm_id )
{
  struct mq_attr att = {0, 10, BUF_SIZE, 0};

  *mqd = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, MAX_ACCESS, &att);
  ERR_CHECK_DF(*mqd);

  *shm_id = shm_open(QUEUE_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  ERR_CHECK_DF(*shm_id);

  return 0;
}


int my_close( mqd_t mqd, int shm )
{
  ERR_CHECK_DF(close(shm));
  ERR_CHECK_DF(mq_close(mqd));
  ERR_CHECK_DF(mq_unlink(QUEUE_NAME));
  ERR_CHECK_DF(shm_unlink(QUEUE_NAME));
  return 0;
}

int process_error( char * str_err )
{
  perror(str_err);
  return errno;
}
