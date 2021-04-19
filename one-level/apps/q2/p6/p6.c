#include "os/memory_constants.h"
#include "usertraps.h"
#include "misc.h"

/*
6 - Spawn 30 simultaneous processes that print a message, count to a large number
    in a for loop, and then print another message before exiting. You should
    choose a number large enough for counting that all 30 processes end up
    running at the same time. You should not run out of memory with 30 processes.
*/

void main (int argc, char *argv[])
{
  sem_t s_procs_completed;                      // Semaphore used to wait until all spawned processes have completed
  int i = 0;
  int largeNumber = 500000;

  if (argc != 2) {
    Printf("Usage: %s <handle_to_procs_completed_semaphore>\n");
    Exit();
  }

  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);

  //Make the test
  Printf("Test 6 (%d): BEFORE LOOP!\n", getpid());
  for(i=0; i<largeNumber;i++);
  Printf("Test 6 (%d): AFTER LOOP!\n", getpid());

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Test 2 (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }

  Printf("Test 2 (%d): Done!\n", getpid());
}