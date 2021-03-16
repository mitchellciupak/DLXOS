#include "usertraps.h"
#include "misc.h"

#include "s_rea.h"

void main (int argc, char *argv[]) {

  mbox_t h_mbox_s2;             // Handle to the shared memory page
  mbox_t h_mbox_s;
  sem_t s_proc_sem;         // Semaphore to signal for sem_signal, based on argument
  sem_t s_procs_completed;  // Semaphore to signal the original process that we're done
  int mol = 0;
  int i;

  // Argument Check
  if (argc != 4) { 
    Printf("Usage: %s <handle_to_mailbox 1>  <handle_to_mailbox 2> <handle_to_page_mapped_semaphore>\n"); 
    Exit();
  } 

  // Argument Handling
  h_mbox_s2 = dstrtol(argv[1], NULL, 10);
  h_mbox_s  = dstrtol(argv[2], NULL, 10);
  s_procs_completed = dstrtol(argv[3], NULL, 10);

  // Open mailboxes
  if (mbox_open(h_mbox_s2) == MBOX_FAIL) {
    Printf("s_rea (%d): Could not open mailbox %d!\n", getpid(), h_mbox_s2);
    Exit();
  }
  if (mbox_open(h_mbox_s) == MBOX_FAIL) {
    Printf("s_rea (%d): Could not open mailbox %d!\n", getpid(), h_mbox_s);
    Exit();
  }
  
  // Recieve one S2 molecule
  if (mbox_recv(h_mbox_s2, sizeof(mol), (void *)&mol) == MBOX_FAIL) {
    Printf("s_rea (%d): Could not map the virtual address to the memory!\n", getpid());
    Exit();
  }

  // Inject 2 S molecules into mailbox
  for(i=0;i<2;i++){
    if (mbox_send(h_mbox_s, sizeof(mol), (void *)&mol) == MBOX_FAIL) {
      Printf("co_rea (%d): Could not map the virtual address to the memory!\n", getpid());
      Exit();
    }
  }
  Printf("S2 -> S + S reacted, PID: %d\n", getpid());

  if (mbox_close(h_mbox_s2) == MBOX_FAIL) {
    Printf("s_rea (%d): Could not close mailbox %d!\n", getpid(), h_mbox_s2);
    Exit();
  }
  if (mbox_close(h_mbox_s) == MBOX_FAIL) {
    Printf("s_rea (%d): Could not close mailbox %d!\n", getpid(), h_mbox_s);
    Exit();
  }

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

}
