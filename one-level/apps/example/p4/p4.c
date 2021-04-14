#include "usertraps.h"
#include "misc.h"

//overload callstack recursively
int summation(int x) {

  if (n != 0){
    return n + sum(n-1);
  }

  return n;
}

void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  int testval = 10;

  if (argc != 2) {
    Printf("Usage: %s <handle_to_procs_completed_semaphore>\n");
    Exit();
  }

  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);

  // Now print a message to show that everything worked
  Printf("Par 4 (%d): Starting Overload Test!\n", getpid());
  Printf("Summaion of %d is %d", testval, summation(testval));

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Part 4 (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }

  Printf("Part 4 (%d): Done!\n", getpid());
}
