#include "usertraps.h"
#include "misc.h"

#include "s2_inj.h"

void main (int argc, char *argv[]) {

  mbox_t h_mbox;             // Handle to the shared memory page
  sem_t s_proc_sem;         // Semaphore to signal for sem_signal, based on argument
  sem_t s_procs_completed;  // Semaphore to signal the original process that we're done
  int mol = 0;

  // Argument Check
  if (argc != 3) { 
    Printf("Usage: %s <handle_to_mailbox> <handle_to_page_mapped_semaphore>\n"); 
    Exit();
  } 

  // Argument Handleing
  h_mbox = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  s_procs_completed = dstrtol(argv[2], NULL, 10);

  // Just inject s2 once into mailbox
  if (mbox_open(h_mbox) == MBOX_FAIL) {
    Printf("makeprocs (%d): Could not open mailbox %d!\n", getpid(), h_mbox);
    Exit();
  }
  if (mbox_send(h_mbox, sizeof(mol), (void *)&mol) == MBOX_FAIL) {
    Printf("Could not send message to mailbox %d in %s (%d)\n", h_mbox, argv[0], getpid());
    Exit();
  }
  Printf("S2 Injected into Radeon atmosphere, PID: %d\n", getpid());

  if (mbox_close(h_mbox) == MBOX_FAIL) {
    Printf("makeprocs (%d): Could not close mailbox %d!\n", getpid(), h_mbox);
    Exit();
  }
  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

}
