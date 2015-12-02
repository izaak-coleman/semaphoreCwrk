// Consumer
#include <iostream>
#include "helper.h"

using namespace std;

void consumeJob( int pid, time_t &start, time_t &end, QUEUE *queue );
/* Runs the next job in shm QUEUE array, and prints
 * status before and after job execution. 
 * Once job is complete, job is deallocated, by
 * incrementing pointer to next job (QUEUE->front), passed the 
 * completed job */

int main (int argc, char *argv[])
{
  time_t start, end;
  time(&start);                                     // start process time

  if( argc != 2) return -1;

  int pid = check_arg( argv[1] );

  int semid = sem_attach( SEM_KEY );          // gain access to semaphore set
  QUEUE *shmQueue;
  shmQueue = (QUEUE*) shm_attach( SHM_KEY );  // gain access to shared memory

  while( true ){
  int processTime;
  
  sem_wait( semid, ITEM );               // wait for an item to become available
  /*sleep if no items available*/
  sem_wait( semid, MUTEX );              // request shared memory access

  processTime = consumeJob( pid, start, end, shmQueue );    // compute "process"
  
  sem_signal( semid, MUTEX );            // unlock shared memory
  sem_signal( semid, SPACE );            // signal space has freed up

  sleep( processTime ); // sleep for duration specified by job 
  time(&end);
  elapsed = difftime( end, start );
  cout << "Consumer(" << pid << ") time\t" << elapsed
       << ": Job id " << queue->jobs[front].id
       << " completed\n";

  }
  
  return 0;
}

int consumeJob( int pid, time_t &start, time_t &end, QUEUE *queue ){

  int front;
  time(&end);
  int elapsed = difftime( end, start );
  front = queue->front;

  cout << "Consumer(" << pid << ") time\t" << elapsed
       << ": Job id " << queue->jobs[front].id
       << " executing sleep duration " << queue->jobs[front].duration
       << "\n";

  queue->front++;                   // increment start of queue one executed job
  queue-> front %= queue->size;     // loop circular queue 

  return queue->jobs[front].duration;
}
