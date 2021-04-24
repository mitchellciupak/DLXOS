#include "usertraps.h"
#include "misc.h"

#include "shared.h"

int run_test(char file[15], int num){
  int i;
  sem_t s_procs_completed;             // Semaphore used to wait until all spawned processes have completed
  char s_procs_completed_str[10];      // Used as command-line argument to pass page_mapped handle to new processes

  // Create semaphore to not exit this process until all other processes
  // have signalled that they are complete.
  if ((s_procs_completed = sem_create(0)) == SYNC_FAIL) {
    Printf("makeprocs (%d): Bad sem_create\n", getpid());
    Exit();
  }

  // Setup the command-line arguments for the new processes.  We're going to
  // pass the handles to the semaphore as strings
  // on the command line, so we must first convert them from ints to strings.
  ditoa(s_procs_completed, s_procs_completed_str);
  for(i=0; i<num; i++) {
    process_create(file, s_procs_completed_str, NULL);
    Printf("%d\n", i);
    if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
      Printf("Bad semaphore s_procs_completed (%d) in\n", s_procs_completed);
      Exit();
    }
  }

}

void main (int argc, char *argv[])
{
  int numprocs = 30;             // Used to store number of processes to create
  int i;                               // Loop index variable
  sem_t s_procs_completed;             // Semaphore used to wait until all spawned processes have completed
  char s_procs_completed_str[10];

  if (argc != 1) {
    Printf("Usage: %s \n", argv[0]);
    Exit();
  }

  Printf("Part 1 - Starting");
  run_test(P1_FILE, 1);
  Printf("Part 1 - Ending\n");

  run_test(P2_FILE, 1);

  run_test(P3_FILE, 1);

  run_test(P4_FILE, 1);

  Printf("Part 5 - Startin\n");
  run_test(P1_FILE, 100);
  Printf("Part 5 - Ending\n");

  if ((s_procs_completed = sem_create(-(numprocs - 1))) == SYNC_FAIL) {
    Printf("makeprocs (%d): Bad sem_create\n", getpid());
    Exit();
  }

  ditoa(s_procs_completed, s_procs_completed_str);
  for(i=0; i<numprocs; i++) {
    process_create(P6_FILE, s_procs_completed_str, NULL);
  }

  if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in\n", s_procs_completed);
    Exit();
  }

  Printf("-------------------------------------------------------------------------------------\n");
  Printf("makeprocs (%d): All other processes completed, exiting main process.\n", getpid());

}
