#include "usertraps.h"
#include "misc.h"

#define HELLO_WORLD "hello_world.dlx.obj"
#define TEST_2 "p2.dlx.obj"
#define TEST_3 "p3.dlx.obj"
#define TEST_4 "p4.dlx.obj"
#define TEST_6 "p6.dlx.obj"

/*
[x] 1 - Print "Hello World" and exit. (/make.sh -R example/ run_test1 > make.txt)
[x] 2 - Access memory beyond the maximum virtual address. /make.sh -R example/ run_test2 > make.txt
[x] 3 - Access memory inside the virtual address space, but outside of currently
    allocated pages. /make.sh -R example/ run_test3 > make.txt
[x] 4 - Cause the user function call stack to grow larger than 1 page.
[18] 5 - Call the "Hello World" program 100 times to make sure you are rightly
    allocating and freeing pages.
[x] 6 - Spawn 30 simultaneous processes that print a message, count to a large number
    in a for loop, and then print another message before exiting. You should
    choose a number large enough for counting that all 30 processes end up
    running at the same time. You should not run out of memory with 30 processes.
*/

void main (int argc, char *argv[])
{
  int numprocs = 0;                    // Used to store number of processes to create
  int numtest = 0;                     // Used to store the of test to run
  int i;                               // Loop index variable
  int sem_val;                         // Init val of sem
  sem_t s_procs_completed;             // Semaphore used to wait until all spawned processes have completed
  char s_procs_completed_str[10];      // Used as command-line argument to pass page_mapped handle to new processes

  if (argc != 3) {
    Printf("Usage: %s <test to run> <number processes to create>\n", argv[0]);
    Exit();
  }

  // Convert string from ascii command line argument to integer number
  numtest = dstrtol(argv[1], NULL, 10); // the "10" means base 10
  numprocs = dstrtol(argv[2], NULL, 10); // the "10" means base 10
  Printf("makeprocs (%d): Creating %d processes for test %d\n", getpid(), numprocs, numtest);

  // Create semaphore to not exit this process until all other processes
  // have signalled that they are complete.
  sem_val = -1 * (numprocs - 1);
  if ((s_procs_completed = sem_create(sem_val)) == SYNC_FAIL) {
    Printf("makeprocs (%d): Bad sem_create\n", getpid());
    Exit();
  }

  // Setup the command-line arguments for the new processes.  We're going to
  // pass the handles to the semaphore as strings
  // on the command line, so we must first convert them from ints to strings.
  ditoa(s_procs_completed, s_procs_completed_str);

  Printf("-------------------------------------------------------------------------------------\n");

  if((numtest == 1) | (numtest == 5)) {
    //TODO - ONLY 18/100 Process Work due to an undetermined error
    // Create Hello World processes
    Printf("makeprocs (%d): Creating %d hello world's in a row, but only one runs at a time\n", getpid(), numprocs);
    for(i=0; i<numprocs; i++) {
      Printf("makeprocs (%d): Creating hello world #%d\n", getpid(), i);
      process_create(HELLO_WORLD, s_procs_completed_str, NULL);
      if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
        Printf("Bad semaphore s_procs_completed (%d) in %s\n", s_procs_completed, argv[0]);
        Exit();
      }
    }
  }
  else if(numtest == 2){
    Printf("makeprocs (%d): Creating %d test 2 processes (ran one at a time)\n", getpid(), numprocs);
    for(i=0; i<numprocs; i++) {
      Printf("makeprocs (%d): Creating test 2 #%d\n", getpid(), i);
      process_create(TEST_2, s_procs_completed_str, NULL);
      if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
        Printf("Bad semaphore s_procs_completed (%d) in %s\n", s_procs_completed, argv[0]);
        Exit();
      }
    }
  }
  else if(numtest == 3){
    Printf("makeprocs (%d): Creating %d Test 3 processes (ran one at a time)\n", getpid(), numprocs);
    for(i=0; i<numprocs; i++) {
      Printf("makeprocs (%d): Creating Test 3 #%d\n", getpid(), i);
      process_create(TEST_3, s_procs_completed_str, NULL);
      if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
        Printf("Bad semaphore s_procs_completed (%d) in %s\n", s_procs_completed, argv[0]);
        Exit();
      }
    }
  }
  else if(numtest == 4){
    Printf("makeprocs (%d): Creating %d test 4 processes (ran one at a time)\n", getpid(), numprocs);
    for(i=0; i<numprocs; i++) {
      Printf("makeprocs (%d): Creating test 4 #%d\n", getpid(), i);
      process_create(TEST_4, s_procs_completed_str, NULL);
      if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
        Printf("Bad semaphore s_procs_completed (%d) in %s\n", s_procs_completed, argv[0]);
        Exit();
      }
    }
  }
  else if(numtest == 6){
    //TODO - NOT DONE
    Printf("makeprocs (%d): Creating %d test 6 processes (ran one at a time)\n", getpid(), numprocs);
    for(i=0; i<numprocs; i++) {
      Printf("makeprocs (%d): Creating test 6 #%d\n", getpid(), i);
      process_create(TEST_6, s_procs_completed_str, NULL);

    }

    if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
        Printf("Bad semaphore s_procs_completed (%d) in %s\n", s_procs_completed, argv[0]);
        Exit();
    }
  }

  Printf("-------------------------------------------------------------------------------------\n");
  Printf("makeprocs (%d): All other processes completed, exiting main process.\n", getpid());

}
