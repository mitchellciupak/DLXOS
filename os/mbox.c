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
      mbox = i;
      break;
    }
  }
  RestoreIntrs(intrval);
  if(mbox==MBOX_NUM_MBOXES) return MBOX_FAIL;

  mboxes[mbox].lock = LockCreate();
  mboxes[mbox].empty = SemCreate(MBOX_NUM_BUFFERS);
  mboxes[mbox].full = SemCreate(0); // dont need this! also, make a semaphore
  //CondHandleSignal(mboxes[mbox].empty);
  if(mboxes[mbox].lock == SYNC_FAIL){
    printf("FATAL ERROR: could not lock in MboxInit!\n");
    exitsim();
  }

  for(i=0;i<PROCESS_MAX_PROCS;i++){
    mboxes[mbox].track_procs[i] = 0;
  }
  
  if (AQueueInit (&mboxes[mbox].q) != QUEUE_SUCCESS) {
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
  if(LockHandleRelease(mboxes[handle].lock) == SYNC_FAIL) return MBOX_FAIL;
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
  int i;

  if(handle > MBOX_NUM_MBOXES || handle < 0) return MBOX_FAIL;
  if(mboxes[handle].inuse== 0 || mboxes[handle].track_procs[GetCurrentPid()] == 0) return MBOX_FAIL;

  if(LockHandleAcquire(mboxes[handle].lock) == SYNC_FAIL) {return MBOX_FAIL;}
  mboxes[handle].track_procs[GetCurrentPid()] = 0;

  for(i=0;i<PROCESS_MAX_PROCS;i++){
    if(mboxes[handle].track_procs[i]){
      break;
    }
  }
  if(i == PROCESS_MAX_PROCS){
    AQueueInit(&mboxes[handle].q); // Reset queue
    mboxes[handle].inuse = 0;
  }

  if(LockHandleRelease(mboxes[handle].lock) == SYNC_FAIL) return MBOX_FAIL;
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
  mes_t mes;
  
  if(length > MBOX_MAX_MESSAGE_LENGTH) return MBOX_FAIL;
  if(handle > MBOX_NUM_MBOXES || handle < 0) return MBOX_FAIL;
  if(mboxes[handle].inuse== 0 || mboxes[handle].track_procs[GetCurrentPid()] == 0) return MBOX_FAIL;

  SemHandleWait(mboxes[handle].empty);
  if(LockHandleAcquire(mboxes[handle].lock) == SYNC_FAIL) {return MBOX_FAIL;}

  // Copy the message to a message buffer
  if((mes = MessageInit(length, message)) == MBOX_FAIL){
    printf("FATAL ERROR: could not allocate message!\n");
    exitsim();
  }

  // Insert the message into the mailbox buffer queue
  InsertMessageLink(mes, handle);

  // Signal full
  if(LockHandleRelease(mboxes[handle].lock) == SYNC_FAIL) return MBOX_FAIL;
  SemHandleSignal(mboxes[handle].full);
  return MBOX_SUCCESS;
}

void InsertMessageLink(mes_t mes, mbox_t handle){
  Link* l;

  if ((l = AQueueAllocLink ((void *)&(messages[mes]))) == NULL) {
    printf("FATAL ERROR: could not allocate link for message queue in MboxSend!\n");
    exitsim();
  }
  if (AQueueInsertFirst(&mboxes[handle].q, l) != QUEUE_SUCCESS) {
    printf("FATAL ERROR: could not insert new link into message queue in MboxSend!\n");
    exitsim();
  }

  messages[mes].l = l;
}

mes_t MessageInit(int length, void* message){
  int i;
  mes_t mes;
  char * src;

  for(i=0;i<length;i++){
    if(messages[i].inuse == 0){
      messages[i].inuse = 1;
      mes = i;
      break;
    }
  }
  if(i == length) return MBOX_FAIL;

  src = message;
  for(i=0;i<length;i++){
    messages[mes].buffer[i] = src[i];
  }

  messages[mes].length = length;
  return mes;
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
  MboxMessage * mmes;
  char * dest;
  int i;
  int length;

  if(maxlength > MBOX_MAX_MESSAGE_LENGTH) return MBOX_FAIL;
  if(handle > MBOX_NUM_MBOXES || handle < 0) return MBOX_FAIL;
  if(mboxes[handle].inuse== 0 || mboxes[handle].track_procs[GetCurrentPid()] == 0) return MBOX_FAIL;

  SemHandleWait(mboxes[handle].full);
  if(LockHandleAcquire(mboxes[handle].lock) == SYNC_FAIL) return MBOX_FAIL;

  // Get link from q
  mmes = RemoveMessageLink(handle);
  length = mmes->length;

  dest = (char*)message;
  for(i=0;i<mmes->length;i++){
    dest[i] = mmes->buffer[i];
  }
  mmes->inuse = 0;
  if(AQueueLength(&mboxes[handle].q) == 9){
    CondHandleBroadcast(mboxes[handle].full);
  }

  if(LockHandleRelease(mboxes[handle].lock) == SYNC_FAIL) return MBOX_FAIL;
  SemHandleSignal(mboxes[handle].empty);
  return length;
}

MboxMessage* RemoveMessageLink(mbox_t handle){
  MboxMessage *mmes;

  mmes = (MboxMessage *)AQueueObject(AQueueFirst(&mboxes[handle].q));
  if (AQueueRemove(&(mmes->l)) != QUEUE_SUCCESS) {
    printf("FATAL ERROR: could not remove message from message in ProcessSchedule!\n");
    exitsim();
  }
  return mmes;
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
