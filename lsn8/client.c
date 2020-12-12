#include "sem.h"

int shm_id = 0;
char * shm_buf = NULL;

int print( FILE * fin, int sem_id );

int main( int ac, char ** av)
{
  key_t key = ftok(SERV_NAME, SERV_ID);
  shm_id = shmget(key, BUFFER_SIZE, MAX_ACCESS);
  ERR_CHECK(shm_id < 0);

  shm_buf = shmat(shm_id, NULL, 0);
  int sem_id = semget(key, 2, MAX_ACCESS);
  ERR_CHECK(sem_id < 0);

  if (ac == 1 || av[1][0] == '-')
  {
    if (!print(stdin, sem_id))
      return 1;
    return 0;
  }

  for (int i = 1; i < ac; ++i)
  {
    int fd = open(av[i], O_RDONLY);
    if (fd < 0)
    {
      perror(av[i]);
      continue;
    }
    FILE * fin = fdopen(fd, "rb");
    if (fin == NULL)
    {
      perror(av[i]);
      continue;
    }

    if (!print(fin, sem_id))
      return 1;

    ERR_CHECK(fclose(fin) < 0);
  }

  return 0;
}

int print( FILE * fin, int sem_id )
{
  while (1)
  {
    P(sem_id, CHAN, 1);

    if (fgets(shm_buf, BUFFER_SIZE, fin) == NULL)
    {
      V(sem_id, CHAN, 1);
      break;
    }

    V(sem_id, MEM, 1);
    Z(sem_id, MEM);
  }

  return 1;
}
