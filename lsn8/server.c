#include "sem.h"

int shm_id = 0;
char * buffer = 0;

void process_serv( int sem_id );

int main( )
{
  char buf[BUFFER_SIZE];
  setvbuf(stdout, buf, _IOLBF, BUFFER_SIZE);

  key_t key = ftok(SERV_NAME, SERV_ID);
  shm_id = shmget(key, BUFFER_SIZE, MAX_ACCESS | IPC_CREAT);
  int sem_id = semget(key, 2, MAX_ACCESS | IPC_CREAT);
  buffer = shmat(shm_id, NULL, 0);

  process_serv(sem_id);

  shmctl(shm_id, IPC_RMID, NULL);
  shmctl(sem_id, 0, IPC_RMID);

  return 0;
}

void process_serv( int sem_id )
{
  V(sem_id, CHAN, 1);

  while (1)
  {
    P(sem_id, MEM, 1);

    printf("%s", buffer);
    V(sem_id, CHAN, 1);
    Z(sem_id, CHAN);
  }
}
