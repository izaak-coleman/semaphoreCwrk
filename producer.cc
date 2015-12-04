// Producer
#include <iostream>
#include "helper.h"

using namespace std;

void addJob( int pid, int time, QUEUE *queue );
/* Assigns a new job to the shm QUEUE->jobs array
 * randomly generates a duration time for 
 * the job. */

void printJob( int pid, int time, int jobid , int duration );
/* Print the process id, the job id and the time 
 * the job was added to the shm QUEUE array.
 * also prints the duration of the job.
 * Once job is added, pointer to next empty
 * space (QUEUE->end) is moved forward. */

int main (int argc, char *argv[])
{
  time_t start, end;
  int elapsed;
  time(&start);                          // start process time
  shmid_ds shmStatInfo;
  shmid_ds *shmStatInfo_p = &shmStatInfo;


  if( argc != 3) return -1;
  srand( time(NULL));

  int pid, njobs;
  pid = check_arg( argv[1] );            
  njobs = check_arg( argv[2] );          // number of jobs p will add to queue


  int semid = sem_attach( SEM_KEY );     // gain access to semaphore set
  sem_signal( semid, PROCESSES );        // add process to PROCESSES
  cout << "PROCESSES value after starting procuder " << pid << ": " << get_sem_value(semid, PROCESSES ) << "\n";
  
  int shmid = shm_create( SHM_KEY, SHM_SIZE );
  QUEUE *shmQueue;
  shmQueue = (QUEUE*) shm_attach( SHM_KEY, SHM_W );     // gain access to shared memory




  int nextJob = 1;
  while( nextJob <= njobs ) {
    
    if( sem_timewait( semid, SPACE, EXIT_TIMEP ) != 0 ){
      sem_wait( semid, PROCESSES );
      return -1;                         // non-0 return as job process timeout 
    }
    /*sleep if no space*/
    sem_wait( semid, MUTEX );            // lock shared memory to add job
  
    time(&end);
    elapsed = difftime( end, start );
    
    addJob( pid, elapsed, shmQueue );

    sem_signal( semid, MUTEX );          // unlock shared memory
    sem_signal( semid, ITEM );           // signal item has been added

    int stall = (rand() % 3) + 2;
    sleep( stall );                      // stall between 2-4s before request
    nextJob++;

  }
  cout << "Producer finished all jobs. So detatching from memory\n";
  shmdt( shmQueue );                     // detach from shared memory
  shmctl(shmid, IPC_STAT, shmStatInfo_p);
  
  cout << "Producer " << pid << " waiting until last process\n";
  sem_wait( semid, PROCESSES );          // remove from PROCESSES 
  cout << "PROCESSES value after ending procuder " << pid << ": " << get_sem_value(semid, PROCESSES ) << "\n";
  sem_zero_wait( semid, PROCESSES );     // wait untill last process finishes
  
  cout << "Producer ending. \n";
  return 0;
}

void addJob( int pid, int time, QUEUE *queue){
  int end;              
  end = queue->end;                      // get current end of queue   
  queue->jobs[end].id = (end+1);         // assign id
  queue->jobs[end].duration = (rand() % 5) + 2;  // assign duration

  /* Notify job addition */
  printJob( pid, time, queue->jobs[end].id, queue->jobs[end].duration);

  queue->end++;                          // increment end of queue after new job
  queue->end %= queue->size;             // loop circular queue
}

void printJob( int pid, int time, int jobid , int duration ){

  cout << "Producer(" << pid << ") time\t" << time << ": Job id " << jobid
       << " duration " << duration << "\n";
}
