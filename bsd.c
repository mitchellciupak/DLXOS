
#include "include/process.h"

static Queue runQueues[NUM_RUN_QUEUES];
static Queue	waitQueue;

int quanta = 0;

#define load 1


void ProcessRecalcPriority(PCB *pcb){
    if (pcb->window_jiffies >= 10) pcb->estcpu++;	
    pcb->priority = BASE_PRIORITY + pcb->estcpu/4 + 2*pcb->pnice;
}

// Figure out which queue a given pcb is in
inline int WhichQueue(PCB *pcb){
  return (int)(pcb->priority / PRIORITIES_PER_QUEUE);
}

// supposed to insert the given PCB at the end of the proper run queue based on its current priority.
// It returns T or F depending on if the function successfully runs or no
int ProcessInsertRunning(PCB *pcb){
  queue_number = (int) (pcb->priority / PRIORITIES_PER_QUEUE);
  if ((pcb->l = AQueueAllocLink(pcb)) == NULL) {
    printf("FATAL ERROR: could not get link for ProcessInsertRunning!\n");
    exitsim();
  }
  if (AQueueInsertLast(&(runQueues[queue_number]), pcb->l) != QUEUE_SUCCESS) {
    printf("FATAL ERROR: could not insert link into runQueue in ProcessInsertRunning!\n");
    exitsim();
  }
  return PROCESS_SUCCESS;
}

// Increment estcpu value so piriorty goes down
// DONE
void ProcessDecayEstcpu(PCB *pcb){
  int i;
  Link *l;
  PCB* pcb;

  for(i=0;i<NUM_RUN_QUEUES;i++){
    l = AQueueFirst(&runQueues[i]);
    while (l != NULL) {	
        pcb = AQueueObject(l);
        pcb->estcpu = [pcb->estcpu * (2*load)/(2*load + 1) ] + pcb->pnice;
        pcb->priority = BASE_PRIORITY + pcb->estcpu/4 + 2*pcb->pnice;
        l = AQueueNext(l);
    }	
  }
}

// When a process wakes up, if it has been sleeping for a while, catch up
// the decay. Decay = estcpu goes up -> then priority goes down (gets higher)
// DONE
// External (wakeup)
void ProcessDecayEstcpuSleep(PCB *pcb, int time_asleep_jiffies){
  int num_windows_asleep;
  if (time_asleep_jiffies >= 100) {
    num_windows_asleep = time_asleep_jiffies / (TIME_PER_CPU_WINDOW * CPU_WINDOWS_BETWEEN_DECAYS);
    pcb->estcpu = pcb->estcpu * [ (2*load)/(2*load+1) ] ^ (num_windows_asleep);
  }
}

// To decide which process to run, you start at the lowest-numbered (highest-priority) queue (queue 0), and look for any processes in there. If you don't find any, move up one queue. Continue this until you find the queue with the highest priority that contains processes.
// Once you find the PCB at the highest priority queue with processess, you can start it
// DONE
PCB *ProcessFindHighestPriorityPCB(){
  int i;
  Link *l;
  PCB* pcb;

  for(i=0;i<NUM_RUN_QUEUES;i++){
    l = AQueueFirst(&runQueues[i]);
    if (l != NULL) {	
        pcb = AQueueObject(l);
        return pcb;
    }	
  }
}

// Do if 10 process quanta have passed
// DONE
void ProcessDecayAllEstcpus(){
  int i;
  Link *l;
  PCB* pcb;

  for(i=0;i<NUM_RUN_QUEUES;i++){
    l = AQueueFirst(&runQueues[i]);
    while (l != NULL) {	
        pcb = AQueueObject(l);
        pcb->estcpu = [pcb->estcpu * (2*load)/(2*load + 1) ] + pcb->pnice;
        pcb->priority = BASE_PRIORITY + pcb->estcpu/4 + 2*pcb->pnice;
        l = AQueueNext(l);
    }	
  }
}


// Looks through all of the queues and moves any process whose priority indicates
// that it is in the wrong queue
// DONE
void ProcessFixRunQueues(){
  int i;
  Link *l;
  PCB* pcb;
  int queue_number;

  for(i=0;i<NUM_RUN_QUEUES;i++){
    l = AQueueFirst(&runQueues[i]);
    while (l != NULL) {	
        pcb = AQueueObject(l);
        queue_number = (int) (pcb->priority / PRIORITIES_PER_QUEUE);
        // Take it out of this queue and put it where it belongs...
        if(i != queue_number){
          if (AQueueRemove(&(pcb->l)) != QUEUE_SUCCESS) {
            printf("FATAL ERROR: could not remove in ProcessFixRunQueues!\n");
            exitsim();
          } 
          if ((pcb->l = AQueueAllocLink(pcb)) == NULL) {
            printf("FATAL ERROR: could not get link for ProcessFixRunQueues!\n");
            exitsim();
          }
          if (AQueueInsertLast(&runQueue, pcb->l) != QUEUE_SUCCESS) {
            printf("FATAL ERROR: could not insert link into runQueue in ProcessFixRunQueues!\n");
            exitsim();
          }
        }
        l = AQueueNext(l);
    }	
  }
}


// Counts and returns the number of autoawoke processes on the waitQueue
int ProcessCountAutowake();

// Print every process id in every run q. Debugging
// DONE
void ProcessPrintRunQueues(){
  int i;
  Link *l;
  PCB* pcb;

  for(i=0;i<NUM_RUN_QUEUES;i++){
    l = AQueueFirst(&runQueues[i]);
    while (l != NULL) {	
        pcb = AQueueObject(l);
        printf("%s", pcb->name);
        l = AQueueNext(l);
    }	
  }
}

// Job is to rearrange priority queues and
// reset current PCB

// Don't have to deal with the wait queue
void ProcessSchedule (){
  int queue_number;
  
  quanta++;
  dbprintf ('p', "Now entering ProcessSchedule (cur=0x%x)\n",(int)currentPCB);
    // The OS exits if there's no runnable process.  This is a feature, not a
  // bug.  An easy solution to allowing no runnable "user" processes is to
  // have an "idle" process that's simply an infinite loop.
  if (AQueueEmpty(&runQueue)) {
    if (!AQueueEmpty(&waitQueue)) {
      printf("FATAL ERROR: no runnable processes, but there are sleeping processes waiting!\n");
      l = AQueueFirst(&waitQueue);
      while (l != NULL) {
        pcb = AQueueObject(l);
        printf("Sleeping process %d: ", i++); printf("PID = %d\n", (int)(pcb - pcbs));
        l = AQueueNext(l);
      }
      exitsim();
    }
    printf ("No runnable processes - exiting!\n");
    exitsim ();	// NEVER RETURNS
  }

  // Move the process to the back of its own queue
  queue_number = WhichQueue(currentPCB);
  AQueueMoveAfter(&(runQueues[i]), AQueueLast(&(runQueues[i])), AQueueFirst(&(runQueues[i])));

  // Recalculate process priority
  ProcessRecalcPriority(currentPCB);
  if (quanta > 10){
    ProcessDecayAllEstcpus();
    quanta=0;
  }

  ProcessFixRunQueues();
  currentPCB = ProcessFindHighestPriorityPCB();

}
