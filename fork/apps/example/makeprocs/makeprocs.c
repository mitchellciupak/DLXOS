#include "misc.h"
#include "usertraps.h"

void main(int argc, char *argv[]) {
  int c;
  int x = 1111;

  Printf("Makeprocs: Starting (%d)\n", getpid());
  Printf("Makeprocs: BEFORE FORK: x is %d (%d)\n", x, getpid());

  c = fork();

  Printf("Makeprocs: Return Value from _fork() is %d\n",c);

  if (c == 0) {
    Printf("Makeprocs: BEFORE CHILD: x is %d (%d)\n", x, getpid());
    x = 2222;
    Printf("Makeprocs: AFTER CHILD: x is %d (%d)\n", x, getpid());
  } else {
    Printf("Makeprocs: BEFORE PARENT: x is %d (%d)\n", x, getpid());
    x = 3333;
    Printf("Makeprocs: AFTER PARENT: x is %d (%d)\n", x, getpid());
  }

  Printf("Makeprocs: AFTER: x is %d (%d)\n", x, getpid());
  Printf("Makeprocs: Forked Successfully!\n");
}


//TODO - add another test for ROP