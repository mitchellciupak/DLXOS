#ifndef __USERPROG__
#define __USERPROG__

#define H20_INJECTIONFILE "h20.c"
#define SO4_INJECTIONFILE "so4.c"
#define H2x2_02_REACTIONFILE "h2x2_o2.c"
#define H2SO4_REACTIONFILE "h2so4.c"
#define SO2_O2_REACTIONFILE "s02_o2.c"

typedef struct molcomp {

  sem_t h2o_sem;
  sem_t s04_sem;
  sem_t h2_sem;
  sem_t o2_sem;
  sem_t s02_sem;
  sem_t h2so4_sem;

} molcomp;

#endif