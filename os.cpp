/*
 * os.cpp
 *
 * Baker, Ballard, Jager-Kujawa
 * CSC 341
 * Spring 2016
 */

#include "os.hpp"

queue<User> readyQueue = queue<User>();
queue<User> blockedQueue = queue<User>();
User currentUser;
bool semaphore = unlocked;

int sysclock;
int switchTime;

// Dump contents of main memory and all registers and semaphore status
void dump()
{
	// Dump Registers
	User temp;
	cout << endl << ">>>>>>DUMPING REGISTERS<<<<<<" << endl;
	// Dump semaphore
	cout << endl << "Semaphore status: " << \
		(semaphore == locked ? "Locked" : "Unlocked") << endl;
	printQueue("readyQueue", readyQueue, readyQueue.size());
	printQueue("blockedQueue", blockedQueue, blockedQueue.size());

	// Dump memory
	cout << endl << ">>>>>>DUMPING MEMORY<<<<<<" << endl;
	// Print table header
	cout << "_____________________________________" \
		<< "__________________________" << endl;
	cout << "ad: valu | ad: valu | ad: valu | ad: " \
		<< "valu | ad: valu | ad: valu" << endl;
	cout << "_____________________________________" \
		<< "__________________________" << endl;
	// Print table contents
	for (int i = 0; i < 256; i++) {
		if (i % 6 == 0)
		cout << endl;
		else
		cout << " | ";
		cout << hex;
		cout << setw(2) << i << ": " << setw(4) << main_memory[i];
		cout << dec;
	}
	cout << endl << endl << ">\tEnd of dump" << endl << endl;
}

// Print the contents of a queue, with a header containing the queue name
void printQueue(string queueName, queue<User> &q, int num)
{
	cout << endl << "\t>>>>>>QUEUE: " << queueName << "<<<<<<" << endl;
	printQueue(q, num);
}

// Print the contents of a queue
void printQueue(queue<User> &q, int num)
{
    if(!num)
    {
        cout << endl;
        return;
    }
    User curr= q.front();
    q.pop();
		registers regs = curr.regs;
    cout << "\tUSER " << curr.id << ": Dumping Registers..." << endl;
		cout << "\t\trA: " << regs.rA << ", r1: " << regs.r1 << ", r2: " \
			<< regs.r2 << ", r3: " << regs.r3 << endl;
		cout << "\t\tIR: " << regs.IR << ", PC: " << regs.PC << ", CR: " \
			<< regs.CR << endl;
    q.push(curr);
    printQueue(q,--num);
}

// If main memory is unlocked and a process requests it, lock it.
// If it's already locked, block the process.
// Returns true if user has to wait, false if user doesn't have to wait.
bool semwait()
{
	// If the memory is unlocked, lock it, return false
	if (semaphore == unlocked) {
		semaphore = locked;
		return false;
	// If memory is blocked, call dispatcher with BLOCKED keyword, return true
	} else {
		dispatcher(BLOCKED);
		return true;
	}
}

// Signal that main memory has become available, and the
// current program is no longer running.
void semsignal()
{
	string titleFiller = "####################"\
	"################################";
	if (semaphore == locked) {
		semaphore = unlocked;
		cout << titleFiller << endl;
		cout << "USER " << currentUser.id << " HAS SIGNALED FOR " \
			"UNLOCK OF MEMORY" << endl;
		cout << titleFiller << endl << endl;

		int i = 0;
		for (int i = 0; i < blockedQueue.size(); i++) {
			User x = blockedQueue.front();
			blockedQueue.pop();
			cout << titleFiller << endl;
			cout << "USER " << x.id << " WAS PUT BACK INTO READY QUEUE" << endl;
			cout << titleFiller << endl << endl;
			readyQueue.push(x);
		}
	}
}

// Responsible for swapping users in and out, and controlling
// which users are blocked vs which are ready.
void dispatcher(int action)
{
	string titleFiller = "####################################################";
	// Save user state
	currentUser.regs = machine;
	if (action == READY) {
		// Return user to readyQueue
		readyQueue.push(currentUser);
	} else {
		blockedQueue.push(currentUser);
		cout << endl << titleFiller;
		cout << endl << "USER "  << currentUser.id << " WAS DENIED ACCESS TO "\
			<< "MEMORY AND PUT INTO" << endl << "THE BLOCKED QUEUE UNTIL ANOTHER " \
			<< "PROCESS FINISHES";
		cout << endl << titleFiller << endl << endl;
	}
		// Load next user
		currentUser = readyQueue.front();
		readyQueue.pop();
		// Load user state
		machine = currentUser.regs;

		// Output information about user switch

		cout << endl << titleFiller << endl;
		cout << "############### Switching to user " << currentUser.id \
		<< " ################" << endl;
		cout << titleFiller << endl << endl;
}

