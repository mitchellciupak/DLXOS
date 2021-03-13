#ifndef __USERPROG__
#define __USERPROG__

#include "usertraps.h"

typedef struct Molecule {
  int ct;
  mbox_t box;
} Molecule;

#ifndef NULL
#define NULL (void *)0x0
#endif


#endif