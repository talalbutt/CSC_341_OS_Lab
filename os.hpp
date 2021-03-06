/*
 * os.hpp
 *
 * Baker, Ballard, Jager-Kujawa
 * CSC 341
 * Spring 2016
 */

#include <string>   // string
#include <queue>    // queue
#include <iostream> // cout, cerr
#include <iomanip>  // boolalpha, setw, setfill, hex, dec, endl
#include <sstream>  // sstream
#include "machine.hpp"
#include "fsys.hpp"
#include "utils.hpp"
  using namespace std;

#ifndef OS
#define OS

extern string systemStats;

extern FSYS fileSystem;

extern FrameTable framesInUse;
//extern FrameTable framesLocked;
#define LOCKED true
#define UNLOCKED false

// Define the user struct
enum userID {sys, u1, u2, noID};

struct User {
  userID id;
  int time;

  User() : id(noID), time(0) {}
  User(userID uid) : id(uid), time(0) {}
};
struct Process {
  int pid;
  string pname;
  User *user;
  bool running;
  registers regs;

  Process(string name, int id, User *uid) : pid(id), pname(name), user(uid),
    running(false), regs(registers(0, 0, 0, 0, 61440, 0, 0)) {}
  string toString() {
    stringstream ss;
    ss << (user->id == 0 ? "SYS" : ("U" + itos(user->id))) << " \"" << pname
       << "\" process with PID : " << pid << endl;
    ss << "\trA: " << regs.rA << ", r1: " << regs.r1 \
         << ", r2: " << regs.r2 << ", r3: " << regs.r3 << endl;
    ss << "\tIR: " << regs.IR << ", PC: " << regs.PC \
         << ", CR: " << regs.CR << endl;
    ss << "\tRunning: " << boolalpha << running << endl << endl;

    if (regs.PTBR) {
      ss << endl << "Page Table:" << endl;
      ss << regs.PTBR->toString() << endl << endl;
    }

    return ss.str();
  }
};

extern User U1, U2, SYS;

extern Process* currentProcess;

#define READY 1
#define BLOCKED 0

// Dump contents of main memory and all registers
void dump();

// Dump everything (to be called only on STP)
void fulldump();

// Print the contents of a queue, with a header containing the queue name
//void printQueue(string queueName, queue<User> &q,int num);

// Print the contents of a queue
//void printQueue(queue<User> &q,int num);

// Print all process control blocks
void printAllProcs();

// Loads the file into a new PCB and returns a pointer to the new Process.
// Takes the user by reference, as well as the cmdline entry the user entered.
// cmdline entry is split into cmd and args on the first encountered whitespace.
Process* loader(User& currentUser, string pname);

// Round-robin scheduler, 3 ticks per user
void scheduler();

// The user interface, prompting for input
void userinterface();

// Convert a string command (run, dmp, etc) into an int that can be used
// in a switch case statement.
int cmdToInt(string);

// Initialize all values required by the OS
void init();

// Main function (starts the OS)
int main();

// Convert process queue to string
string qtos(queue<Process*>);

#endif
