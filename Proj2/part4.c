#include <stdlib.h> 
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "MCP.h"
#include <sys/wait.h>
#include <sys/types.h>


void alarm_handler(int signum) {
	/* This is the alarm handler function*/
	/*if(signum == SIGALRM) {
		write(STDOUT_FILENO, "Parent got SIGALRM\n", 32);
	}*/
	return;
}


void print_proc(pid_t pid) {
	/* print selected information from proc specified by pid */
	char* buf = NULL;
	char* tok;
	char* command;
	
	char state;
	char str_array[20];
	char delim[2] = " ";

	size_t bufsiz = sizeof(char)*500;
	
	int ppid;
	int group_id;
	int terminal;

	long first;

	unsigned long user_time;
	unsigned long vm_size;
	unsigned long kernel_time;

	unsigned long long start_time;

	// this line prints something werid
	sprintf(str_array, "/proc/%d/stat", pid);

	// opens file and gets the specified line
	FILE *f = fopen(str_array, "r");
	getline(&buf, &bufsiz, f);

	// tokenize tok and command
	tok = strtok(buf, delim);

	command = strtok(NULL, delim);

	// scan for the state, ppid, and group id
	sscanf(strtok(NULL, delim), "%c", &state);
	sscanf(strtok(NULL, delim), "%d", &ppid);
	sscanf(strtok(NULL, delim), "%d", &group_id);

	strtok(NULL, delim);

	// scan for terminal	
	sscanf(strtok(NULL, delim), "%d", &terminal);

	int i;

	// skip the lines between 7 and 13
	for(i=0; i< (13-7); i++) {
		strtok(NULL, delim);
	}

	// scan for user time and kernel time
	sscanf(strtok(NULL, delim), "%lu", &user_time);
	sscanf(strtok(NULL, delim), "%lu", &kernel_time);

	// skip the lines between 15 and 17
	for(i=0; i<(17-15);i++) {
		strtok(NULL, delim);
	}

	// scan for the priority
	sscanf(strtok(NULL, delim), "%ld", &first);

	// skil the lines between 18 and 21
	for(i=0; i<(21-18);i++) {
		strtok(NULL, delim);
	}
	
	// scan for start time and vm size
	sscanf(strtok(NULL, delim), "%llu", &start_time);
	sscanf(strtok(NULL, delim), "%lu", &vm_size);

	// print out all of that information with tabs in between to space out
	printf("%d\t", pid);
	printf("%s\t", command);
	printf("%c\t", state);
	printf("%d\t", ppid);
	printf("%d\t\t", group_id);
	printf("%d\t\t", terminal);
	printf("%lu\t\t", (user_time/sysconf(_SC_CLK_TCK)));
	printf("%lu\t\t", (kernel_time/sysconf(_SC_CLK_TCK)));
	printf("%llu\t\t", (start_time/sysconf(_SC_CLK_TCK)));
	printf("%lu\t\t", vm_size >>10);
	printf("%ld\t", first);
	printf("\n");

	fclose(f);
}

