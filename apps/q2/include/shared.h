#ifndef __USERPROG__
#define __USERPROG__

#define BUFF_LEN 11


typedef struct buffer_char {
  int numprocs;
  lock_t lock;
  sem_t full;
  sem_t empty;
  char buff[BUFF_LEN];
} buffer_char;

#endif