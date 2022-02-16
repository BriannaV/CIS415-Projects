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

int main(int argc, char *argv[]) {

	// this opens the given file
	FILE* f;
	f = fopen(argv[1], "r");

	int len_file;

	// this makes sure that the file was a valid file 
	// and there was something there
	if(f == NULL) {
		printf("ERROR. No file found: %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	
	// find the length of the file (num of commands)
	len_file = num_command(f);

	size_t bufsize = 256;
	ssize_t val = 0;
	char* line_buf;
	line_buf = (char*)malloc(bufsize * sizeof(char));
	
	// get a line from the file and then remove the \n from the end
	val = getline(&line_buf, &bufsize, f);
       	rm_line(line_buf);
	
	pid_t array_child[len_file];
	char* save;
	int current = 0;

	// while the line is more than 0
	while(val >0) {
		// find the number of arguments per line and make an
		// array of them. Make a temporary variable with a tokenized
		// line buf (one argument)
		int arg_num = arg_per_line(line_buf);
		char* array_arg[arg_num+1];
		array_arg[arg_num] = NULL;
		char* temp = strtok_r(line_buf, " ", &save);
		
		// use the for loop to put each of the arguments
		// into the array of arguments
		for(int i=0; i<arg_num;i++) {
			array_arg[i] = temp;
			temp = strtok_r(NULL, " ", &save);
		}

		// fork the current child
		array_child[current] = fork();

		// if the current child is zero then run execvp and
		// if that doesnt work print an error message
		if(array_child[current] == 0) {
			if(array_child[current] = execvp(array_arg[0], array_arg) < 0); {
				perror("ERROR. Execvp(): ");
				free(line_buf);
				fclose(f);
				exit(-1);
			}
		}
		// get another line in the file and remove the \n from it
		// add 1 to the current child 
		val = getline(&line_buf, &bufsize, f);
		rm_line(line_buf);
		current++;
	}

	int stat;
	
	// for the length of the file wait for the children to complete
	for(int i=0; i<len_file; i++) {
		while(waitpid(array_child[i], &stat, 0) > 0);
	}
	fclose(f);
	free(line_buf);
	exit(0);
}
