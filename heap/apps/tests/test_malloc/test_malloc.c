#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  int test_num;
  int i;
  int* heap_idcs[128];
  int* forty;
  int* sixty;
  int* onetwentynine;
  int* onetwentyeight;

  if (argc != 3) { 
    Printf("Usage: %s <handle_to_procs_completed_semaphore>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);
  test_num = dstrtol(argv[2], NULL, 10);

  // Now test based on number
  if(test_num == 2){
    Printf("Test %d: filling the heap to the brim\n\n", test_num);
    for(i=0;i<128;i++){
      heap_idcs[i] = malloc(32);
    }
    malloc(1);
    for(i=0;i<128;i++){
      mfree(heap_idcs[i]);
    }
  }
  // Now test based on number
  else if(test_num == 1){
    Printf("Test %d: freeing full heap\n\n", test_num);
    heap_idcs[0] = malloc(4096);
    mfree(heap_idcs[0]);
    heap_idcs[1] = malloc(2);
    
  }
  else if (test_num == 5){
    Printf("Test %d: making holes\n\n", test_num);
    heap_idcs[0] = malloc(32);
    heap_idcs[1] = malloc(64);
    heap_idcs[2] = malloc(64);
    heap_idcs[3] = malloc(128);
    mfree(heap_idcs[0]);
    mfree(heap_idcs[1]);
    mfree(heap_idcs[2]);
    mfree(heap_idcs[3]);
  }
  else if (test_num == 0){
    Printf("seven %d: allocating 40, 60, 129, and 128 bytes!\n", getpid());
    forty = malloc(40);
    Printf("40 addr: %d\n", (int)forty);
    sixty = malloc(60);
    Printf("60 addr: %d\n", (int)sixty);
    onetwentynine = malloc(129);
    Printf("129 addr: %d\n", (int)onetwentynine);
    onetwentyeight = malloc(128);
    Printf("128 addr: %d\n", (int)onetwentyeight);

    dstrcpy(onetwentynine, "Hello world!");
    Printf("Hello world printed from heap memory: %s\n", onetwentynine);
    dstrcpy(forty, "forty\n");
    dstrcpy(sixty, "sixty\n");
    Printf("String in 40: %s", forty);
    Printf("String in 60: %s", sixty);
    Printf("Freeing all the allocations\n");
    if(mfree(forty) < 0) {
        Printf("mfree failed!\n");
    }
    if(mfree(sixty) < 0) {
        Printf("mfree failed!\n");
     }
    if(mfree(onetwentynine) < 0) {
        Printf("mfree failed!\n");
     }
    if(mfree(onetwentyeight) < 0) {
        Printf("mfree failed!\n");
     }
  }
  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("hello_world (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }

  Printf("test_malloc (%d): Done!\n", getpid());
}
