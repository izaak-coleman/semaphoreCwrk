// Consumer
#include <iostream>
#include "helper.h"

using namespace std;


void aquireJobInfo( int &jobID, int &duration, QUEUE *queue );
/* Get the next job id and duration pointed to by (QUEUE->front)
 * Once informaiotn aquired, deallocate the job by incrementing the pointer
 * to the next job*/

void processJob( int pid, time_t &start, time_t &end, int jobID, int duration );
/* Processes the next job, printing time, pid, job id
 * and job duration information */

bool deadWait( int exitSwitch , QUEUE* queue );
/* If no new jobs in 10s, then detach process from shared memory */

int main (int argc, char *argv[])
{
  time_t start, end;
  time(&start);                                     // start process time

  if( argc != 2) return -1;
  int pid = check_arg( argv[1] );


  int semid = sem_attach( SEM_KEY );          // gain access to semaphore set
  QUEUE *shmQueue;
  shmQueue = (QUEUE*) shm_attach( SHM_KEY );  // gain access to shared memory


  int processTime, jobID;
  while( true ){
    if(  deadWait( sem_timewait( semid, ITEM, EXIT_TIME ), shmQueue ) ){
      cout << "ending consumer ... \n";
      return 0;                         // if dead wait is true, end process
    }

    sem_wait( semid, MUTEX );                   // request shared memory access

    aquireJobInfo( jobID, processTime, shmQueue );      // get next job duration
    
    sem_signal( semid, MUTEX );                 // unlock shared memory
    sem_signal( semid, SPACE );                 // signal space has freed up

    processJob( pid, start, end, jobID, processTime );  // process current job
  }

  return 0;
}

bool deadWait( int exitSwitch , QUEUE* const queue ){
  switch( exitSwitch ){
    case 0:
      return false;

    case -1:
    cout << "A consumer waited for 10s and nothing came... so detatching memory\n";
      shmdt( queue );
      sleep( 10 );   // sleep for 10s to allow other consumers to finish job
      return true;
    
    default:
      return true;   // error must have occured so terminate
  }
}


void aquireJobInfo( int &jobID, int &duration, QUEUE *queue ){
  int currentFront = queue-> front;

  jobID = queue->jobs[currentFront].id;
  duration = queue->jobs[currentFront].duration;
  
  queue->front++;
  queue->front %= queue->size;
}


void processJob( int pid, time_t &start, time_t &end, int jobID, int duration )
{
  int elapsed;

  time(&end);
  elapsed = difftime( end, start );
  cout << "Consumer(" << pid << ") time\t" << elapsed
       << ": Job id " << jobID << " executing sleep duration " 
       << duration << "\n";


  sleep( duration ); // sleep for duration specified by job 
  time(&end);
  elapsed = difftime( end, start );
  cout << "Consumer(" << pid << ") time\t" << elapsed
       << ": Job id " << jobID << " completed\n";
}
