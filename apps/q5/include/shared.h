#ifndef __USERPROG__
#define __USERPROG__

#define H2O_INJECTIONFILE "h2o.dlx.obj"
#define SO4_INJECTIONFILE "so4.dlx.obj"
#define H2x2_O2_REACTIONFILE "h2x2_o2.dlx.obj"
#define H2SO4_REACTIONFILE "h2so4.dlx.obj"
#define SO2_O2_REACTIONFILE "so2_o2.dlx.obj"

typedef struct molcomp {

  sem_t h2o_sem;
  sem_t so4_sem;
  sem_t h2_sem;
  sem_t o2_sem;
  sem_t so2_sem;
  sem_t h2so4_sem;

} molcomp;

#endif