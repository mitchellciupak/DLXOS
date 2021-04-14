#include "usertraps.h"
#include "misc.h"

//Spawn 30 simultaneous processes that print a message,
// count to a large number in a for loop, and then print
// another message before exiting. You should choose a number
// large enough for counting that all 30 processes end up
// running at the same time (mult of 30). You should not run
// out of memory with 30 processes.


void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  int i = 0;

  if (argc != 2) {
    Printf("Usage: %s <handle_to_procs_completed_semaphore>\n");
    Exit();
  }

  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);

  //Printing initial messege
  Printf("Part 6 (%d): Starting!\n", getpid());

  // Loop up to a large number
  for(i = 0; i<30000; i++) //may need a larger multiple of 30

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Part 6 (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }

  Printf("Part 6 (%d): Done!\n", getpid());
}
