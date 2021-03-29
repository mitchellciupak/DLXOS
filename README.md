<p align="center">
  <a href="" rel="noopener">
 <img width=200px height=200px src="https://cdn2.vectorstock.com/i/thumb-large/28/96/corn-kernel-vector-7732896.jpg" alt="Project logo"></a>
</p>

<h3 align="center">DLXOS</h3>

<div align="center">

  [![Status](https://img.shields.io/badge/status-active-success.svg)]()
  [![GitHub Pull Requests](https://img.shields.io/github/issues-pr/kylelobo/The-Documentation-Compendium.svg)](https://github.com/mitchellciupak/DLXOS/pulls)

</div>

---

<p align="center"> Lab 3 by Kempak Kernels (Team No. 24)
    <br>
</p>

## 📝 Table of Contents
- [About](#about)
- [Getting Started](#getting_started)
- [Deployment](#deployment)
- [Usage](#usage)
- [Authors](#authors)
- [Acknowledgments](#acknowledgement)
- [Appendix](#appendix)

## 🧐 About <a name = "about"></a>

For answers to the questions listed in the assignment description please visit the [Assignement](./ASSIGNMENT.md) file.

Information about how are code works goes here

## 🏁 Getting Started <a name = "getting_started"></a>
These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See [deployment](#deployment) for notes on how to deploy the project on a live system.

### Prerequisites
* Linux
* gcc-dlx
* dlxsim
* dlxasm

### Installing
Simply, unzip and run given you have the required programs

```
unzip ee469lab3_g24.tar.gz
gcc-dlx -mtraps -O3 -Wall -c -o foo.o foo.c #TODO-is this correct Compiling Step
#TODO-is this correct run (dlxsim) Step
```

## 🔧 Running the tests <a name = "tests"></a>
Explain how to run the automated tests for this system.

```
chmod u+x
make.sh -R q2
```
Run each process (q2, q3, and q5) with the commands above from the DLXOS folder. Alternatively, you could cd into the project's respective directory under the DLXOS/apps folder and run

```
make
make run
```

To clean, run "make clean" from any project folder, or simply run "make.sh -c" from the DLXOS folder.

## 🎈 Usage <a name="usage"></a>
Add notes about how to use the system.

### WARNING
* before you exit from ssh session: Please make sure that you copy the contents of your tmp folder into your home for backup

        cp -r /tmp/$USER/ee469 ~/

* When you login again to ecegrid, you can restore your work to tmp as:

        cp -r ~/ee469 /tmp/$USER/"

## 🚀 Deployment <a name = "deployment"></a>
Add additional notes about how to deploy this on a live system.

## ✍️ Authors <a name = "authors"></a>
- [@mitchellciupak](https://github.com/mitchellciupak) - Teammate
- [@ckemmet](https://github.com/ckemmet) - Teammate

## 🎉 Acknowledgements <a name = "acknowledgement"></a>
- Hat tip to anyone whose code was used
- Inspiration
- References

## 🔎 Appendix <a name = "appendix"></a>

### File Structure
    ./
        apps/: user programs go here
        bin/: put any executable DLX files here
        include/:
            header files for user programs are here
            os/: header files for the operating system are here
        lib/: precompiled object files for user programs (i.e. usertraps.o) go here
        os/

#### C Source Files:
* memory.c: contains functions dealing with paging and other memory managment tasks.
* process.c: contains functions dealing with process maintenance and switching.
* misc.c: contains miscellaneous helpful functions such as string manipulation functions.
* queue.c: contains functions related to queue management.
* synch.c: contains functions related to synchronization primitives such as semaphores.
* sysproc.c: contains functions used primarily for tesing the other operating system functions.
* traps.c: contains functions that respond to traps.
* filesys.c: contains functions for reading and writing to a filesystem.

#### Assembly Source Files:
* dlxos.s: contains low-level operating system code such as the bootloader.
* osend.s: a bookkeeping file listing the last address of the operating system.
* trap_random.s: contains the trap subroutines to generate random numbers.
* usertraps.s: contains the trap subroutines that are available to user programs.
