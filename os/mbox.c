#include "ostraps.h"
#include "dlxos.h"
#include "process.h"
#include "synch.h"
#include "queue.h"
#include "mbox.h"

static Mbox mboxes[MBOX_NUM_MBOXES];
static MboxMessage messages[MBOX_NUM_BUFFERS];
//-------------------------------------------------------
//
// void MboxModuleInit();
//
// Initialize all mailboxes.  This process does not need
// to worry about synchronization as it is called at boot
// time.  Only initialize necessary items here: you can
// initialize others in MboxCreate.  In other words, 
// don't waste system resources like locks and semaphores
// on unused mailboxes.
//
//-------------------------------------------------------

void MboxModuleInit() {
  int i; // Loop Index variable
  dbprintf ('p', "Mbox Module Init: Entering MboxModuleInit\n");
  for(i=0; i<MBOX_NUM_MBOXES; i++) {
    mboxes[i].inuse = 0;
  }
  for(i=0; i<MBOX_NUM_BUFFERS; i++) {
    messages[i].inuse = 0;
  }
  
  dbprintf ('p', "MboxModuleInit: Leaving MboxModuleInit\n");
}

//-------------------------------------------------------
//
// mbox_t MboxCreate();
//
// Allocate an available mailbox structure for use. 
//    Will initialize process bit array to zero
//    Will reserve a lock for the process
// Returns the mailbox handle on success
// Returns MBOX_FAIL on error.
//
//-------------------------------------------------------
mbox_t MboxCreate() {
  int i;
  mbox_t mbox;
  uint32 intrval;

  intrval = DisableIntrs();
  for(i = 0; i < MBOX_NUM_MBOXES; i++){
    if(mboxes[i].inuse == 0){
      mboxes[i].inuse = 1;
      printf("MBOX #%d is in use\n", i);
      mbox = i;
      break;
    }
  }
  RestoreIntrs(intrval);
  if(mbox==MBOX_NUM_MBOXES) return MBOX_FAIL;

  mboxes[mbox].lock = LockCreate();
  if(mboxes[mbox].lock == SYNC_FAIL){
    printf("FATAL ERROR: could not lock in MboxInit!\n");
    exitsim();
  }

  for(i=0;i<PROCESS_MAX_PROCS;i++){
    mboxes[mbox].track_procs[i] = 0;
  }
  
  if (AQueueInit (&mboxes[mbox].messages) != QUEUE_SUCCESS) {
    printf("FATAL ERROR: could not initialize queue in MboxINIT!\n");
    exitsim();
  }
  return mbox;
}

//-------------------------------------------------------
// 
// void MboxOpen(mbox_t);
//
// Open the mailbox for use by the current process.  Note
// that it is assumed that the internal lock/mutex handle 
// of the mailbox and the inuse flag will not be changed 
// during execution.  This allows us to get the a valid 
// lock handle without a need for synchronization.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxOpen(mbox_t handle) {
  if(handle > MBOX_NUM_MBOXES || handle < 0) return MBOX_FAIL;
  if(mboxes[handle].inuse == 0) return MBOX_FAIL;

  if(LockHandleAcquire(mboxes[handle].lock) == SYNC_FAIL) return MBOX_FAIL;
  mboxes[handle].track_procs[GetCurrentPid()] = 1;

  return MBOX_SUCCESS;
}

//-------------------------------------------------------
//
// int MboxClose(mbox_t);
//
// Close the mailbox for use to the current process.
// If the number of processes using the given mailbox
// is zero, then disable the mailbox structure and
// return it to the set of available mboxes.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxClose(mbox_t handle) {
  return MBOX_FAIL;
}

//-------------------------------------------------------
//
// int MboxSend(mbox_t handle,int length, void* message);
//
// Send a message (pointed to by "message") of length
// "length" bytes to the specified mailbox.  Messages of
// length 0 are allowed.  The call 
// blocks when there is not enough space in the mailbox.
// Messages cannot be longer than MBOX_MAX_MESSAGE_LENGTH.
// Note that the calling process must have opened the 
// mailbox via MboxOpen.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxSend(mbox_t handle, int length, void* message) {
  return MBOX_FAIL;
}

//-------------------------------------------------------
//
// int MboxRecv(mbox_t handle, int maxlength, void* message);
//
// Receive a message from the specified mailbox.  The call 
// blocks when there is no message in the buffer.  Maxlength
// should indicate the maximum number of bytes that can be
// copied from the buffer into the address of "message".  
// An error occurs if the message is larger than maxlength.
// Note that the calling process must have opened the mailbox 
// via MboxOpen.
//   
// Returns MBOX_FAIL on failure.
// Returns number of bytes written into message on success.
//
//-------------------------------------------------------
int MboxRecv(mbox_t handle, int maxlength, void* message) {
  return MBOX_FAIL;
}

//--------------------------------------------------------------------------------
// 
// int MboxCloseAllByPid(int pid);
//
// Scans through all mailboxes and removes this pid from their "open procs" list.
// If this was the only open process, then it makes the mailbox available.  Call
// this function in ProcessFreeResources in process.c.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//--------------------------------------------------------------------------------
int MboxCloseAllByPid(int pid) {
  return MBOX_FAIL;
}
