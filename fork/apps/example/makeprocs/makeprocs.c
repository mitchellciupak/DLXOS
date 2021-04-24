#include "misc.h"
#include "usertraps.h"

void main(int argc, char *argv[]) {
  int c;
  int b;
  int x = 1111;

  Printf("Makeprocs: Starting (%d)\n", getpid());
  Printf("Makeprocs: BEFORE FORK: x is %d Process (%d)\n", x, getpid());

  c = fork();

  Printf("Makeprocs: Return Value from _fork() is %d\n",c);

  if (c == 0) {
    Printf("Makeprocs: BEFORE CHILD: x is %d (%d)\n", x, getpid());
    x = 2222;
    Printf("Makeprocs: AFTER CHILD: x is %d (%d)\n", x, getpid());
    b = fork();
    if(b == 0){
      Printf("A grandchild.\n");
      x = 2;
    }
    else{
      Printf("I'm getting old\n");
    }
  } else {
    Printf("Makeprocs: BEFORE PARENT: x is %d (%d)\n", x, getpid());
    x = 3333;
    Printf("Makeprocs: AFTER PARENT: x is %d (%d)\n", x, getpid());
  }

  Printf("Makeprocs: AFTER: x is %d (%d)\n", x, getpid());
  Printf("Makeprocs: Forked Successfully!\n");
}
