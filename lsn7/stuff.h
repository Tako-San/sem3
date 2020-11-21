#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_MEM_SIZE  4096
#define ACCESS        0777
#define SEM_NUM          7

#define ERR_CHECK( statement ) \
  if (statement)               \
  {                            \
    perror(#statement);        \
    _exit(1);                  \
  }                            \

typedef struct sembuf sembuf;

typedef enum SemInd
{
  SUN = 0,
  TRIP,
  SHIP,
  LADDER,
  DOCK_IN,
  DOCK_OUT
} SemInd;

int PASS_NUM   = 0,
    SHIP_CAP   = 0,
    DOCK_CAP   = 0,
    LADDER_CAP = 0;

void P( int sem_id, SemInd ind, short delta );
void V( int sem_id, SemInd ind, short delta );
void Z( int sem_id, SemInd ind );
int Z_NO_WAIT( int sem_id, SemInd ind );


int process_cap( int sem_id, int rides );
int process_pass( int sem_id, int num );