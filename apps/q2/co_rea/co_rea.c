#include "usertraps.h"
#include "misc.h"

#include "co_rea.h"

void main (int argc, char *argv[]) {

  mbox_t h_mbox_co;             // Handle to the shared memory page
  mbox_t h_mbox_c2;
  mbox_t h_mbox_o2;
  sem_t s_proc_sem;         // Semaphore to signal for sem_signal, based on argument
  sem_t s_procs_completed;  // Semaphore to signal the original process that we're done
  int i;
  int mol = 0;

  // Argument Check
  if (argc != 5) {
    Printf("Usage: %s <handle_to_mailbox 1>  <handle_to_mailbox 2> <handle_to_mailbox 3> <handle_to_page_mapped_semaphore>\n");
    Exit();
  }

  // Argument Handling
  h_mbox_co = dstrtol(argv[1], NULL, 10);
  h_mbox_c2 = dstrtol(argv[2], NULL, 10);
  h_mbox_o2 = dstrtol(argv[3], NULL, 10);
  s_procs_completed = dstrtol(argv[4], NULL, 10);

  if (mbox_open(h_mbox_co) == MBOX_FAIL) {
    Printf("co_rea (%d): Could not open mailbox %d!\n", getpid(), h_mbox_co);
    Exit();
  }
  if (mbox_open(h_mbox_c2) == MBOX_FAIL) {
    Printf("co_rea (%d): Could not open mailbox %d!\n", getpid(), h_mbox_c2);
    Exit();
  }
  if (mbox_open(h_mbox_o2) == MBOX_FAIL) {
    Printf("co_rea (%d): Could not open mailbox %d!\n", getpid(), h_mbox_o2);
    Exit();
  }

  // Recieve 4 COs from mailbox
  for(i=0;i<4;i++){
    if (mbox_recv(h_mbox_co, sizeof(mol), (void *)&mol) == MBOX_FAIL) { //TODO: What is size of mol
      Printf("co_rea (%d): Could not map the virtual address to the memory!\n", getpid());
      Exit();
     }
  }

  // Inject 2 C2s into mailbox
  for(i=0;i<2;i++){
    if (mbox_send(h_mbox_c2, sizeof(mol), (void *)&mol) == MBOX_FAIL) {
      Printf("co_rea (%d): Could not map the virtual address to the memory!\n", getpid());
      Exit();
     }
  }

  // Inject 2 O2 into mailbox
  for(i=0;i<2;i++){
    if (mbox_send(h_mbox_o2, sizeof(mol), (void *)&mol) == MBOX_FAIL) {
      Printf("co_rea (%d): Could not map the virtual address to the memory!\n", getpid());
      Exit();
     }
  }
  Printf("4CO -> 2C2 + 2O2 reacted, PID: %d\n", getpid());

  if (mbox_close(h_mbox_co) == MBOX_FAIL) {
    Printf("co_rea (%d): Could not close mailbox %d!\n", getpid(), h_mbox_co);
    Exit();
  }
  if (mbox_close(h_mbox_c2) == MBOX_FAIL) {
    Printf("co_rea (%d): Could not close mailbox %d!\n", getpid(), h_mbox_c2);
    Exit();
  }
  if (mbox_close(h_mbox_o2) == MBOX_FAIL) {
    Printf("co_rea (%d): Could not close mailbox %d!\n", getpid(), h_mbox_o2);
    Exit();
  }

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

}
