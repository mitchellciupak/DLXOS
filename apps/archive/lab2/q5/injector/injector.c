#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "injector.h"

void main (int argc, char *argv[]) {

  molcomp *mc;              // Used to access struct in shared memory page
  uint32 h_mem;             // Handle to the shared memory page
  sem_t s_proc_sem;         // Semaphore to signal for sem_signal, based on argument
  sem_t s_procs_completed;  // Semaphore to signal the original process that we're done
  int injectionID;               // Index for selectSignal
  int i;                    // Counter for injection creation loop
  int num_injections;

  // Argument Check
  if (argc != 5) {
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore> <injectionID> <numberofinjections>\n");
    Exit();
  }

  // Argument Handleing
  h_mem              = dstrtol(argv[1], NULL, 10);
  s_procs_completed  = dstrtol(argv[2], NULL, 10);
  injectionID        = dstrtol(argv[3], NULL, 10);
  num_injections     = dstrtol(argv[4], NULL, 10);

  // Map shared memory page into this process's memory space
  if ((mc = (molcomp *)shmat(h_mem)) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
  s_proc_sem = mc->so4_sem;
  if(injectionID == 0){ s_proc_sem = mc->so4_sem;}
  else if(injectionID == 1){ s_proc_sem = mc->h2o_sem;}

  // Do thoes number of injections
  for(i=0;i<num_injections;i++){
      if(sem_signal(s_proc_sem) != SYNC_SUCCESS) {
			  Printf("Bad semaphore in %d", s_proc_sem);
			  Exit();
		  }

      if(injectionID == 0){Printf("SO4 injected into Radeon atmosphere, PID: %d\n",getpid());}
      else if(injectionID == 1){   Printf("H2O injected into Radeon atmosphere, PID: %d\n",getpid());}
  }

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

}