// Round-robin scheduler, 3 ticks per user
void scheduler()
{
	string titleFiller = "######################################" \
	 "##############";
	// Scheduler code
	while (true) {
		// Load next user and assign its max time (3 ticks)
		// NOTE: If we return to this point in the loop, the dispatcher was not
		// 			 called during last iteration, i.e. current user was not BLOCKED,
		// 			 i.e. current user must be READY.
		dispatcher(READY);
		switchTime = sysclock + 3;

		while (sysclock < switchTime) {
			bool running = currentUser.running;
			bool instrLoaded = !(getOpcode(machine.IR) == 15);
			sysclock++;
			// If current user is not running, prompt for a command
			if (currentUser.id != sys && !running && !instrLoaded) {
				cout << "USER" << (currentUser.id == u1 ? 1 : 2) << " > ";
			// If current user is not running, but has an instruction loaded, begin
			// executing instructions (user just restored from blocked state)
			} else if (currentUser.id != sys && !running && instrLoaded) {
				// If memory is free, run as usual.
				if (semwait() == false) {
					cout << endl << titleFiller;
					cout << endl << "USER " << currentUser.id << " REQUESTED MEM " \
						<< "ACCESS AND ACCESS WAS GRANTED." << endl << "USER MAY " \
						<< "EXECUTE INSTRUCTIONS.";
					cout << endl << titleFiller << endl << endl;
					currentUser.running = true;
					interpreter();
					// If last instruction executed was HLT
					if (machine.IR == 61440) {
						// Signal semaphore is unlocked
						semsignal();
						currentUser.running = false;
						// Exit loop (current user is done)
						switchTime = sysclock;
					}
				// If memory is locked, dispatch to blockedQueue, load next user
				// Reset switchtime (i.e. reset the loop) so next user gets
				// 3 ticks.
				} else {
					switchTime = sysclock + 3;
					break;
				}
			// If current user is system, prompt as system
			} else if (currentUser.id != sys && running) {
				interpreter();
				// If last instruction executed was HLT
				if (machine.IR == 61440) {
					// Signal semaphore is unlocked
					semsignal();
					currentUser.running = false;
					// Exit loop (current user is done)
					switchTime = sysclock;
				}
			} else {
				cout << "SYS > ";
			}

		// If necessary, take input from command line
		if ((!running  && !instrLoaded) || currentUser.id == sys) {
			string input;
			getline(cin, input);
			switch (cmdToInt(input)) {
				case 0: // "run"
					if (currentUser.id != sys) {
						machine.IR = main_memory[machine.PC];
						// If memory is free, run as usual.
						if (semwait() == false) {
							cout << endl << titleFiller;
							cout << endl << "USER " << currentUser.id << " REQUESTED MEM " \
								<< "ACCESS AND ACCESS WAS GRANTED." << endl << "USER MAY " \
								<< "EXECUTE INSTRUCTIONS.";
							cout << endl << titleFiller << endl << endl;
							currentUser.running = true;
							interpreter();
							// If last instruction executed was HLT
							if (machine.IR == 61440) {
								// Signal semaphore is unlocked
								semsignal();
								currentUser.running = false;
								// Exit loop (current user is done)
								switchTime = sysclock;
							}
						// If memory is locked, dispatch to blockedQueue, load next user
						// Reset switchtime (i.e. reset the loop) so next user gets
						// 3 ticks.
						} else {
							switchTime = sysclock + 3;
						}
					} else {
						cout << "Invalid command for system" << endl;
					}
					break;
				case 1: // "dmp"
					if (currentUser.id == sys) {
						dump();
					} else {
						cout << "Invalid command for users" << endl;
					}
					break;
				case 2: // "nop"
					// Exit loop (current user is done)
					switchTime = sysclock;
					break;
				case 3: // "stp"
					if (currentUser.id == sys) {
						exit(EXIT_SUCCESS);
					} else {
						cout << "Invalid command for users" << endl;
					}
					break;
				default:
					cout << "Invalid command: " << input << endl;
					break;
				}
			}
		}
	}
}

// Read program into memory
void readFile()
{
	// This method will need to be rewritten entirely using c++ streams
	ifstream infile("part2.dat");
	int i = 0;
	short unsigned int current;
	string line;
	while (std::getline(infile, line))
	{
		if (line == "*") {
			i = 100;
			continue;
		}
		char * ptr;
		current = strtol(line.c_str(), & ptr, 2);
		main_memory[i] = current;
		i++;
	}
}

// Convert a command as a string into an integer value to simplify
// switch statements using commands.
int cmdToInt(string cmd)
{
	if (my_strcasecmp(cmd, "run")) return 0;
	else if (my_strcasecmp(cmd, "dmp")) return 1;
	else if (my_strcasecmp(cmd, "nop")) return 2;
	else if (my_strcasecmp(cmd, "stp")) return 3;
	else return -1;
}

// Case-insensitive string comparison
bool my_strcasecmp(string str1, string str2)
{
	int length1 = str1.length();
	for (int i = 0; i < length1; i++) {
		char &c = str1.at(i);
		c = tolower(c);
	}
	int length2 = str2.length();
	for (int i = 0; i < length2; i++) {
		char &c = str2.at(i);
		c = tolower(c);
	}
	return ((str1 == str2) ? true : false);
}

// Initializes all values required by the OS
void init()
{
	// Initialize main_memory
	for (int i = 0; i < 255; i++) {
		main_memory[i] = 0;
	}
	semaphore = unlocked;

	registers defaultRegisterValues =
	{				// Registers:
		0,			// R1
		0,			// R2
		0,			// R3
		0,			// RA
		61440,	// IR
		0,			// PC
		0				// CR
	};
	User U1 = {u1, false, defaultRegisterValues};
	User U2 = {u2, false, defaultRegisterValues};
	User SYS = {sys, false, defaultRegisterValues};

	// Read in the program for u1 and u2
	readFile();
	U1.regs.PC = 0;
	U2.regs.PC = 100;

	readyQueue.push(U1);
	readyQueue.push(U2);
	currentUser = SYS;

	// Initialize sysclock and switchTime
	sysclock = 0;
	switchTime = sysclock + 3;
}

// Main function (starts the OS)
int main(int argc, char** argv)
{
	// Print OS startup header
	string titleFiller = "####################################################";
	string titleText = "################ CSC 341 OS Lab ####################";
	cout << endl << titleFiller << endl \
		<< titleText << endl \
		<< titleFiller << endl << endl;
	// Initialization
	init();
	// Start scheduler
	scheduler();
	return 0;
}
