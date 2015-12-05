// Consumer
#include <iostream>
#include "helper.h"

using namespace std;


void aquireJobInfo( int &jobID, int &duration, QUEUE *queue );
/* Get the next job id and duration pointed to by (QUEUE->front)
 * Once information aquired, deallocate the job by incrementing the pointer
 * to the next job*/

void processJob( int pid, time_t &start, time_t &end, int jobID, int duration );
/* Processes the next job, printing time, pid, job id
 * and job duration information */

int main (int argc, char *argv[])
{
  
  /*------------ Set up process -----------*/ 
  time_t start, end;
  int elapsed;
  time(&start);                               // start process time
  shmid_ds shmStatInfo;                       // shm IPC_STAT info struct
  shmid_ds *shmStatInfo_p = &shmStatInfo;


  if( argc != 2) exit(1);                     // check process id specified
  int pid = check_arg( argv[1] );             // get the process id


  int semid = sem_attach( SEM_KEY );          // gain access to semaphore set
  sem_signal( semid, PROCESSES );             // add process to PROCESSES


  int shmid = shm_create( SHM_KEY, SHM_SIZE );      // get shmid 
  QUEUE *shmQueue;
  shmQueue = (QUEUE*) shm_attach( SHM_KEY, SHM_R ); // attach to shared memory 

  /*-------------- consume required jobs in while loop ----*/
  int processTime, jobID;
  while( true ){

    /* ---------------- if job not received in 10s ----------------*/
    /*-------- then exit program by running if statment -----------*/

    if( sem_timewait( semid, ITEM, EXIT_TIMEC ) != 0 ){ 

      sem_wait( semid, PROCESSES );                 // remove from PROCESSES
      sem_zero_wait( semid, PROCESSES );    // wait untill last process finishes

     
      /* As detatching the consumer, updating the shm stat info,
         and branching the shm/sem deletion to the last consumer
         is non atomic, this is performed in a mutually excluded
         Crit.sec to stop multiple deletions. */
      sem_wait( semid, MUTEX );

      time(&end);
      elapsed = difftime( end, start );
      printf( "Consumer(%d) time %2d: No more jobs left.\n", pid, elapsed );
      shmdt( shmQueue );                             // detach from shm
      shmctl(shmid, IPC_STAT, shmStatInfo_p);        // update shm stat info
      if( shmStatInfo_p->shm_nattch == 0 ){          // if last process attached
        shmctl(shmid, IPC_RMID, shmStatInfo_p);      // destory memory
        sem_close( semid );                          // destroy semaphore
      }
      
      sem_signal( semid, MUTEX );

      return 0;                                      // end consumer
    }


    /*------------ else, if job was received within 10s ----------*/
    /*------------- access shm, get job and process it  ----------*/

    sem_wait( semid, MUTEX );                 // request shared memory access
    aquireJobInfo( jobID, processTime, shmQueue );      // get next job duration
    sem_signal( semid, MUTEX );               // unlock shared memory
    sem_signal( semid, SPACE );               // signal space has freed up


    processJob( pid, start, end, jobID, processTime );  // process current job
  }

  /* Program should NOT exit from here, so exit(1) */

  sem_wait( semid, PROCESSES );               // remove from PROCESSES
  sem_zero_wait( semid, PROCESSES );          // wait until last process fin

  exit(1);                                    // should return from while
}

void aquireJobInfo( int &jobID, int &duration, QUEUE *queue ){
  int currentFront = queue-> front;

  jobID = queue->jobs[currentFront].id;             // get job id
  duration = queue->jobs[currentFront].duration;    // and duration
  
  queue->front++;                                   // increment queue front
  queue->front %= queue->size;                      // loop if necessary
}

void processJob( int pid, time_t &start, time_t &end, int jobID, int duration )
{
  int elapsed;

  time(&end);
  elapsed = difftime( end, start );                 // time elapsed before job
  printf( "Consumer(%d) time %2d: Job id %d executing sleep duration %d\n",
          pid, elapsed, jobID, duration );


  sleep( duration ); // sleep for duration specified by job 
  time(&end);
  elapsed = difftime( end, start );                 // time elapsed after job  
  printf( "Consumer(%d) time %2d: Job id %d completed\n", pid, elapsed, jobID );
}
