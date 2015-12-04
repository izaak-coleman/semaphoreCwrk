/******************************************************************
 * The helper file that contains the following helper functions:
 * check_arg - Checks if command line input is a number and returns it
 * sem_create - Create number of sempahores required in a semaphore array
 * sem_attach - Attach semaphore array to the program
 * sem_init - Initialise particular semaphore in semaphore array
 * sem_wait - Waits on a semaphore (akin to down ()) in the semaphore array
 * sem_timewait - Waits on a semaphore for a particular time
 * sem_signal - Signals a semaphore (akin to up ()) in the semaphore array
 * sem_close - Destroy the semaphore array
 ******************************************************************/

# include "helper.h"
#include <iostream>
using namespace std;

int check_arg (char *buffer)
{
  int i, num = 0;
  for (i=0; i < (int) strlen (buffer); i++)
  {
    if ((0+buffer[i]) > 57)
      return -1;
    num += pow (10, strlen (buffer)-i-1) * (buffer[i] - 48);
  }
  return num;
}

int sem_attach (key_t key)
{
  int id;
  if ((id = semget (key, 1,  0)) < 0)
    return -1;
  return id;
}
 
int sem_create (key_t key, int num) // generate semaphore set
{
  int id;  // 'unique' sem id, used by process to access semset
  if ((id = semget (key, num,  0666 | IPC_CREAT | IPC_EXCL)) < 0) // gen id
    return -1;
  return id;
}

int sem_init (int id, int num, int value)  // needed to give sem value
{
  union semun semctl_arg;  // generate a semun, semun for better management
  semctl_arg.val = value;  // load value
  if (semctl (id, num, SETVAL, semctl_arg) < 0)  // set value for one sem
    return -1;
  return 0;
}

void sem_wait (int id, short unsigned int num)
{
  struct sembuf op[] = {
    {num, -1, SEM_UNDO}
  };
  semop (id, op, 1);
}

int sem_timewait (int id, short unsigned int num, int tim)
{
  struct timespec ts = {tim, 0};
  struct sembuf op[] = {
    {num, -1, SEM_UNDO}
  };
  if (semtimedop(id, op, 1, &ts ) == -1 )
    if (errno == EAGAIN)
      return -1;
  return 0;
}

void sem_signal (int id, short unsigned int num)
{
  struct sembuf op[] = {
    {num, 1, SEM_UNDO}
  };
  semop (id, op, 1);
}

void sem_zero_wait( int id, short unsigned int num )
{
  struct sembuf op[] = {
    {num, 0, SEM_UNDO}
  };
  semop( id, op, 1);
}
  
int get_sem_value( int id, short unsigned int num )
{
  return semctl( id, num, GETVAL, 0 );
}

int sem_close (int id)
{
  if (semctl (id, 0, IPC_RMID, 0) < 0)
    return -1;
  return 0;
}

int shm_create( key_t shmKey, int shmSize )
{
  int shmID;
  shmID = shmget( shmKey, shmSize, 0666 | IPC_CREAT );
  return shmID;
}

void* shm_attach( key_t shmKey, int access )
{
  int shmid;
  shmid = shmget( shmKey, 0, access|0 );

  void *shmemory;
  shmemory = shmat( shmid, (void *)0, 0 );
  return shmemory;
}
