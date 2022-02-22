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

		// while the argument isnt null
		// put the argument inside of the array
		while(argument != NULL && i <12) {
			argv[i] = argument;
			argument = strtok(NULL, delim);
			i++;
		}
		argv[i] = NULL;

		// fork child
		pid = fork();

		// if child doesnt exist
		if(pid <0) {
			printf("ERROR: Unable to fork\n");
			return -1;
		}
		// if child does exist and is first
		else if(pid == 0) {
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

	// for programs print parent waits and wait
	for(int x=0;x<program_total;x++) {
		printf("Parent: %d waits for Child\n", parent);
		waitpid(pid_vals[x], NULL, 0);
	}
	// all processes done
	printf("All processes completed. Exiting..\n");
	exit(EXIT_SUCCESS);
}


int main(int argc, char* argv[]) {
	int status = -1;

	// checks to see if file is there
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

























