#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "producer.h"

void main (int argc, char *argv[]) {


  Printf("Producer"); //TODO - Debug

  // Init Vars (must be declared here)
  buffer_char *bc;          // Used to access buffer chars in shared memory page
  uint32 h_mem;             // Handle to the shared memory page

  char fill[11] = "Hello World"; //Char String
  int i = 0;                     // Counter for for loop

  sem_t s_procs_completed;  // Semaphore to signal the original process that we're done

  // Argument Check
  if (argc != 3) {
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore>\n");
    Exit();
  }

  // Argument Handleing
  h_mem = dstrtol(argv[1], NULL, 10); //Convert the command-line strings into integers for use as handles in base 10
  s_procs_completed = dstrtol(argv[2], NULL, 10);

  // Map shared memory page into this process's memory space
  if ((bc = (buffer_char *)shmat(h_mem)) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

  // Do it
  while(i < 11) {

    sem_wait(bc->empty);
    lock_acquire(bc->lock);

    if(fill[bc->head] != '\0'){

      bc->buff[bc->head] = fill[bc->head];
      bc->head = (bc->head + 1) % BUFF_LEN;
      Printf("Producer %d inserted %c\n",getpid(), fill[bc->head]);

      fill[bc->head] = '\0';

      i++;
    }

    lock_release(bc->lock);
    sem_signal(bc->full);
  }


  // Signal the semaphore to tell the original process that we're done
  Printf("Producer: PID %d is complete.\n", getpid());
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

  }