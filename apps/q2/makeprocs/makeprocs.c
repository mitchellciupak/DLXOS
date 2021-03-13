#include "usertraps.h"
#include "misc.h"

#include "co_inj.h"
#include "s2_inj.h"
#include "shared.h"

#define NUM_MOLS 6

void main (int argc, char *argv[])
{
  int numprocs = 0;               // Used to store number of processes to create
  int i;                          // Loop index variable
  int j;
  mbox_t h_mbox;                  // Used to hold handle to mailbox
  sem_t s_procs_completed;        // Semaphore used to wait until all spawned processes have completed
  char h_mbox_str[10];            // Used as command-line argument to pass mem_handle to new processes
  char s_procs_completed_str[10]; // Used as command-line argument to pass page_mapped handle to new processes
  Molecule s2;
  Molecule co;
  Molecule s;
  Molecule o2;
  Molecule c2;
  Molecule so4;

  Molecule MoleList[NUM_MOLS] = {s2, co, s, o2, c2, so4};

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
  if(s.ct > 2 * o2.ct){
    so4.ct = 2 * o2.ct;
  }
  else{
    so4.ct = s.ct;
  }
  Printf("%d, %d, %d\n", s2.ct, co.ct, so4.ct);
  // Convert string from ascii command line argument to integer number
  numprocs = s2.ct + (int)(co.ct / 4) + so4.ct;

  for(i=0;i<NUM_MOLS;i++){
    // Allocate space for a mailbox
    if ((h_mbox = mbox_create()) == MBOX_FAIL) {
      Printf("makeprocs (%d) molecule (%d): ERROR: could not allocate mailbox!", getpid(), i);
      Exit();
    }

    // Open mailbox to prevent deallocation
    if (mbox_open(h_mbox) == MBOX_FAIL) {
      Printf("makeprocs (%d) molecule (%d): Could not open mailbox %d!\n", getpid(), i, h_mbox);
      Exit();
    }
    MoleList[i].box = h_mbox;
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

  ditoa(s2.box, h_mbox_str);
  for(i=0;i<(s2.ct);i++){
    process_create(S2_INJ_FILE, h_mbox_str, s_procs_completed_str, NULL);
  }

  ditoa(co.box, h_mbox_str);
  for(i=0;i<(c2.ct);i++){
    process_create(CO_INJ_FILE, h_mbox_str, s_procs_completed_str, NULL);
    Printf("CO inj proc %d created\n",i);
  }

  /*
  ditoa(h_mbox, h_mbox_str);
  ditoa(s_procs_completed, s_procs_completed_str);

  // Now we can create the processes.  Note that you MUST end your call to
  // process_create with a NULL argument so that the operating system
  // knows how many arguments you are sending.
  for(i=0; i<numprocs; i++) {
    process_create(FILENAME_TO_RUN, 0, 0, h_mbox_str, s_procs_completed_str, NULL);
    Printf("makeprocs (%d): Process %d created\n", getpid(), i);
  }

  // Send the missile code messages
  for (i=0; i<numprocs; i++) {
    if (mbox_send(h_mbox, sizeof(missile_code), (void *)&mc) == MBOX_FAIL) {
      Printf("Could not send message to mailbox %d in %s (%d)\n", h_mbox, argv[0], getpid());
      Exit();
    }
    Printf("makeprocs (%d): Sent message %d\n", getpid(), i);
  }

  // And finally, wait until all spawned processes have finished.
  if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf("\n");
    Exit();
  }
  mbox_close(h_mbox);
  }*/
  Printf("makeprocs (%d): All other processes completed, exiting main process.\n", getpid());
}
