#include "os/memory_constants.h"
#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  sem_t s_procs_completed;                      // Semaphore used to wait until all spawned processes have completed
  int * location = MEM_MAX_VIRTUAL_ADDRESS + 5; //location in memory of process

  if (argc != 2) {
    Printf("Usage: %s <handle_to_procs_completed_semaphore>\n");
    Exit();
  }

  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Test 2 (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }
  //Make the test
  Printf("Test 2 (%d): Acessing Location: (%d)!\n", getpid(), location);
  Printf("Test 2 (%d): (%d)!\n", getpid(), *location);

  // Signal the semaphore to tell the original process that we're done


  Printf("Test 2 (%d): Done!\n", getpid());
}