/*
 * string_parser.c
 *
 *  Created on: Nov 25, 2020
 *      Author: gguan, Monil
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_parser.h"

#define _GNU_SOURCE

int count_token (char* buf, const char* delim)
{
	int token_val;
	char*copy = malloc(strlen(buf) +1);
	strcpy(copy, buf);
	if(copy != NULL || copy != delim) {
		if(&copy[0] == delim) {
			//remove delim from the beginning of buf
			memmove(copy, copy+1, strlen(copy));
		}
		if(&copy[strlen(copy)-1] != delim) {
			//remove delim from end of string
			copy[strlen(copy)-1] = 0;
		}
		token_val = 0;
		char* token;
		while(token = strtok_r(copy, delim, &copy)) {
			token_val = token_val + 1;
		}
	}
	else {
		token_val = 0;
	}
	return token_val;
	/*
	*	#1.	Check for NULL string
	*	#2.	iterate through string counting tokens
	*		Cases to watchout for
	*			a.	string start with delimeter (eliminate)
	*			b. 	string end with delimeter (eliminate)
	*			c.	account NULL for the last token
	*	#3. return the number of token (note not number of delimeter)
	*/
}

command_line str_filler (char* buf, const char* delim)
{	
	
	command_line* command;
	command = malloc(sizeof(*command));
	char*save = malloc(strlen(buf)+1);
	strcpy(save, buf);
	command->num_token = count_token(save, delim)-1;
	command->command_list = (char**)malloc(command->num_token +1);
	char *token;
	char *copy = malloc(strlen(buf)+1);
	strcpy(copy, buf);
	int i=0;
	while((token = strtok_r(copy, delim, &copy))) {
		if(token != NULL){
			command->command_list[i] = (char*)malloc(strlen(token));
			command->command_list[i] = token;
			i += 1;
		}
	}
	return *command;
	/*
	*	#1.	create command_line variable to be filled and returned
	*	#2.	count the number of tokens with count_token function, set num_token. 
    *           one can use strtok_r to remove the \n at the end of the line.
	*	#3. malloc memory for token array inside command_line variable
	*			based on the number of tokens.
	*	#4.	use function strtok_r to find out the tokens 
    *   #5. malloc each index of the array with the length of tokens,
	*			fill command_list array with tokens, and fill last spot with NULL.
	*	#6. return the variable.
	*/
}


void free_command_line(command_line* command)
{
	/*For some reason this free is working for the first time i run through the code, but not the second time. I spent hours trying to figure out why, but I still dont know why it is happening.*/
	for(int i=0; i < command->num_token;i++) {
		free(command->command_list[i]);
	}
	free(command->command_list);
	/*
	*	#1.	free the array base num_token
	*/
}
