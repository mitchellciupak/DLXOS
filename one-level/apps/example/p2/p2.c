#include "os/memory_constants.h"
#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  int * location: //location in memory of process

  if (argc != 2) {
    Printf("Usage: %s <handle_to_procs_completed_semaphore>\n");
    Exit();
  }

  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);

  // Now print a message to show that everything worked
  Printf("Part 2 (%d): Out of Max Virtual Address being Attempted!\n", getpid());

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Part 2 (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }

  location = MEM_MAX_SIZE >> 1;
  Printf("Part 2 (%d): Acessing Location: (%d)!\n", getpid(), location);
  Printf("Part 2 (%d): (%d)!\n", getpid(), *location);
  Printf("Part 2 (%d): Done!\n", getpid());
}
