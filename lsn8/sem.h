#ifndef INC_3_RD_SEM_UTILS_H
#define INC_3_RD_SEM_UTILS_H

#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>


#define BUFFER_SIZE 4096
#define MAX_ACCESS 0777

#define ERR_CHECK( statement ) \
  if (statement)               \
  {                            \
    perror(#statement);        \
    _exit(1);                  \
  }                            \

const char SERV_NAME[] = "server.c";
const int SERV_ID = 0;

typedef enum SemInd
{
  MEM = 0,
  CHAN
} SemInd;

typedef struct sembuf sembuf;

void P( int sem_id, SemInd ind, short delta )
{
  sembuf buf = {ind, -delta, 0};
  ERR_CHECK(semop(sem_id, &buf, 1) > 0);
}

void V( int sem_id, SemInd ind, short delta )
{
  sembuf buf = {ind, delta, 0};
  ERR_CHECK(semop(sem_id, &buf, 1) > 0);
}

void Z( int sem_id, SemInd ind )
{
  sembuf buf = {ind, 0, 0};
  ERR_CHECK(semop(sem_id, &buf, 1) > 0);
}

int Z_NO_WAIT( int sem_id, SemInd ind )
{
  sembuf buf = {ind, 0, IPC_NOWAIT};
  return semop(sem_id, &buf, 1);
}

#endif //INC_3_RD_SEM_UTILS_H
