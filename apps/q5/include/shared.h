#ifndef __USERPROG__
#define __USERPROG__

typedef struct molcomp {

  sem_t h2o_sem;
  sem_t so4_sem;
  sem_t h2_sem;
  sem_t o2_sem;
  sem_t so2_sem;
  sem_t h2so4_sem;

} molcomp;

#endif