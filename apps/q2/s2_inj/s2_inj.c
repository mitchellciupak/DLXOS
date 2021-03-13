#include "usertraps.h"
#include "misc.h"

#include "s2_inj.h"

void main (int argc, char *argv[]) {

  uint32 h_mem;             // Handle to the shared memory page
  sem_t s_proc_sem;         // Semaphore to signal for sem_signal, based on argument
  sem_t s_procs_completed;  // Semaphore to signal the original process that we're done

  // Argument Check
  Printf("%d", argc);
  if (argc != 4) {
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore> <injectionID> <numberofinjections>\n");
    Exit();
  }

  // Argument Handleing
  h_mem              = dstrtol(argv[1], NULL, 10);
  s_procs_completed  = dstrtol(argv[2], NULL, 10);

  // Map shared memory page into this process's memory space
  /*if ((mc = (molcomp *)shmat(h_mem)) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

  // Just inject s2 once into mailbox
  */
  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

}
