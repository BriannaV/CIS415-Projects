#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int num_command(FILE* file) {
	/* This function counts the total number
	 * of commands that were in a file (a line)*/
	int buf_size = 256;
	int return_val = 0;
	
	char* line_buf;
	line_buf = (char*)malloc(buf_size * sizeof(char));

	while(fgets(line_buf, buf_size, file) != NULL) {
		return_val ++;
	}
	fclose(file);
	free(line_buf);
	return return_val;
}

void rm_line(char* in) {
	/* This function removes the new line
	 * symbol out of the lines so that it
	 * is easier to work with*/
	int len = strlen(in);

	for(int i=0;i <len;i++) {
		char copy_line = in[i];
		if(strcmp(&in[i], "\n") ==0) {
			in[i] = '\0';
		}
	}
}

int arg_per_line(char* line) {
	/* This function counts the total number
	 * of arguments that are in a command (line)*/
	int len = strlen(line);
	int num_args = 0;
	
	for(int i=0; i < len; i++) {
		char copy_line = line[i];
		
		if(&line[i] == " ") {
			num_args ++;
		}
	}
	num_args = num_args +1;
	return num_args;
}

void printer(pid_t *pid_array, int size) {
	/* This code prints things to an script.sh file*/
	FILE* f;
	f = fopen("script.sh", "w");
	fprintf(f, "#!/bin/bash\ntop");
	
	for(int i=0; i<size;i++) {
		fprintf(f, "-p %d", (int)(pid_array[i]));
	}
	fprintf(f, "\n");
	fclose(f);
}

void signal_change(pid_t* pool, int total_child, int sign, int wait) {
	/* This code changes the signal the parent is 
	 * sending the child*/
	sleep(wait);
	
	for(int i=0; i< total_child;i++) {
		printf("Parent: <%d> -- Send signal: <%d> to new child <%d>\n", getpid(), sign, pool[i]);
		kill(pool[i], sign);
	}
}

int main(int argc, char* argv[]) {
	
	// this code gets file and opens it
	FILE* f;
	f = fopen(argv[1], "r");

	// if the file is null then it will print an error message
	if(f == NULL) {
		printf("ERROR. No file found: %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	int total_line;
	total_line = num_command(f);
	
	size_t bufsize = 256;
	ssize_t val = 0;
	pid_t array_child[total_line];

	char* line_buf;
	line_buf = (char*)malloc(bufsize * sizeof(char));

	// get a line from the file and remove the \n from it
	val = getline(&line_buf, &bufsize, f);
	rm_line(line_buf);

	char* save;
	int current = 0;

	sigset_t sign;
	int signal_number;

	// sigemptyset() -> initializes signal to empty
	// (no signal)
	sigemptyset(&sign);
	// sigaddset() -> sets the signal to whatever
	// is given (SIGUSR1)
	sigaddset(&sign, SIGUSR1);

	struct sigaction act = {0};
	//act.sa_handler = handler;
	// sigaction() -> allows calling process to specify
	// action to be associated with signal
	sigaction(SIGUSR1, &act, NULL);

	while(val > 0) {
		// get the number of arguments in a line,
		// make an array for the arguments
		int num_arg = arg_per_line(line_buf);
		char* array_arg[num_arg+1];
		array_arg[num_arg] = NULL;

		char* temp = strtok_r(line_buf, " ", &save);
		
		// put each argument into the array
		for(int i=0; i<num_arg;i++) {
			array_arg[i] = temp;
			temp = strtok_r(NULL, " ", &save);
		}
		// fork the current child
		array_child[current] = fork();

		// if the current child is 0 then make child
		// wait until ready and then run exec
		if(array_child[current] == 0) {
			printf("Child: <%d> -- Waiting for SIGUSR1\n", getpid());
			// sigwait() -> pauses execution of the child
			// until something is ready
			sigwait(&sign, &signal_number);
			printf("Child: <%d> -- Recieved signal SIGUSR1 -- Now calling exec()\n", getpid());
			// if the execvp doesnt work send error
			if(array_child[current] = execvp(array_arg[0], array_arg) < 0); {
				perror("ERROR. Execvp did not work\n ");
				free(line_buf);
				fclose(f);
				exit(-1);
			}
		}
		// get a new line and add 1 to the current child
		// to move on to the next one
		val = getline(&line_buf, &bufsize, f);
		rm_line(line_buf);
		current++;
	}
	sleep(1);
	// run child in printer and then send child signal change
	// to SIGUSR1
	printer(array_child, total_line);
	printf("\nSending each child to SIGUSR1\n");
	signal_change(array_child, total_line, SIGUSR1, 2);

	// call exec and send child to signal change with SIGSTOP
	printf("\nCalling exec and sending each child to SIGSTOP\n");
	signal_change(array_child, total_line, SIGSTOP, 3);

	// send child to signal change with SIGCONT
	printf("\nSending each child to SIGCONT\n");
	signal_change(array_child, total_line, SIGCONT, 3);

	// for each command wait for the child to complete 
	int stat;
	for(int i=0;i<total_line;i++) {
		while(waitpid(array_child[i], &stat, 0) >0);
	}

	free(line_buf);
	fclose(f);
	printf("\n All processes have been completed now\n");
	exit(0);
}

























