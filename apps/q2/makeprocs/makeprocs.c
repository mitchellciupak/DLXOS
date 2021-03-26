#include "usertraps.h"
#include "misc.h"

#include "co_inj.h"
#include "s2_inj.h"
#include "s_rea.h"
#include "co_rea.h"
#include "so4_rea.h"

#include "shared.h"

#define NUM_MOLS 6

void main (int argc, char *argv[])
{
  int numprocs = 0;               // Used to store number of processes to create
  int i;                          // Loop index variable
  int j;
  mbox_t h_mbox;                  // Used to hold handle to mailbox
  char h_mbox_str1[10];
  char h_mbox_str2[10];
  sem_t s_procs_completed;        // Semaphore used to wait until all spawned processes have completed
  char h_mbox_str[10];            // Used as command-line argument to pass mem_handle to new processes
  char s_procs_completed_str[10]; // Used as command-line argument to pass page_mapped handle to new processes
  char s2_mbox_str[10];
  char co_mbox_str[10];
  char c2_mbox_str[10];
  char o2_mbox_str[10];
  char s_mbox_str[10];
  char so4_mbox_str[10];
  Molecule s2;
  Molecule co;
  Molecule s;
  Molecule o2;
  Molecule c2;
  Molecule so4;

  Molecule *MoleList[NUM_MOLS] = {&s2, &co, &s, &o2, &c2, &so4};

  // Usage Checking
  if (argc != 3) {
    Printf("Usage: "); 
    Printf(argv[0]);
    Printf(" <number of h2o injections> <number of so4 injections>\n");
    Exit();
  }

  // Argument Handling
  s2.ct = dstrtol(argv[1], NULL, 10); // Convert string from ascii command line argument to integer number base 10 
  co.ct = dstrtol(argv[2], NULL, 10);

  // Initialize counts
  s.ct  = 2 * s2.ct;
  o2.ct = 2 * (int)(co.ct / 4);
  c2.ct = 2 * (int)(co.ct / 4);
  if(s.ct > (int) (o2.ct / 2)){
    so4.ct = (int)(o2.ct / 2);
  }
  else{
    so4.ct = s.ct;
  }
  // Convert string from ascii command line argument to integer number
  numprocs = s2.ct + co.ct + s2.ct + (int)(co.ct / 4) + so4.ct;

  for(i=0;i<NUM_MOLS;i++){
    // Allocate space for a mailbox
    h_mbox = mbox_create();
    if (h_mbox == MBOX_FAIL) {
      Printf("makeprocs (%d) molecule (%d): ERROR: could not allocate mailbox!", getpid(), i);
      Exit();
    }
    // Open mailbox to prevent deallocation
    if (mbox_open(h_mbox) == MBOX_FAIL) {
      Printf("makeprocs (%d) molecule (%d): Could not open mailbox %d!\n", getpid(), i, h_mbox);
      Exit();
    }
    MoleList[i]->box = h_mbox;
    
  }

  // Create semaphore to not exit this process until all other processes 
  // have signalled that they are complete.  To do this, we will initialize
  // the semaphore to (-1) * (number of signals), where "number of signals"
  // should be equal to the number of processes we're spawning - 1.  Once 
  // each of the processes has signaled, the semaphore should be back to
  // zero and the final sem_wait below will return.
  if ((s_procs_completed = sem_create(-(numprocs-1))) == SYNC_FAIL) {
    Printf("makeprocs (%d): Bad sem_create\n", getpid());
    Exit();
  }

  // Setup the command-line arguments for the new process.  We're going to
  // pass the handles to the shared memory page and the semaphore as strings
  // on the command line, so we must first convert them from ints to strings.
  ditoa(s_procs_completed, s_procs_completed_str);
  ditoa(s2.box, s2_mbox_str);
  ditoa(co.box, co_mbox_str);
  ditoa(c2.box, c2_mbox_str);
  ditoa(o2.box, o2_mbox_str);
  ditoa(s.box, s_mbox_str);
  ditoa(so4.box, so4_mbox_str);

  Printf("Creating %d S2s and %d CO's.\n", s2.ct, co.ct);

  i = (s2.ct > co.ct) ? s2.ct : co.ct;
  i = (i > so4.ct) ? i : so4.ct;
  for(i;i>=0;i--){
    if(i < s2.ct) process_create(S2_INJ_FILE, 0, 0, s2_mbox_str, s_procs_completed_str, NULL);
    if(i < co.ct) process_create(CO_INJ_FILE, 0, 0, co_mbox_str, s_procs_completed_str, NULL);
    if(i < (int) (co.ct / 4)) process_create(CO_REA_FILE, 0, 0, co_mbox_str, c2_mbox_str, o2_mbox_str, s_procs_completed_str, NULL);
    if(i < s2.ct) process_create(S_REA_FILE, 0, 0, s2_mbox_str, s_mbox_str, s_procs_completed_str, NULL);
    if(i < so4.ct) process_create(SO4_REA_FILE, 0, 0, s_mbox_str, o2_mbox_str, so4_mbox_str, s_procs_completed_str, NULL);
  }

  // Wait until all spawned processes have finished.
  if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf("\n");
    Exit();
  }

  Printf("%d S2's left over. %d CO's left over. %d S's left over. %d C2's left over. %d O2's left over. %d SO4's created.\n", \
        0, co.ct % 4, s.ct - so4.ct, c2.ct, o2.ct - 2*so4.ct, so4.ct);

  for(i=0;i<NUM_MOLS;i++){
    // Close mailbox to deallocate
    if (mbox_close(MoleList[i]->box) == MBOX_FAIL) {
      Printf("makeprocs (%d) molecule (%d): Could not open mailbox %d!\n", getpid(), i, h_mbox);
      Exit();
    }    
  }
  Printf("makeprocs (%d): All other processes completed, exiting main process.\n", getpid());
}
