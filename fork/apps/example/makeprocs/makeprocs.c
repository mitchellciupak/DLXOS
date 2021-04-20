#include "misc.h"
#include "usertraps.h"

void main(int argc, char *argv[]) {
  int c;
  int x = 1111;

  Printf("Makeprocs: Starting (%d)\n", getpid());
  Printf("Makeprocs: BEFORE FORK: x is $d (%d)\n", x, getpid());

  c = fork();

  if (c == 0) {
    Printf("Makeprocs: BEFORE CHILD FORK: x is $d (%d)\n", x, getpid());
    x = 2222;
    Printf("Makeprocs: AFTER CHILD FORK: x is $d (%d)\n", x, getpid());
  } else {
    Printf("Makeprocs: BEFORE PARENT FORK: x is $d (%d)\n", x, getpid());
    x = 3333;
    Printf("Makeprocs: AFTER PARENT FORK: x is $d (%d)\n", x, getpid());
  }

  Printf("Makeprocs: AFTER FORK: x is $d (%d)\n", x, getpid());
  Printf("Makeprocs: Forked Successfully!\n");
}
