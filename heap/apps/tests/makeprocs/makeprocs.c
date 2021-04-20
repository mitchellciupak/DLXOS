#include "usertraps.h"
#include "misc.h"

#define TEST_MALLOC "test_malloc.dlx.obj"

void main (int argc, char *argv[])
{
  int num_tests = 0;             // Used to store number of processes to create
  int i;                               // Loop index variable
  int* a;
  sem_t s_procs_completed;             // Semaphore used to wait until all spawned processes have completed
  char s_procs_completed_str[10];      // Used as command-line argument to pass page_mapped handle to new processes
  char num_tests_str[2];

  if (argc != 2) {
    Printf("Usage: %s <number of hello world processes to create>\n", argv[0]);
    Exit();
  }

  // Convert string from ascii command line argument to integer number
  num_tests = dstrtol(argv[1], NULL, 10); // the "10" means base 10
  Printf("makeprocs (%d): Creating %d test processes\n", getpid(), num_tests);

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
  
  // Create Hello World processes
  Printf("-------------------------------------------------------------------------------------\n");
  for(i=0; i<num_tests; i++) {
    ditoa(i, num_tests_str);
    process_create(TEST_MALLOC, s_procs_completed_str, num_tests_str, NULL);
    if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
      Printf("Bad semaphore s_procs_completed (%d) in %s\n", s_procs_completed, argv[0]);
      Exit();
    }
  }

  Printf("-------------------------------------------------------------------------------------\n");
  Printf("makeprocs (%d): All other processes completed, exiting main process.\n", getpid());

}
