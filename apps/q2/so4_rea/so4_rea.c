#include "usertraps.h"
#include "misc.h"

#include "so4_rea.h"

void main (int argc, char *argv[]) {

  mbox_t h_mbox_s;             // Handle to the shared memory page
  mbox_t h_mbox_o2;
  mbox_t h_mbox_so4;
  sem_t s_proc_sem;         // Semaphore to signal for sem_signal, based on argument
  sem_t s_procs_completed;  // Semaphore to signal the original process that we're done
  int mol = 0;
  int i;

  // Argument Check
  if (argc != 5) { 
    Printf("Usage: %s <handle_to_mailbox 1>  <handle_to_mailbox 2> <handle_to_mailbox 3> <handle_to_page_mapped_semaphore>\n"); 
    Exit();
  } 

  // Argument Handling
  h_mbox_s = dstrtol(argv[1], NULL, 10);
  h_mbox_o2 = dstrtol(argv[2], NULL, 10);
  h_mbox_so4 = dstrtol(argv[3], NULL, 10);
  s_procs_completed = dstrtol(argv[4], NULL, 10);

  // Open all mailboxes
  if (mbox_open(h_mbox_s) == MBOX_FAIL) {
    Printf("so4_rea (%d): Could not open mailbox %d!\n", getpid(), h_mbox_s);
    Exit();
  }
    if (mbox_open(h_mbox_o2) == MBOX_FAIL) {
    Printf("so4_rea (%d): Could not open mailbox %d!\n", getpid(), h_mbox_o2);
    Exit();
  }
    if (mbox_open(h_mbox_so4) == MBOX_FAIL) {
    Printf("so4_rea (%d): Could not open mailbox %d!\n", getpid(), h_mbox_so4);
    Exit();
  }

  // Recieve 1 S molecule from a mailbox
  if (mbox_recv(h_mbox_s, sizeof(mol), (void *)&mol) == MBOX_FAIL) {
    Printf("so4_rea (%d): Could not map the virtual address to the memory!\n", getpid());
    Exit();
  }

  // Recieve 2 O2 molecules from mailbox
  for(i=0;i<2;i++){
    if (mbox_recv(h_mbox_o2, sizeof(mol), (void *)&mol) == MBOX_FAIL) {
      Printf("so4_rea (%d): Could not map the virtual address to the memory!\n", getpid());
      Exit();
    }
  }

  // Inject 1 SO4 molecule into mailbox
  if (mbox_send(h_mbox_so4, sizeof(mol), (void *)&mol) == MBOX_FAIL) {
    Printf("so4_rea (%d): Could not map the virtual address to the memory!\n", getpid());
    Exit();
  }

  if (mbox_close(h_mbox_s) == MBOX_FAIL) {
    Printf("so4_rea (%d): Could not close mailbox %d!\n", getpid(), h_mbox_s);
    Exit();
  }
    if (mbox_close(h_mbox_o2) == MBOX_FAIL) {
    Printf("so4_rea (%d): Could not close mailbox %d!\n", getpid(), h_mbox_o2);
    Exit();
  }
    if (mbox_close(h_mbox_so4) == MBOX_FAIL) {
    Printf("so4_rea (%d): Could not close mailbox %d!\n", getpid(), h_mbox_so4);
    Exit();
  }

  Printf("S + O2 -> SO4 reacted, PID: %d\n", getpid());
  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

}
