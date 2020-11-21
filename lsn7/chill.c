#include "stuff.h"

int main( int argc, char ** argv )
{
  /* Arguments checking */
  if (argc < 5)
  {
    printf("USAGE: ./chill PASSENGERS_NUM "
           "LADDER_CAP "
           "SHIP_CAP "
           "RIDE_NUM\n");
    return 0;
  }

  /* Buffer setting */
  char print_buf[MAX_MEM_SIZE];
  setvbuf(stdout, print_buf, _IOLBF, MAX_MEM_SIZE);

  /* Vars init and other stuff */
  PASS_NUM     = atoi(argv[1]);
  LADDER_CAP   = atoi(argv[2]);
  SHIP_CAP     = atoi(argv[3]);
  int ride_num = atoi(argv[4]);

  SHIP_CAP     = SHIP_CAP > PASS_NUM ? SHIP_CAP : PASS_NUM;
  DOCK_CAP     = SHIP_CAP;

  int sem_id = semget(IPC_PRIVATE, SEM_NUM, ACCESS);

  /* Capitan fork and processing */

  V(sem_id, SUN, 1);
  V(sem_id, TRIP, 1);

  pid_t cap_pid = fork();

  if (cap_pid == 0)
    return process_cap(sem_id, ride_num);
  else
    ERR_CHECK(cap_pid < 0);

  /* Passenger fork and processing */
  for (int i = 0; i < PASS_NUM; ++i)
  {
    pid_t pass_pid = fork();

    if (pass_pid == 0)
    {
      int total_chill = process_pass(sem_id, i);
      printf("#%d chilled total: %d\n", i, total_chill);
      return 0;
    }
    else
      ERR_CHECK(pass_pid < 0);
  }

  for (int i = 0; i < PASS_NUM + 1; ++i)
      ERR_CHECK(wait(NULL) < 0);

  /* finishing */
  semctl(sem_id, 0, IPC_RMID);
  return 0;
}

int process_cap( int sem_id, int rides )
{
  V(sem_id, SHIP, SHIP_CAP);
    printf("SHIP OPENED\n");


  for (int i = 0; i < rides; ++i)
  {
    V(sem_id, DOCK_IN, DOCK_CAP);
      printf("DOCK IN OPENED\n");

    V(sem_id, LADDER, LADDER_CAP);
      printf("LADDER OPENED\n");

    Z(sem_id, DOCK_IN);
      printf("DOCK IN VAL ZERO\n");

    Z(sem_id, DOCK_OUT);
      printf("DOCK OUT VAL ZERO\n");

    Z(sem_id, SHIP);
      printf("SHIP FULL\n");

    P(sem_id, LADDER, LADDER_CAP);
      printf("LADDER CLOSED\n");

    P(sem_id, TRIP, 1);
      printf("---------TRIP---------\n");

    V(sem_id, TRIP, 1);

    V(sem_id, DOCK_OUT, DOCK_CAP);
      printf("DOCK OUT OPENED\n");
  }

  Z(sem_id, DOCK_OUT);
  V(sem_id, SUN, 1);

  return 0;
}

int process_pass( int sem_id, int num )
{
  int count = 0;

  //Z(sem_id, SUN);

  while (1)
  {
      printf("    #%03d waiting\n", num);

    P(sem_id, DOCK_IN, 1);
      printf("    #%03d in dock\n", num);

    P(sem_id, LADDER, 1);
      printf("    #%03d in ladder\n", num);
    V(sem_id, LADDER, 1);
      printf("    #%03d left ladder\n", num);

    P(sem_id, SHIP, 1);
      printf("    #%03d on board\n", num);

    Z(sem_id, TRIP);
      printf("    #%03d tripping\n", num);
      ++count;

    V(sem_id, SHIP, 1);
      printf("    #%03d left board\n", num);

    P(sem_id, LADDER, 1);
      printf("    #%03d in ladder\n", num);

    V(sem_id, LADDER, 1);
      printf("    #%03d left ladder\n", num);

    P(sem_id, DOCK_OUT, 1);
      printf("    #%03d left dock\n", num);

    if (Z_NO_WAIT(sem_id, SUN) == -1)
      break;
  }

  return count;
}

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
  sembuf buf = {ind, 0, 0};
  return semop(sem_id, &buf, 1);
}