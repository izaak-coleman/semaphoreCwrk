/******************************************************************
 * Program for setting up semaphores/shared memory before running
 * the Producer and Consumer 
 ******************************************************************/

#include <iostream>
#include "helper.h"

static const int NUM_SEM = 3; // Semaphores: mutex(0), item(1), space(2)

using namespace std;

int main (int argc, char **argv)
{

  if ( argc != 2 ) return -1;      // need queue size
  int queueSize = check_arg( argv[1] ) ;   // store number of processes to run


/*---------------------- Generate Semaphores ------------------------*/

  int semid;                        // provides access to sem by other processes
  semid = sem_create( SEM_KEY, NUM_SEM );       // gen semaphore set

  sem_init( semid, MUTEX, 1 );                  // MUTEX is binary
  sem_init( semid, ITEM, 0 );                   // QUEUE starts with no items
  sem_init( semid, SPACE, queueSize );          // QUEUE starts with only spaces


/*---------------------- Generate Shared Memory ---------------------*/

  int shmid = shm_create( SHM_KEY, SHM_SIZE );        // gen shm
  
  QUEUE *shmQueue;
  shmQueue = (QUEUE*) shm_attach( SHM_KEY );     

  shmQueue->front = 0;  shmQueue->end = 0;       // set queue to empty
  shmQueue->size = queueSize;                    // set the total queue size

  /* Sleep for max possible time to assign one job and process it (11)
   * times the queue size. Plus 10 for a buffer. */
  cout << "Start.cc Starting to sleep...\n";
  sleep( (queueSize * 11 ) + 10 );
  cout << "Start.cc finished sleeping... \n";
  sem_close( semid );        // destroy semaphore
  cout << "start.cc destroyed semaphore \n";
  shmdt( shmQueue );
  cout << "start.coo detatching start.cc... \n";
  shmctl( shmid, IPC_RMID, NULL );
  cout << "start.cc Destrying memory\n";
  
  cout << "Start.cc ends \n";
  return 0;
}
