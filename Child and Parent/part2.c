#include <stdlib.h> 
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "MCP.h"
#include <sys/wait.h>
#include <sys/types.h>

int file_fork(char* name) {
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

		// while argument isnt null put
		// argument in array
		while(argument != NULL && i <12) {
			argv[i] = argument;
			argument = strtok(NULL, delim);
			i++;
		}
		argv[i] = NULL;

		// fork child
		pid = fork();

		// if cild doesnt exist print error
		if(pid <0) {
			printf("ERROR: Unable to fork\n");
			return -1;
		}
		// if child is first block it until gets
		// SIGUSR1
		else if(pid == 0) {
			sigprocmask(SIG_BLOCK, &user_set, &old_set);
			printf("Child: %d waiting to get SIGUSR1\n", getpid());
			i = sigwait(&user_set, &sig);

			// if i is 0 then print child
			// has SIGUSR1
			if(i==0) {
				printf("Child: %d has SIGUSR1\n", getpid());
			}
			// run command
			execvp(command, argv);
		}
		// else print parent created child
		else {
			pid_vals[program_total] = pid;
			printf("Parent: %d created Child: %d\n", parent, pid);
			program_total++;
		}
	}
	free(buf);
	fclose(f);
	
	sleep(1);

	int x=0;

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

	// send SIGCONT to child
	for(x=0; x<program_total;x++) {
		printf("Parent: %d send SIGCONT to Child: %d\n", parent, pid_vals[x]);
		kill(pid_vals[x], SIGCONT);
	}

	// for each program wait for child
	for(x=0;x<program_total;x++) {
		printf("Parent: %d waits for Child\n", parent);
		waitpid(pid_vals[x], NULL, 0);
	}
	// all processes done
	printf("All processes completed. Exiting..\n");
	exit(EXIT_SUCCESS);
}


int main(int argc, char* argv[]) {
	int status = -1;

	// checks to see if arguments are corrent
	if(argc != 2) {
		printf("ERROR: No specified file\n");
		return EXIT_FAILURE;
	}
	// run file_fork
	else {
		status = file_fork(argv[1]);
		if(status == -1) {
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}

























