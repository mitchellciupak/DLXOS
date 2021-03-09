#ifndef __USERPROG__
#define __USERPROG__

#define BUFF_LEN 11


typedef struct buffer_char {

  int numprocs;
  int head;
  int tail;

  lock_t lock; // does the actuall locking triggered by sem logic
  sem_t full;  // 0 <-> 11
  sem_t empty; // 11 <-> 0, tiggers producer, consumer waits on

  char buff[BUFF_LEN];

} buffer_char;

#endif