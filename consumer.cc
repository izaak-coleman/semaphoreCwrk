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
  time_t start, end;
  time(&start);                               // start process time
  shmid_ds shmStatInfo;
  shmid_ds *shmStatInfo_p = &shmStatInfo;

  if( argc != 2) return -1;
  int pid = check_arg( argv[1] );


  int semid = sem_attach( SEM_KEY );          // gain access to semaphore set
  sem_signal( semid, PROCESSES );             // add process to PROCESSES
  cout << "PROCESSES value after starting consumer " << pid << ": " << get_sem_value(semid, PROCESSES ) << "\n";

  int shmid = shm_create( SHM_KEY, SHM_SIZE );
  QUEUE *shmQueue;
  shmQueue = (QUEUE*) shm_attach( SHM_KEY, SHM_R );  // gain access to shared memory



  int processTime, jobID;
  while( true ){
    if( sem_timewait( semid, ITEM, EXIT_TIMEC ) != 0 ){

      sem_wait( semid, PROCESSES );           // remove from PROCESSES
      cout << "Consumer " << pid << " waiting until last process\n";
      cout << "PROCESSES value after ending consumer " << pid << ": " << get_sem_value(semid, PROCESSES ) << "\n";
      sem_zero_wait( semid, PROCESSES );    // wait untill last process finishes
     
      sem_wait( semid, MUTEX ); 

      shmdt( shmQueue );
      shmctl(shmid, IPC_STAT, shmStatInfo_p);
      printf("consumer (%d), attachnum value is %d \n", pid, shmStatInfo_p->shm_nattch);
      if( shmStatInfo_p->shm_nattch == 0 ){
        cout << "Consumer deleting shm and sem...\n";
        shmctl(shmid, IPC_RMID, shmStatInfo_p);
        sem_close( semid );                   // ...destroy semaphore
      }
      
      sem_signal( semid, MUTEX );

      cout << "ending consumer ... \n";
      return 0;                             // if dead wait is true, end process
    }

    sem_wait( semid, MUTEX );                 // request shared memory access

    aquireJobInfo( jobID, processTime, shmQueue );      // get next job duration
    
    sem_signal( semid, MUTEX );               // unlock shared memory
    sem_signal( semid, SPACE );               // signal space has freed up

    processJob( pid, start, end, jobID, processTime );  // process current job
  }

  sem_wait( semid, PROCESSES );               // remove from PROCESSES
  sem_zero_wait( semid, PROCESSES );          // wait untill last process fin


  return -1;                                  // should return from while
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
