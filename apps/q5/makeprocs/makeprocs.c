#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "shared.h"

void main (int argc, char *argv[])
{
  // Init Vars (all inits must be place here)
  molcomp *mc;               // Used to get address of shared memory page
  int numprocs = 0;              // Used to store number of processes to create

  uint32 h_mem;                  // Used to hold handle to shared memory page
  sem_t s_procs_completed;       // Semaphore used to wait until all spawned processes have completed

  char h_mem_str[10];            // Used as command-line argument to pass mem_handle to new processes
  char s_procs_completed_str[10];// Used as command-line argument to pass page_mapped handle to new processes
  int h2oInjectionCount;
  int so4InjectionCount;

  int i = 0;                     // Loop index variable for H20
  int j = 0;                     // Loop index variable for SO4

  // Usage Checking
  if (argc != 3) {
    Printf("Usage: "); Printf(argv[0]); Printf(" <h2oInjectionCount> <so4InjectionCount>\n");
    Exit();
  }

  // Argument Handleing
  h2oInjectionCount = dstrtol(argv[1], NULL, 10);
  Printf("Creating %d H20 processes\n", h2oInjectionCount);

  so4InjectionCount = dstrtol(argv[2], NULL, 10);
  Printf("Creating %d SO4 processes\n", so4InjectionCount);

  // Allocate space for a shared memory page, which is exactly 64KB (Always returns 64KB)
  if ((h_mem = shmget()) == 0) {
    Printf("ERROR: could not allocate shared memory page in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

  // Map shared memory page into this process's memory space
  if ((mc = (molcomp *)shmat(h_mem)) == NULL) {
    Printf("Could not map the shared page to virtual address in "); Printf(argv[0]); Printf(", exiting..\n");
    Exit();
  }

  // Populate buffer with init vals
  mc->h2o_sem = sem_create(0);
  mc->so4_sem = sem_create(0);
  mc->h2_sem = sem_create(0);
  mc->o2_sem = sem_create(0);
  mc->so2_sem = sem_create(0);
  mc->h2so4_sem = sem_create(0);

  // Error Handleing for Sems and Locks Created
  if (mc->h2o_sem == SYNC_FAIL || mc->so4_sem == SYNC_FAIL || mc->h2_sem == SYNC_FAIL || mc->o2_sem == SYNC_FAIL || mc->so2_sem == SYNC_FAIL || mc->h2so4_sem == SYNC_FAIL) {
    Printf("Bad cond in "); Printf(argv[0]); Printf("\n");
    Exit();
  }

  // Create sem to not exit this process until all other processes
  // have signalled that they are complete:
  // - initialize the semaphore to (-1) * (n), where "n" = number of processes - 1.
  // Note: Once each of the processes has signaled, the semaphore should be back to
  // zero and the final sem_wait below will return.
  numprocs = h2oInjectionCount + 1;//3 + h2oInjectionCount + so4InjectionCount;
  if ((s_procs_completed = sem_create(-(numprocs-1))) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }

  // Setup the command-line arguments for the new process.

  // Convert these vars to strings for shared mem page
  ditoa(h_mem, h_mem_str);
  ditoa(s_procs_completed, s_procs_completed_str);

  //Create the processes.
  // Note: that you MUST end your call to process_create with a NULL argument
  // so that the operating system knows how many arguments you are sending.
  while(i < h2oInjectionCount && j < so4InjectionCount){
    if(j < so4InjectionCount){
      Printf("creating process so4\n");
      //process_create(SO4_INJECTIONFILE, h_mem_str, s_procs_completed_str, NULL);
    }
    if(i < h2oInjectionCount){
      Printf("creating process h20\n");
      process_create(H2O_INJECTIONFILE, h_mem_str, s_procs_completed_str, NULL);
    }
    if(i == 0 && j == 0){
      Printf("creating process h2x\n");
      process_create(H2x2_O2_REACTIONFILE, h_mem_str, s_procs_completed_str, NULL);
      //process_create(H2SO4_REACTIONFILE, h_mem_str, s_procs_completed_str, NULL);
      //process_create(SO2_O2_REACTIONFILE, h_mem_str, s_procs_completed_str, NULL);
    }
    Printf("Process %d created\n", i);
    i++; j++;
  }
  // Wait until all spawned processes have finished.
  if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf("\n");
    Exit();
  }

  //Done
  Printf("All processes completed sucessfully, exiting main process.\n");
}
