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

/*--------------- Set up process ------------------*/
  time_t start, end;
  int elapsed;
  time(&start);                                 // start process time
  shmid_ds shmStatInfo;                         // shm IPC_STAT info
  shmid_ds *shmStatInfo_p = &shmStatInfo;


  if( argc != 3) return -1;        // check process id and njobs specified
  srand( time(NULL) );             

  int pid, njobs;
  pid = check_arg( argv[1] );            // store process id
  njobs = check_arg( argv[2] );          // store number of jobs


  int semid = sem_attach( SEM_KEY );     // gain access to semaphore set
  sem_signal( semid, PROCESSES );        // add process to PROCESSES
  
  int shmid = shm_create( SHM_KEY, SHM_SIZE );          // get shared memory id
  QUEUE *shmQueue;
  shmQueue = (QUEUE*) shm_attach( SHM_KEY, SHM_W );   // attach to shared memory


/*------------- produce required jobs in while loop ----------*/

  int nextJob = 1;
  while( nextJob <= njobs ) {
    
    /*---------------- if no jobs are consumed within 30min -------------*/
    /*----------------- then exit the program with error ----------------*/
    if( sem_timewait( semid, SPACE, EXIT_TIMEP ) != 0 ){
      sem_wait( semid, PROCESSES );           // remove from PROCESSES COUNT
      /* Dont delete shared memory automatically
         as jobs are still valid. */
      exit(1);                       // exit(1) as no consumer to remove jobs
    }

    /*-------- if jobs are removed before 30min then add more -------*/

    sem_wait( semid, MUTEX );            // lock shared memory to add job
  
    time(&end);
    elapsed = difftime( end, start );    // time elapsed when job added
    
    addJob( pid, elapsed, shmQueue );

    sem_signal( semid, MUTEX );          // unlock shared memory
    sem_signal( semid, ITEM );           // signal item has been added

    int stall = (rand() % 3) + 2;        // random stall time between 2-4s

    if( nextJob == njobs ){              // if last job, print last job msg
      time(&end);
      elapsed = difftime( end, start );
      printf( "Producer(%d) time %2d: No more jobs to generate.\n", 
              pid, elapsed );
    }   
    sleep( stall );                      // stall before add next job
    nextJob++;
  }

  shmdt( shmQueue );                           // detach from shared memory
  shmctl(shmid, IPC_STAT, shmStatInfo_p);      // update shm stat info
  
  sem_wait( semid, PROCESSES );          // remove from PROCESSES 
  sem_zero_wait( semid, PROCESSES );     // wait untill last process finishes
  
  return 0;                               // end producer
}

void addJob( int pid, int time, QUEUE *queue){
  int currentEnd;              
  currentEnd = queue->end;                          // get current end of queue   
  queue->jobs[currentEnd].id = (currentEnd+1);         // assign id to job
  queue->jobs[currentEnd].duration = (rand() % 5) + 2; // assign duration to job

  /* Notify job addition */
  printJob( pid, time, queue->jobs[currentEnd].id, 
            queue->jobs[currentEnd].duration);

  queue->end++;                          // increment end of queue after new job
  queue->end %= queue->size;             // loop circular queue
}

void printJob( int pid, int time, int jobid , int duration ){
  
  printf( "Producer(%d) time %2d: Job id %d duration %d\n",
          pid, time, jobid, duration );
}
