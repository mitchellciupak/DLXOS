#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "consumer.h"

void main (int argc, char *argv[]) {

  Printf("Consumer"); //TODO - Debug

  // Init Vars (must be declared here)
  buffer_char *bc;         // Used to access buffer chars in shared memory page
  uint32 h_mem;            // Handle to the shared memory page

  int i;
  char str[11] = {'\0'};   //TODO Comment
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done

  // Argument Check
  if (argc != 3) {
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore>\n");
    Exit();
  }

  // Argument Handleing
  h_mem = dstrtol(argv[1], NULL, 10); //Convert the command-line strings into integers for use as handles in base 10
  s_procs_completed = dstrtol(argv[2], NULL, 10);


  // Map shared memory page into this process's memory space
  if ((bc = (buffer_char *)shmget()) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

  // Do it
  while(i < 11) {

    sem_wait(bc->full);
    lock_acquire(bc->lock);

    if(str[bc->tail] == '\0'){

      str[bc->tail] = bc->buff[bc->tail];
      bc->tail = (bc->tail + 1) % BUFF_LEN;
      Printf("Consumer %d removed %c\n",getpid(), str[bc->tail]);

      bc->buff[bc->tail] = '\0';

      i++;
    }

    lock_release(bc->lock);
    sem_signal(bc->empty);
  }




  // Signal the semaphore to tell the original process that we're done
  Printf("Consumer: PID %d is complete.\n", getpid());
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

}
