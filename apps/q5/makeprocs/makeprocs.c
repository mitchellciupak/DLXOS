#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "reactor.h"
#include "injector.h"

void main (int argc, char *argv[])
{
  // Init Vars (all inits must be place here)
  molcomp *mc;
  int numprocs = 0;              // Used to store number of processes to create

  uint32 h_mem;                  // Used to hold handle to shared memory page
  sem_t s_procs_completed;       // Semaphore used to wait until all spawned processes have completed

  char h_mem_str[10];            // Used as command-line argument to pass mem_handle to new processes
  char s_procs_completed_str[10];// Used as command-line argument to pass page_mapped handle to new processes
  char count_str[10];

  int h2o_inj_ct = 0;
  int so4_inj_ct = 0;
  int h2o_rea_ct = 0;
  int so4_rea_ct = 0;
  int hos_rea_ct = 0;
  int h2_ct = 0;
  int o2_ct = 0;
  int so_ct = 0;

  // Usage Checking
  if (argc != 3) {
    Printf("Usage: "); Printf(argv[0]); Printf(" <number of h2o injections> <number of so4 injections>\n");
    Exit();
  }

  // Argument Handling
  // H2O reaction
  h2o_inj_ct = dstrtol(argv[1], NULL, 10); // Convert string from ascii command line argument to integer number base 10 
  h2o_rea_ct = (int)(h2o_inj_ct / 2);
  h2_ct = h2o_rea_ct * 2;
  o2_ct = h2o_rea_ct;

  // SO4 Reaction
  so4_inj_ct = dstrtol(argv[1], NULL, 10);
  so4_rea_ct = so4_inj_ct;
  so_ct =  so4_inj_ct;
  o2_ct += so4_inj_ct;

  // H2SO4 Reaction
  while( h2_ct > 0 && o2_ct > 0 && so_ct > 0 ){
    hos_rea_ct++;
    h2_ct--; so_ct--; o2_ct--;
  }
  //Printf("Creating %d processes\n", h2o_rea_ct);


  // Print 'Hello World' for 'n' producers and 'n' consumers (printers)

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
  // Initialize all semaphores in structure to zero
  mc->so2_sem   = sem_create(0);
  mc->h2_sem    = sem_create(0);
  mc->h2o_sem   = sem_create(0);
  mc->h2so4_sem = sem_create(0);
  mc->so4_sem   = sem_create(0);
  mc->o2_sem    = sem_create(0);

  // Error Handleing for Sems Created

  // Create sem to not exit this process until all other processes
  // have signalled that they are complete:
  // - initialize the semaphore to (-1) * (n), where "n" = number of processes - 1.
  // Note: Once each of the processes has signaled, the semaphore should be back to
  // zero and the final sem_wait below will return.
  numprocs = 5;
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
  ditoa(so4_inj_ct, count_str);
  process_create(INJECTOR_FILE, h_mem_str, s_procs_completed_str, "0", count_str, NULL);
  ditoa(h2o_inj_ct, count_str);
  process_create(INJECTOR_FILE, h_mem_str, s_procs_completed_str, "1", count_str, NULL);
  ditoa(so4_rea_ct, count_str);
  process_create(REACTOR_FILE,  h_mem_str, s_procs_completed_str, "0", count_str, NULL);
  ditoa(h2o_rea_ct, count_str);
  process_create(REACTOR_FILE,  h_mem_str, s_procs_completed_str, "1", count_str, NULL);
  ditoa(hos_rea_ct, count_str);
  process_create(REACTOR_FILE,  h_mem_str, s_procs_completed_str, "2", count_str, NULL);

  // Wait until all spawned processes have finished.
  if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf("\n");
    Exit();
  }

  Printf("%d H2O's left over. ", (h2o_inj_ct % 2));
  Printf("%d H2's left over. ", h2_ct);
  Printf("%d O2's left over. %d SO2's left over. %d H2SO4's created.\n", o2_ct, so_ct, hos_rea_ct);
  //Done
  Printf("All processes completed sucessfully, exiting main process.\n");
}
