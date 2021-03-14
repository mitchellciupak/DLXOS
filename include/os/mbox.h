#ifndef __MBOX_OS__
#define __MBOX_OS__

#define MBOX_NUM_MBOXES 16           // Maximum number of mailboxes allowed in the system
#define MBOX_NUM_BUFFERS 50          // Maximum number of message buffers allowed in the system
#define MBOX_MAX_BUFFERS_PER_MBOX 10 // Maximum number of buffer slots available to any given mailbox
#define MBOX_MAX_MESSAGE_LENGTH 100   // Buffer size of 100 for each message

#define MBOX_FAIL -1
#define MBOX_SUCCESS 1

//---------------------------------------------
// Define your mailbox structures here
//--------------------------------------------

typedef struct MboxMessage {
    char buffer[MBOX_MAX_MESSAGE_LENGTH];
    unsigned int length;
    char inuse;
    Link* l;
} MboxMessage;

typedef struct Mbox {
    char track_procs[PROCESS_MAX_PROCS]; // initialize all to zero
    Link* links[MBOX_MAX_BUFFERS_PER_MBOX];
    char inuse;
    lock_t lock;
    Queue q;
    sem_t full;
    sem_t empty;
} Mbox;

typedef int mbox_t; // This is the "type" of mailbox handles
typedef int mes_t;

//-------------------------------------------
// Prototypes for Mbox functions you have to write
//-------------------------------------------

void MboxModuleInit();
mbox_t MboxCreate();
int MboxOpen(mbox_t m);
int MboxClose(mbox_t m);
int MboxSend(mbox_t m, int length, void *message);
mes_t MessageInit(int length, void* message);
void InsertMessageLink(mes_t mes, mbox_t handle);
MboxMessage*  RemoveMessageLink( mbox_t handle);
int MboxCloseByPID(mbox_t handle, int pid);
int MboxRecv(mbox_t m, int maxlength, void *message);
int MboxCloseAllByPid(int pid);

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

#endif
