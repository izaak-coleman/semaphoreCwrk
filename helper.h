/*****************************************************************
 * Header file for the helper functions. This file includes the
 * required header files, as well as the structures and the shared
 * memory/semaphore values (which are to be changed as needed).
 ******************************************************************/


# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/shm.h>
# include <sys/sem.h>
# include <sys/time.h>
# include <math.h>
# include <errno.h>
# include <string.h>

# define SHM_W 0200
# define SHM_R 0400
# define SHM_MODE (SHM_R | SHM_W)

typedef struct jobtype
{
  int id;
  int duration;
} JOBTYPE;

typedef struct queue 
{
  int size; 
  int front;
  int end;
  JOBTYPE jobs[500]; // Can assume this to be maximum queue size
} QUEUE;

static const key_t SEM_KEY = ftok( "helper.h", 'I' ); // key for sem access / gen
static const key_t SHM_KEY = ftok( "helper.cc", 'Z' );// key for shm access / gen
static const int EXIT_TIME = 10;
static const int SHM_SIZE = sizeof(QUEUE);
static const int MUTEX = 0;
static const int ITEM = 1;
static const int SPACE  = 2;

union semun {
    int val;               /* used for SETVAL only */
    struct semid_ds *buf;  /* used for IPC_STAT and IPC_SET */
    ushort *array;         /* used for GETALL and SETALL */
};


int check_arg (char *);
int sem_create (key_t, int);
int sem_attach (key_t);
int sem_init (int, int, int);
void sem_wait (int, short unsigned int);
int sem_timewait (int, short unsigned int, int);
void sem_signal (int, short unsigned int);
int sem_close (int);

int shm_create( key_t shmKey, int shmSize );
void* shm_attach( int shmid );
