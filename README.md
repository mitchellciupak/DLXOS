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

<p align="center"> Lab 4 by Kempak Kernels (Team No. 24)
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
unzip ee469lab4_g24.zip

```

## 🔧 Running the tests <a name = "tests"></a>
This repository actually contains three OS's. The first OS deals with questions 1 and 2 of lab 4. To run these tests, change directories (cd) into one-level and execute the following commands:

### Running One-level
```
chmod u+x make.sh
make.sh -R q2
```
This will run the tests for question 2 on a one-level system.

### Running Fork
To run the test for fork, cd into fork and execute the following commands:
```
chmod u+x make.sh
make.sh -R q2
```

### Running Heap
To run the test for heap, cd into heap and execute the following commands:
```
chmod u+x make.sh
make.sh -R tests
```
To view the heap after each allocation and free, enter debug mode by executing:
```
make.sh -D tests
```
The flag "-D h" will run the fancyPrint function and display heaps.

### Additional Information

Note that in any OS, you can always cd into the os folder and run
```
make
make run
```
and then cd into the folder of the application you wish to run and run
```
make
make run
```

To clean, run "make clean" from any project folder, or simply run "make.sh -c" from the heap, fork, or one-level folders.

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
