#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "reactor.h"

void main (int argc, char *argv[]) 
{
  molcomp *mc;             // Used to access struct in shared memory page
  uint32 h_mem;            // Handle to the shared memory page
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  sem_t mol_0;
  sem_t mol_1;
  sem_t mol_2;
  int reaction_id; // 0 is so4, 1 is h20, 2 is h2so4
  int num_reactions = 0;
  int i = 0;

  // Argument Check
  if (argc != 5) {
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore> \
            <molecule 1> <(optional) molecule 2> <(optional) molecule 3>\n");
    Exit();
  }

  // Argument Handleing
  h_mem             = dstrtol(argv[1], NULL, 10); //Convert the command-line strings into integers for use as handles in base 10
  s_procs_completed = dstrtol(argv[2], NULL, 10);
  reaction_id       = dstrtol(argv[3], NULL, 10);
  num_reactions     = dstrtol(argv[4], NULL, 10);

  // Map shared memory page into this process's memory space
  if ((mc = (molcomp *)shmat(h_mem)) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

  // Do it
  for(i=0;i<num_reactions;i++){
    if(reaction_id == 0){
      sem_wait(mc->so4_sem);  // SO4
      sem_signal(mc->so2_sem);
      sem_signal(mc->o2_sem);
      Printf("SO4 -> SO2 + O2 reacted, PID: %d\n", getpid());
    }
    else if(reaction_id == 1){  // 2 H2O
      sem_wait(mc->h2o_sem);
      sem_wait(mc->h2o_sem);
      sem_signal(mc->h2_sem);
      sem_signal(mc->h2_sem);
      sem_signal(mc->o2_sem);
      Printf("2 H2O -> 2 H2 + O2 reacted, PID: %d\n", getpid());
    }
    else if(reaction_id == 2){ // H2SO4
      sem_wait(mc->h2_sem);
      sem_wait(mc->o2_sem);
      sem_wait(mc->so2_sem);
      sem_signal(mc->h2so4_sem);
      Printf("(%d) H2 + O2 + SO2 -> H2SO4 reacted, PID: %d\n", i+1, getpid());
    }
  }

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

}
