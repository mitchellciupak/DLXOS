#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "shared.h"

void main (int argc, char *argv[]) {

  molcomp *mc;          // Used to access buffer chars in shared memory page
  uint32 h_mem;             // Handle to the shared memory page
  sem_t s_procs_completed;  // Semaphore to signal the original process that we're done

  // Argument Check
  if (argc != 3) {
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore>\n");
    Exit();
  }

  // Argument Handleing
  h_mem = dstrtol(argv[1], NULL, 10); //Convert the command-line strings into integers for use as handles in base 10
  s_procs_completed = dstrtol(argv[2], NULL, 10);

  // Map shared memory page into this process's memory space
  if ((mc = (molcomp *)shmat(h_mem)) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

  // Signal the semaphore that another H2O molecule is added
  sem_signal(mc->h2o_sem);
  Printf("H2O injected into Radeon atmosphere, PID: %d\n", getpid());

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}