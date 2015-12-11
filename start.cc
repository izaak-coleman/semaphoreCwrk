/******************************************************************
 * Program for setting up semaphores/shared memory before running
 * the Producer and Consumer 
 ******************************************************************/

#include <iostream>
#include "helper.h"


using namespace std;

int main (int argc, char **argv)
{

  if ( argc != 2 ) exit(1);                // check queue size specified
  int queueSize = check_arg( argv[1] );    // store queue size (# of total jobs)
  
  /* If queueSize greater than max array, or negative, exit with error */
  if( queueSize > MAX_QUEUE_SIZE || queueSize < 0 ){
    printf( "Queue size exceeded maximum (500 ). Exiting start \n" );
    exit(1);
  }

  shmid_ds shmStatInfo;                    // shm IPC_STAT info struct
  shmid_ds *shmStatInfo_p = &shmStatInfo; 


/*---------------------- Generate Semaphores ------------------------*/
  int semid;
  semid = sem_create( SEM_KEY, NUM_SEM );       // gen semaphore set

  sem_init( semid, MUTEX, 1 );                  // MUTEX is binary
  sem_init( semid, ITEM, 0 );                   // QUEUE starts with no items
  sem_init( semid, SPACE, queueSize );          // QUEUE starts with only spaces
  sem_init( semid, PROCESSES, 0 );              // number of con/prod processes


/*---------------------- Generate Shared Memory ---------------------*/

  int shmid;
  QUEUE *shmQueue;
  shmid = shm_create( SHM_KEY, SHM_SIZE );
  shmQueue = (QUEUE*) shm_attach( SHM_KEY, shmid, 0 );

  shmQueue->front = 0;  shmQueue->end = 0;       // set queue to empty
  shmQueue->size = queueSize;                    // set the total queue size


/*---------- Remove value from PROCESSES, detach from memory --------*/

  shmdt( shmQueue );
  shmctl( shmid, IPC_STAT, shmStatInfo_p );
  return 0;
}