int file_fork(char* name) {
	/* This function runs all the parts */
	FILE *f;
	char* buf = NULL;
	size_t bufsiz = sizeof(char)* 100;

	const char delim[3] = "\n";
	char* argv[12];
	char* command;
	char* argument;

	pid_t parent;
	parent = getpid();
	
	sigset_t user_set;
	sigset_t old_set;
	
	sigemptyset(&user_set);
	sigaddset(&user_set, SIGUSR1);

	int sig;
	
	// open file
	f = fopen(name, "r");
	if(f == NULL) {
		printf("ERROR: Could not open file. \n");
		return -1;
	}
	else {
		printf("File Opened.\n");
	}

	pid_t pid;
	pid_t pid_vals[15];
	int program_total = 0;

	// fork & execute based on file
	while(!feof(f)) {
		getline(&buf, &bufsiz, f);
		argument = strtok(buf, delim);
		if(argument != NULL) {
			command = argument;
			argv[0] = argument;
		}
		argument = strtok(NULL, delim);
		int i = 1;

		// while the argument isnt NULL
		while(argument != NULL && i <12) {
			argv[i] = argument;
			argument = strtok(NULL, delim);
			i++;
		}
		argv[i] = NULL;

		// fork child
		pid = fork();

		// if child doesnt exist print error
		if(pid <0) {
			printf("ERROR: Unable to fork\n");
			return -1;
		}
		// if child is the first one block it until it gets SIGUSR1
		else if(pid == 0) {
			sigprocmask(SIG_BLOCK, &user_set, &old_set);
			printf("Child: %d waiting to get SIGUSR1\n", getpid());
			i = sigwait(&user_set, &sig);

			// if i is 0 then print that it has SIGUSR1
			if(i==0) {
				printf("Child: %d has SIGUSR1\n", getpid());
			}
			// run command
			execvp(command, argv);
		}
		// else print that child was made
		else {
			pid_vals[program_total] = pid;
			printf("Parent: %d created Child: %d\n", parent, pid);
			program_total++;
		}
	}
	free(buf);
	fclose(f);
	
	sleep(1);

	int x = 0;

	// send SIGUSR to child
	for(x=0; x<program_total; x++) {
		printf("Parent %d sending SIGUSR1 to Child: %d\n", parent, pid_vals[x]);
		kill(pid_vals[x], SIGUSR1);
	}
	
	sleep(1);

	// send SIGSTOP to child
	for(x=0; x<program_total;x++) {
		printf("Parent: %d send SIGSTOP to Child: %d\n", parent, pid_vals[x]);
		kill(pid_vals[x], SIGSTOP);
	}

	// alarm stuff
	struct sigaction alrm_act = {0};
	alrm_act.sa_handler = &alarm_handler;
	sigaction(SIGALRM, &alrm_act, NULL);

	int leave = 0;
	int y;
	
	// while leave is false (0)
	while(leave == 0) {
		waitpid(-1, NULL, WNOHANG);
		// leave is true (1)  if there is one child still
		leave = 1;
		// for each program in array
		for(y=0;y<program_total;y++) {
			// if the child is still there
			if(kill(pid_vals[y], 0) != -1) {
				// continue, alarm
				kill(pid_vals[y], SIGCONT);
				alarm(1);
				printf("Parent: %d resumed Child: %d\n", parent, pid_vals[y]);
				pause();

				//check child again
				if(kill(pid_vals[y], 0) != -1) {
					// stop
					kill(pid_vals[y], SIGSTOP);
					printf("Parent: %d stopped Child: %d\n", parent, pid_vals[y]);
					leave &= 0;
				}
			}
		}
		// print info about process that is currently running
		int z = 0;

		// if z is divisible by 2, print otu a table with:
		// pid, command, state, ppid, group id, terminal, 
		// user time, kernel time, start time, vm size, and priority
		// run the print proc here to get those values
		if(z%2 ==0) {
			system("clear");
			printf("--------------------------------------------------\n");
			printf("PID\tCommand\t\tState\tPPID\tGroup ID\tTerminal\tUserTime (s)\tKernelTime (s)\tStartTime (s)\tVMSize (KB)\tPriority\n");
			// print proc for parent
			print_proc(parent);
			// for program in program total, print proc for
			// each child
			for(y=0;y<program_total;y++) {
				if(kill(pid_vals[y], 0) != -1) {
					print_proc(pid_vals[y]);
				}
			}
			printf("--------------------------------------------------\n");
		}
		z++;
	}
	// all processes done
	printf("All processes completed. Exiting..\n");
	exit(EXIT_SUCCESS);
}


int main(int argc, char* argv[]) {
	int status = -1;

	// check to see if arguments are valid
	if(argc != 2) {
		printf("ERROR: No specified file\n");
		return EXIT_FAILURE;
	}
	// runs file_fork
	else {
		status = file_fork(argv[1]);
		if(status == -1) {
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}

