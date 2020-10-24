#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_MEM_SIZE  4096
#define ACCESS        0700
#define JDG_T         (N + 1)

bool ac_check( int ac );

int judging( int msg_id, unsigned num );
int running( int msg_id, unsigned N, unsigned id );

typedef struct msgbuf
{
  long mtype;
  char mtext[1];
} msgbuf;

#define ERR_CHECK( statement ) \
  if (statement)               \
  {                            \
    perror(#statement);        \
    return 1;                  \
  }                            \

int main(int ac, char ** av)
{
  if (!ac_check(ac))
    return 1;

  char buf[MAX_MEM_SIZE] = {};
  setvbuf(stdout, buf, _IOLBF, MAX_MEM_SIZE);

  unsigned N = atoi(av[1]);

  printf("CREATOR: %d\n", getpid());

  int msg_id = msgget(IPC_PRIVATE, ACCESS);
  ERR_CHECK(msg_id < 0);

  pid_t judge_pid = fork();

  if (judge_pid == 0)
    return judging(msg_id, N);

  for (unsigned i = 0; i < N; ++i)
  {
    pid_t runner_pid = fork();
    ERR_CHECK(runner_pid < 0);

    if (runner_pid == 0)
      return running(msg_id, N, i + 1);
  }

  for (unsigned i = 0; i < N + 1; ++i)
    wait(NULL);

  msgctl(msg_id, IPC_RMID, NULL);

  return 0;
}

bool ac_check( int ac )
{
  if (ac > 2)
    printf("Too many arguments\n");
  else if (ac < 2)
    printf("Too few arguments\n");
  else
    return true;

  return false;
}

int judging( int msg_id, unsigned N )
{
  msgbuf tmp = {0, 0};

  for (unsigned i = 1; i < N + 1; ++i)
    msgrcv(msg_id, &tmp, sizeof(char), JDG_T, 0x0);

  printf("Competition starting!\n");

  msgbuf frst = {1, 0};
  msgsnd(msg_id, &frst, sizeof(char), 0x0);

  msgrcv(msg_id, &tmp, sizeof(char), JDG_T, 0x0);

//  for (unsigned i = 1; i < N + 1; ++i)
//    msgrcv(msg_id, &tmp, sizeof(char), JDG_T, 0x0);

  printf("Competition ended!\n");

  for (unsigned i = 1; i < N + 1; ++i)
  {
    msgbuf msg_i = {i, 0};
    msgsnd(msg_id, &msg_i, sizeof(char), 0x0);
  }

  return 0;
}

int running( int msg_id, unsigned N, unsigned id )
{
  msgbuf to_jdg = {JDG_T, 0};
  printf("Runner #%03u arrived\n", id);
  msgsnd(msg_id, &to_jdg, sizeof(char), 0x0);

  msgbuf stick = {id, 0};
  msgrcv(msg_id, &stick, sizeof(char), id, 0x0);
  printf("Runner #%03u got stick\n", id);

  ++stick.mtype;
  printf("Runner #%03u giving stick away\n", id);
  msgsnd(msg_id, &stick, sizeof(char), 0x0);

  // msgsnd(msg_id, &to_jdg, sizeof(char), 0x0);
  // printf("Runner #%03u ready to leave\n", id);

  msgbuf end = {id, 0};
  msgrcv(msg_id, &end, sizeof(char), id, 0x0);
  printf("Runner #%03u leave\n", id);

  return 0;
}