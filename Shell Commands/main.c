#include "command.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* removeChar(char* command) {
	/* This function removes characters from command when needed*/
	int len = strlen(command) +1;
	char* return_char = (char*)malloc(sizeof(char)*len);
	memset(return_char, '\0', sizeof(char)* len);
	
	// for each thing in command, check if it is a space or endline and
	// add to return char if it isnt
	for(int i=0;i<len;i++) {
		if(command[i] != '\n' && command[i] != ' ') {
			strncat(return_char, &command[i], 1);
		}
	}
	return return_char;
}


int main(int argc, char* argv[]) {
	int chars;
	int mode = 0;
	char* save;
	char* token;
	char* temp_save;
	size_t buf_size = 256;
	char* line_buf;
	line_buf = (char*)malloc(buf_size * sizeof(char));
	
	FILE* out;
	FILE* file_name;
	
	// if argc is more than one
	if(argc > 1) {
		// if the next argument is -f then it will set mode and open
		// output.txt
		if(strcmp(argv[1], "-f") ==0) {
			mode = 1;
			out = freopen("output.txt", "w+", stdout);
		}
		// open the second thing in argv
		file_name = fopen(argv[2], "r");

		// if the file name is null then print error
		if(file_name == NULL) {
			printf("No valid file found: %s\n", argv[2]);
			exit(EXIT_FAILURE);
		}
	}
	// if mode is zero (not -f) 
	if(mode == 0) {
		// while loop
		while(1) {
			// print the arrows and get the line written
			// from stdin
			printf(">>> ");
			chars = getline(&line_buf, &buf_size, stdin);
			
			int len = 0;
			char* line;
			char* command;
			char* temp_command;
			char* arg;
			char* temp_arg1;
			char* temp_arg2;
			char* ptr_command;
			
			// tokenize it for ;
			line = strtok_r(line_buf, ";", &save);

			// while loop
			while(line) {
				// tokenize it for space
				temp_command = strtok_r(line, " ", &ptr_command);
				// send temp command to remove char function
				command = removeChar(temp_command);

				// if the string is ls then do the function
				if (strcmp(command, "ls") ==0 && strlen(ptr_command) == 0) {
					listDir();
				}

				// if the string is pwd then do function
				else if(strcmp(command, "pwd") == 0 && strlen(ptr_command) == 0) {
					showCurrentDir();
				}
				// if the string is mkdir
				else if(strcmp(command, "mkdir") == 0) {
					// if there is something in ptr
					// command
					if(strlen(ptr_command) > 0) {
						// the first token is arg1
						temp_arg1 = strtok_r(NULL, " ", &ptr_command);
						// if the pointer command is
						// empty then call function
						if(strlen(ptr_command) == 0) {
							makeDir(temp_arg1);
						}
						// else print error
						else {
							printf("Error! Incorrect Syntax. No control code found\n");
						}
					}
					// else print error
					else {
						printf("Error! Incorrect Syntax. No control code found\n");
					}
				}
				// if the string is cp
				else if(strcmp(command, "cp") ==0) {
					// if ptr_command has something
					if(strlen(ptr_command) > 0) {
						// the first token is arg1
						temp_arg1 = strtok_r(NULL, " ", &ptr_command);
					}
					// if ptr_command still has something
					if(strlen(ptr_command) > 0) {
						// the second token is arg2
						temp_arg2 = strtok_r(NULL, " ", &ptr_command);
						// if ptr_command is now empty
						// call function
						if(strlen(ptr_command) == 0) {
							copyFile(temp_arg1, temp_arg2);
						}
						// else print error
						else {
							printf("Error! Incorrect Syntax. No control code found\n");
						}
					}
					// else print error
					else {
						printf("Error! Incorrect Syntax. No control code found\n");
					}
				}
				// if string is mv
				else if(strcmp(command, "mv") == 0) {
					// if ptr command is more than zero
					if(strlen(ptr_command) > 0) {
						// first token is arg1
						temp_arg1 = strtok_r(NULL, " ", &ptr_command);
					}
					// if ptr command more than zero
					if(strlen(ptr_command) > 0) {
						// second token is arg2
						temp_arg2 = strtok_r(NULL, " ", &ptr_command);
						// if ptr command now zero
						// call function
						if(strlen(ptr_command) == 0) {
							moveFile(temp_arg1, temp_arg2);
						}
						// else print error
						else{
							printf("Error! Incorrect Syntax. No control code found\n");
						}
					}
					// else print error
					else {
						printf("Error! Incorrect Syntax. No control code found\n");
					}
				}
				// if string is rm
				else if(strcmp(command, "rm") == 0) {
					// if ptr command more than zero
					if(strlen(ptr_command) > 0) {
						// first token is arg1
						temp_arg1 = strtok_r(NULL, " ", &ptr_command);
						// if ptr command now zero
						// call function
						if(strlen(ptr_command) == 0) {
							deleteFile(temp_arg1);
						}
						// else print error
						else {
							printf("Error! Incorrect Syntax. No control code found\n");
						}
					}
					// else print error
					else {
						printf("Error! Incorrect Syntax. No control code found\n");
					}
				}
				// if string is cat
				else if(strcmp(command, "cat") == 0) {
					// if ptr command more than zero
					if(strlen(ptr_command) > 0) {
						// first token is arg1
						temp_arg1 = strtok_r(NULL, " ", &ptr_command);
						// if ptr command now zero
						// call function
						if(strlen(ptr_command) == 0) {
							displayFile(temp_arg1);
						}
						// else print error
						else {
							printf("Error! Incorrect Syntax. No control code found\n");
						}
					}
					// else print error
					else { 
						printf("Error! Incorrect Syntax. No control code found\n");
					}
				}
				// if string is cd
				else if(strcmp(command, "cd") == 0) {
					// if ptr command more than zero
					if(strlen(ptr_command) > 0) {
						// first token is arg1
						temp_arg1 = strtok_r(NULL, " ",&ptr_command);
						// if ptr command now zero
						// call function
						if(strlen(ptr_command) == 0) {
							changeDir(temp_arg1);
						}
						// else print error
						else {
							printf("Error! Incorrect Syntax. No control code found\n");
						}
					}
					// else print error
					else {
						printf("Error! Incorrect Syntax. No control code found\n");
					}
				}
				// if string is error send exit success
				else if(strcmp(command, "exit") == 0) {
					exit(EXIT_SUCCESS);
				}
				// else print error
				else {
					printf("Error! Incorect Syntax. No control code found\n");
				}
				// free command and set line to new token
				free(command);
				line = strtok_r(NULL, ";", &save);
			}
		}
	}
	// if mode is not zero
	if(mode) {
		char* arg;
		char* temp_arg1;
		char* temp_arg2;
		char* command;
		char* temp_command;
		char* ptr_command;
		char* line;
		int len = 0;

		// while the line in file isnt null
		while(fgets(line_buf, buf_size, file_name) != NULL) {
			// set line to first token
			line = strtok_r(line_buf, ";", &save);
			// while loop
			while(line) {
				// set temp command to token of line
				temp_command = strtok_r(line, " ", &ptr_command);
				// run temp command through remove char
				command = removeChar(temp_command);
				
				// if string is ls call function 
				if(strcmp(command, "ls") == 0 && strlen(ptr_command)) {
					listDir();
				}
				// if string is pwd call function
				else if(strcmp(command, "pwd") == 0 && strlen(ptr_command) == 0) {
					showCurrentDir();
				}
				// if string is mkdir
				else if(strcmp(command, "mkdir") == 0) {
					// if ptr command is more than zero
					if(strlen(ptr_command) > 0) {
						// arg1 is first token
						temp_arg1 = strtok_r(NULL, " ", &ptr_command);
						// if ptr command is now zero
						// call function
						if(strlen(ptr_command) == 0) {
							makeDir(temp_arg1);
						}
						//else print error
						else {
							printf("Error! Incorrect Syntax. No control code found\n");
						}
					}
					// else print error
					else {
						printf("Error! Incorrect Syntax. No control code found\n");
					}
				}
				// if string is cp
				else if(strcmp(command, "cp") == 0) {
					// if ptr command is more than zero
					if(strlen(ptr_command) > 0) {
						// first token is arg1
						temp_arg1 = strtok_r(NULL, " ", &ptr_command);
					}
					// if ptr command is more than zero
					if(strlen(ptr_command) > 0) {
						// second token is arg2
						temp_arg2 = strtok_r(NULL, " ", &ptr_command);
						// if ptr is now zero
						// call function
						if(strlen(ptr_command) == 0) {
							copyFile(temp_arg1, temp_arg2);
						}
						// else print error
						else {
							printf("Error! Incorrect Syntax. No control code found\n");
						}
					}
					// else print error
					else {
						printf("Error! Incorrect Syntax. No control code found\n");
					}
				}
				// if string is mv
				else if(strcmp(command, "mv") == 0) {
					// if ptr command is more than zero
					if(strlen(ptr_command) > 0) {
						// first token is arg1
						temp_arg1 = strtok_r(NULL, " ", &ptr_command);
					}
					// if ptr command is more than zero
					if(strlen(ptr_command) > 0) {
						// second token is arg2
						temp_arg2 = strtok_r(NULL, " ", &ptr_command);
						// if ptr command is now zero
						// call function
						if(strlen(ptr_command) == 0) {
							moveFile(temp_arg1, temp_arg2);
						}
						// else print error
						else {
							printf("Error! Incorrect Syntax. No control code found\n");
						}
					}
					// else print error
					else {
						printf("Error! Incorrect Syntax. No control code found\n");
					}
				}
				// if string is rm
				else if(strcmp(command, "rm") == 0) {
					// if ptr command is more than zero 
					if(strlen(ptr_command) > 0) {
						// first token is arg1
						temp_arg1 = strtok_r(NULL, " ", &ptr_command);
						// if ptr command now zero
						// call function
						if(strlen(ptr_command) == 0) {
							deleteFile(temp_arg1);
						}
						// else print error
						else {
							printf("Error! Incorrect Syntax. No control code found\n");
						}
					}
					// else print error
					else {
						printf("Error! Incorrect Syntax. No control code found\n");
					}
				}
				// if string is cat
				else if(strcmp(command, "cat") == 0) {
					// ptr command is more than zero
					if(strlen(ptr_command) > 0) {
						// first token is arg1
						temp_arg1 = strtok_r(NULL, " ", &ptr_command);
						// if ptr command now zero
						// call function
						if(strlen(ptr_command) == 0) {
							displayFile(temp_arg1);
						}
						// else print error
						else {
							printf("Error! Incorrect Syntax. No control code found\n");
						}
					}
					// else print error
					else {
						printf("Error! Incorrect Syntax. No control code found\n");
					}
				}
				// if string is cd
				else if(strcmp(command, "cd") == 0) {
					// ptr command is more than zero
					if(strlen(ptr_command) > 0) {
						// first token is arg1
						temp_arg1 = strtok_r(NULL, " ", &ptr_command);
						// if ptr command is now zero
						// call function
						if(strlen(ptr_command) == 0) {
							changeDir(temp_arg1);
						}
						// else print error
						else {
							printf("Error! Incorrect Syntax. No control code found\n");
						}
					}
				}
				// free command and set line to next token
				free(command);
				line = strtok_r(NULL, ";", &save);
			}
		}
	}
	// if mode isnt zero close file
	if(mode) {
		fclose(file_name);
	}
	// free line buf and return zero
	free(line_buf);
	return 0;
}
