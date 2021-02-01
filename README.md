# DLXOS-LAB1-ECE469
#### DLXOS Introduction at https://engineering.purdue.edu/~ee469/labs_2021/lab1.html
#### Due: 2021-02-02 23:59 EST

## WARNING
* before you exit from ssh session: Please make sure that you copy the contents of your tmp folder into your home for backup

        cp -r /tmp/$USER/ee469 ~/

* When you login again to ecegrid, you can restore your work to tmp as:

        cp -r ~/ee469 /tmp/$USER/"

## File Structure
    ./
        apps/: user programs go here
        bin/: put any executable DLX files here
        include/:
            header files for user programs are here
            os/: header files for the operating system are here
        lib/: precompiled object files for user programs (i.e. usertraps.o) go here
        os/


## DLX Operating System:
Since it is beyond the scope of this course to write an operating system completely from scratch, we are providing you with a set of basic skeleton source files that can be compiled into a DLX-based operating system. This operating system will be referred to as DLXOS. You will be editing these source files throughout this course as you implement new features. The initial set of source files are as follows:

### C Source Files:
* memory.c: contains functions dealing with paging and other memory managment tasks.
* process.c: contains functions dealing with process maintenance and switching.
* misc.c: contains miscellaneous helpful functions such as string manipulation functions.
* queue.c: contains functions related to queue management.
* synch.c: contains functions related to synchronization primitives such as semaphores.
* sysproc.c: contains functions used primarily for tesing the other operating system functions.
* traps.c: contains functions that respond to traps.
* filesys.c: contains functions for reading and writing to a filesystem.

### Assembly Source Files:
* dlxos.s: contains low-level operating system code such as the bootloader.
* osend.s: a bookkeeping file listing the last address of the operating system.
* trap_random.s: contains the trap subroutines to generate random numbers.
* usertraps.s: contains the trap subroutines that are available to user programs.

# Compiling

    gcc-dlx -mtraps -O3 -Wall -c -o foo.o foo.c