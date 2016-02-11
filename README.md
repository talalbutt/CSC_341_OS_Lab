# Operating System Lab

### by Aaron Baker, Andrew Ballard, and Dylan Jager-Kujawa

#### CSC 341, Spring 2016

##### Submitted under cs341109

## Table of contents
  - [Project overview](#project-overview)
    - [Machine](#machine)
    - [OS](#os)
    - [Difficulties Encountered](#difficulties-encountered)
  - [Verification](#verification)
  - [Project Listing](#project-listing)
    - [machine.h](#listing_machine_h)
    - [machine.c](#listing_machine_c)
    - [os.h](#listing_os_h)
    - [os.c](#listing_os_c)
    - [part1.dat](#listing_part1_dat)
    - [part1x.dat](#listing_part1x_dat)
    - [makefile](#listing_makefile)

## Project overview
During this course of this class we are going to be exploring and creating operating system concepts and components. In this very first lab we are going to implement a machine language interpreter (or Hardware Simulator) for the architecture defined in our handout. This will be our basis for the *Hardware System*.

### Machine
First we will go over the Architectural  design of the machine as indicated on the handout:
* 16-bit words
* 256-word memory (16 bits) word addressable
* 3 general purpose registers (1-3), 1 special: A (0)
* 8-bit program counter (PC)
* condition code flags : equals or zero-(010), positive-(001), negative-(100)
* Machine instruction cycle - fetch, decode, opfetch, execute, writeback

This was the basis of the design of our machine. The 16 bit words are held in the <b>short int</b> data type, which in c can only hold 16 bits. The 256 word memory is simply an array of size 256, the indexes obviously refer to the address location of instructions.<br></br>To hold the Registers we designed a structure that represented all possible machine registers states (including condition codes). Since we knew in advance that users were going to have different register states that would need to be saved and loaded, this just made sense.</br></br>The machine instructions and operation codes are as follows:

![alt text](https://i.imgur.com/eAVF44a.png)

The above outlines the basic operations the machine had to be able to perform. Using this outline and feedback from our instructor we were able to get all the functionalilty of the above working.

### OS
The Operating System design currently is very basic. The main functions of the OS are currently to monitor the usage of the machine by each user and switch the users when they use their alloted usage. It also currently has a simple interactive command-line interface. The users type in the commands that they want executed (the typing of a command counts as a single tick against the user). The following commands currently in place are:
* **run** - run the program in main memory (user)
* **dmp** - dump (output) of main memory (o/s only)
* **nop** - none (no command to execute) (user or o/s)
* **stp** - stop the program (o/s only)

When you run the program, the registers are dumped on every tick, for this reason the **dmp** function does not currently dump registers. This will likely change when given the scope of our next project.

#### Scheduling Users
One of the problems we ran into when we started designing the OS is how we were going to schedule the users. Given the scope of this part of the program we decided that a enumerator would work just fine since the users were to be hardcoded in. Each user has an associated register state that is read and written to and by the machine when the user begins and ends his use of the machine.<br></br>We wanted to note that we have already came up with a way of scheduling many more users in a round robin fashion with the user of a queue. But due to the scope of the project and only a rudimentary knowledge of how to schedule users properly, we decided that it would be best to keep free of pointer logic for now.

### Difficulties Encountered
The biggest difficulty that we ran into during this project was having getting a precise idea of what each op code was supposed to do, and how it would affect registers. Even after discussing it with our instructor and eachother we would still find topics that we were confused on (I.E. Should the jump command be able to use immediate addressing?).<br><br>
Additionally we actually used the version control software Github to help with development on this portion of the project. The intial setup of getting everyone set up with their own fork to work on took some time. But that was essentially just the only other difficulty we ran into.

## How to Run
This will be a quick overview of how to run the program:
* Navigate to the submission folder and type the command: **make**, this will compile al the files.

![alt text](https://i.imgur.com/abWpkLq.png)
* Next type: **./os** to run the compiled code.

![alt text](https://i.imgur.com/MFvETMD.png)
* Next use the commands as specified in the OS section of this report.

**NOTE:** Currently we are reading the program directly into memory form the "part1.dat" file that is in the directory. Both user 1 and user 2 are defaulted to this program. If you want to load a different program into memory, you can open the "part1.dat" file and paste your binary into that file, if you already have a file with a new program in it, you can open the os.c file and change the file name retrieved.
![alt text](https://i.imgur.com/FUwVD61.png)

## Time Spent & Charts
We actually have some of the most accurate timings of coding we have ever recorded. This is due to the fact that someone else (Specifically github) did it for us. The following graphs dipict how much code was added to the project and on what days.

First this is the total contribution to the project by day, from the day we started to today (the 11th).
![alt text](https://i.imgur.com/ylnJUZh.png)

The following is a breakdown of lines of code, and how many additional lines of code were added compared to how many were deleted by day.
![alt text](http://i.imgur.com/y9TogtV.png)

Next is the amount of changes and additions that happened to the project by day (as you can see, we finished most of the coding on tuesday).
![alt text](http://i.imgur.com/e1l4lIq.png)

Finally the time of day at which most of our changes were submitted.
![alt text](http://i.imgur.com/ZgwCZxE.png)

We unfortnately did not keep track of time spent by individuals, but we estimate a total of around 26 hours of work on this portion of the project.

## Verification
