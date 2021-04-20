#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  int test_num;
  int i;
  int* heap_idcs[128];

  if (argc != 3) { 
    Printf("Usage: %s <handle_to_procs_completed_semaphore>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);
  test_num = dstrtol(argv[2], NULL, 10);

  // Now test based on number
  if(test_num == 1){
    Printf("Test %d: filling the heap\n", test_num);
    for(i=0;i<128;i++){
      heap_idcs[i] = malloc(32);
    }
    malloc(1);
    for(i=0;i<128;i++){
      mfree(heap_idcs[i]);
    }
  }
  // Now test based on number
  else if(test_num == 0){
    Printf("Test %d: freeing full heap\n", test_num);
    heap_idcs[0] = malloc(4096);
    mfree(heap_idcs[0]);
    heap_idcs[1] = malloc(2);
    
  }
  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("hello_world (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }

  Printf("test_malloc (%d): Done!\n", getpid());
}
