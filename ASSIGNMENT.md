# Lab 2 - Procces Sync at https://engineering.purdue.edu/~ee469/labs_2021/lab2.html

## Questions

1. (10 Points) Read queue.c, queue.h, synch.h, synch.c and answer the following questions. Please write your answers in apps/q1/Step1.txt.
 (2 points) How many semaphores are available in DLXOS?
 (2 points) How many locks are available in DLXOS?
 (2 points) How is a semaphore created in DLXOS?
 (2 points) Explain how sem_signal and sem_wait work.
 (2 points) What is a handle and how is it used to access semaphores in DLXOS?

2. (10 points) Using the above shared memory APIs, implement the following producer/consumer communication through a circular buffer using only locks to prevent race conditions and deadlocks. Note that you should still use a sempahore to signal the parent process that all child processes have completed. The sample code "makeprocs" will serve as an example on how to use semaphore to signal the parent process. The solution to this problem can be expressed as follows:

The producer places characters from the string "Hello world" into a shared circular buffer one character at a time. If the buffer is full, the producer will wait until there is space available. The consumer pulls one character out of the shared circular buffer and prints it to the screen. If the buffer is empty, it must wait until there is something in the buffer to print.

To make the solution simpler, you can assume there are the same number of consumers and producers. In other words, if each producer will put strlen("Hello World") characters into the buffer, then each consumer can remove exactly strlen("Hello World") characters out of the buffer. If we did not have this restriction, then it is more difficult for the consumer to know when to exit. This way, it can simply choose to exit when it has read strlen("Hello World") characters from the buffer.

Your solution must follow the convention of the example code, where a "makeprocs" process creates the producers and consumers, and then waits until all producers and consumers have exited before it exits. makeprocs should have the following program invocation:
dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj <number of producers and consumers>
for example,
dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 3
will create 3 producers and 3 consumers.

Each time a producer puts a character into the buffer, it should print:

Producer X inserted: H

Each time a consumer takes a character out of the buffer, it should print:

Consumer Y removed: H

where X and Y are the process id's of the producer and consumer, respectively.

All your user code should be in apps/q2/. You should create the following subdirectories: apps/q2/include/, apps/q2/makeprocs, apps/q2/producer, and apps/q2/consumer. What goes in those directories should follow the convention of the apps/example directory.

3. (30 points) Implement condition variables in os/synch.c using the existing locks and queues. The user-level interface is already provided in os/usertraps.s. Look at the code for locks and semaphores to see how to implement the process waiting queue. As a hint, recognize that a condition variable is essentially a semaphore without an internal counter, so the code is almost the same. You will find markers in include/os/synch.h and os/synch.c where your code should go.

All your code for this problem (excluding any extra test code you may write) should be in os/synch.c and include/os/synch.h.

4. (10 points) Implement the same producer/consumer problem using locks and condition variables. All code for this problem should go in the apps/q4 directory with the same structure as the previous producer/consumer solutions.

5. (40 points) You are given a hypothetical situation below. In the atmosphere on the planet Radeon, there are 3 primary chemical reactions that can occur:

1) 2 H2O -> 2 H2 + O2 2) SO4 -> SO2 + O2 3) H2 + O2 + SO2 -> H2SO4

The Radeon Atmospheric Sciences (RAS) department has asked us to help out with writing an atmosphere monitoring program. They require a program that
a) Injects molecules of H2O (water) and SO4 (sulfate) into the atmosphere
b) Monitors the formation of the O2 (oxygen) and H2SO4 (sulfuric acid) molecules
c) Prints out messages at every stage

Reaction 1) should happen whenever there are two H2O molecules available
Reaction 2) should happen whenever there is one SO4 molecule available
Reaction 3) should happen whenever there is one H2 molecule, one O2 molecule, SO2 molecule available.

In order to solve this problem, you may think of each reaction as a separate process, and each molecule as a separate semaphore. Also, the "injection" routines which create the original H2O and SO4 molecules are each a separate process, for a total of 5 processes. When a given atom or molecule is created, you can signal the semaphore for that atom or molecule, and when you want to consume an atom or molecule, you should wait on the associated semaphore.

When a H2O or SO4 molecule is injected, you should print a message indicating such. You should also print a message when the final O2 molecule and H2SO4 molecule are created.

Your program should have a makeprocs process that can be invoked like this:

$ dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj <number of H2O molecules> <number of SO4 molecules>

As in all other problems, makeprocs should not exit until all of the 5 other processes have finished. In order to support this, you must pass the expected number of "consumable" molecules to each of the "consumer" processes: i.e. the 3 processes representing the reactions. You will therefore have to compute this based on the reaction formulas for each reaction and the original number of H2O molecules and SO4 molecules.

You should only print the minimum number of messages necessary to describe the underlying reactions. You must print one message for each molecule created.
Sample output:
If the input is 3 H2O and 3 SO4 molecule the output can be as given below.
Creating 3 H2Os and 3 SO4s.
H2O injected into Radeon atmosphere, PID: 30
H2O injected into Radeon atmosphere, PID: 30
SO4 injected into Radeon atmosphere, PID: 29
SO4 injected into Radeon atmosphere, PID: 29
SO4 injected into Radeon atmosphere, PID: 29
2 H2O -> 2 H2 + O2 reacted, PID: 28
H2O injected into Radeon atmosphere, PID: 30
SO4 -> SO2 + O2 reacted, PID: 27
(1) H2 + O2 + SO2 -> H2SO4 reacted, PID: 26
SO4 -> SO2 + O2 reacted, PID: 27
(2) H2 + O2 + SO2 -> H2SO4 reacted, PID: 26
SO4 -> SO2 + O2 reacted, PID: 27
1 H2O's left over. 0 H2's left over. 2 O2's left over. 1 SO2's left over. 2 H2SO4's created.
End of sample output.