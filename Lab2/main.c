#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <stdbool.h>
#include "command.h"

int main() {
	setbuf(stdout, NULL);
	
	size_t size = 100;
	char* input;
	char* token;
	char* token_count;

	input = (char*)malloc(sizeof(char)*size);
	
	bool loop = true;

	while(loop) {
		printf(">>>");
		getline(&input, &size, stdin);
		
		token = strtok(input, " ");
		
		if(strcmp(token, "exit\n") == 0) {
			printf("\n");
			break;
		}

		token_count = 0;
		while(token != NULL && strcmp(token, "\n")){
			if(strcmp(token, "lfcat\n") == 0){
				lfcat();
				loop = false;
				break;
			}
			else {
				printf("Error: Unrecognized command!\n");
			}
			token = strtok(NULL, " ");
		}
	}
	free(input);
	return 0;	
}



