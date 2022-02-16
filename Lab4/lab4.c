#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void script_print (pid_t* pid_ary, int size);

int main(int argc,char*argv[])
{
	if (argc == 1)
	{
		printf ("Wrong number of argument\n");
		exit (0);
	}

	// opens log.txt to write in it later
	FILE* f = freopen("log.txt","w",stdout);
	// gets number of processes to make
	int n = atoi(argv[1]);
	// makes the argument list for running execvp()
	char* arguments[] = {"./iobound", "-seconds", "5", NULL};
	// gets the current pid and puts it into a int
	int pid = getpid();
		
	// makes an array and mallocs for it
	pid_t *array_pid;
	array_pid = (pid_t*)malloc(sizeof(pid_t)*n);

	// for loop to make children
	for(int i=0;i<n;i++) {
		// fork() -> makes child (proosses creation)
		array_pid[i] = fork();
		
		// if statement to make sure it makes child
		// if it cant then it will write in log
		if(array_pid[i] < 0) {
			printf("Can't make child\n");
			free(array_pid);
		}

		// if the pid doesnt equal the one got
		// and the value of execvp() is neg one
		// then the new process isnt made
		// if it isnt then it will write in log
		if(pid != getpid()) {
			// execvp() -> replaces current process
			// image with a new image specified
			// by the file
			if(execvp("./iobound", arguments) == -1) {
				printf("New process unable to be made\n");
				free(array_pid);
			}
			exit(0);
		}
	}
	// run stript print (given at bottom)
	script_print(array_pid, n);

	// for loop used to suspend execution of current process
	int val;
	for(int i=0;i<n;i++) {
		// waitpid() -> suspends execution of current
		// process until one child terminates
		waitpid(array_pid[i], &val, 0);
	}
	// free and close things
	free(array_pid);
	fclose(f);

	/*
	*	TODO
	*	#1	declear child process pool
	*	#2 	spawn n new processes
	*		first create the argument needed for the processes
	*		for example "./iobound -seconds 10"
	*	#3	call script_print
	*	#4	wait for children processes to finish
	*	#5	free any dynamic memories
	*/

	return 0;
}


void script_print (pid_t* pid_ary, int size)
{
	FILE* fout;
	fout = fopen ("top_script.sh", "w");
	fprintf(fout, "#!/bin/bash\ntop");
	for (int i = 0; i < size; i++)
	{
		fprintf(fout, " -p %d", (int)(pid_ary[i]));
	}
	fprintf(fout, "\n");
	fclose (fout);

	char* top_arg[] = {"gnome-terminal", "--", "bash", "top_script.sh", NULL};
	pid_t top_pid;

	top_pid = fork();
	{
		if (top_pid == 0)
		{
			if(execvp(top_arg[0], top_arg) == -1)
			{
				perror ("top command: ");
			}
			exit(0);
		}
	}
}


