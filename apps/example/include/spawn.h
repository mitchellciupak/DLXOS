#ifndef __USERPROG__
#define __USERPROG__

typedef struct missile_code {
  int numprocs;
  char really_important_char;
  char abc[3];
} missile_code;

#define FILENAME_TO_RUN "spawn_me.dlx.obj"

#ifndef NULL
#define NULL (void *)0x0
#endif

#endif
