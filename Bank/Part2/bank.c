#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>

#include "string_parser.h"
#include "account.h"

#define MAX_THREAD 10

void* balance_update();
void* transactions();
void print_array();

account* array;
command_line* tok_array;
int account_total;

// from lab example
pthread_t tid[MAX_THREAD];
pthread_t simple_thread;


void print_array() {
	/* this function prints the balances of each of the files */
	for(int i=0;i<account_total;i++) {
		printf("%d balance: %0.2f\n", i, array[i].balance);
	}
}


void* reward_update() {
	/* this function updates the balance from the accounts using
	 * the transaction tracker */
	for(int i=0;i<account_total;i++) {
		array[i].balance += array[i].reward_rate * array[i].transaction_tracter;
	}
}


void* transactions(void* argument) {
	/* this function reads all of the transactions in the file and
	 * runs them */
	command_line* tok_array = (command_line*)(argument);
	
	// for each of the things in their command list
	for(int z = 0;z<12000;z++) {
		// if the token is check then check account
		if(strcmp(tok_array[z].command_list[0], "C") == 0) {
			for(int i=0;i<account_total;i++) {
				if((strcmp(tok_array[z].command_list[1], array[i].account_number) == 0)) {
					if(strcmp(tok_array[z].command_list[2], array[i].account_number) == 0) {
						break;
					}
				}
			}
		}

		// if the token is transfer then transfer money
		else if(strcmp(tok_array[z].command_list[0], "T") == 0) {
			double money = atof(tok_array[z].command_list[4]);
			for(int i=0;i<account_total;i++) {
				if((strcmp(tok_array[z].command_list[1], array[i].account_number) == 0)) {
					if(strcmp(tok_array[z].command_list[2], array[i].password) == 0) {
						for(int a=0;a<account_total;a++) {
							if(strcmp(tok_array[z].command_list[3], array[a].account_number) == 0) {
								pthread_mutex_lock(&array[i].ac_lock);
								array[i].balance -= money;
								array[i].transaction_tracter += money;
								pthread_mutex_unlock(&array[i].ac_lock);
								pthread_mutex_lock(&array[a].ac_lock);
								array[a].balance += money;
								pthread_mutex_unlock(&array[a].ac_lock);
								break;
							}
						}
					}
				}
			}
		}

		// if the token is deposit then deposit money
		else if(strcmp(tok_array[z].command_list[0], "D") == 0) {
			for(int i=0;i<account_total;i++) {
				if((strcmp(tok_array[z].command_list[1], array[i].account_number) == 0)) {
					if(strcmp(tok_array[z].command_list[2], array[i].password) == 0) {
						double money = atof(tok_array[z].command_list[3]);
						pthread_mutex_lock(&array[i].ac_lock);
						array[i].transaction_tracter += money;
						array[i].balance += money;
						pthread_mutex_unlock(&array[i].ac_lock);
						break;
					}
				}
			}
		}

		// if the token is withdraw then withdraw money
		if(strcmp(tok_array[z].command_list[0], "W") == 0) {
			for(int i=0;i<account_total;i++) {
				if((strcmp(tok_array[z].command_list[1], array[i].account_number) == 0)) {
					if(strcmp(tok_array[z].command_list[2], array[i].password) == 0) {
						double money = atof(tok_array[z].command_list[3]);
						pthread_mutex_lock(&array[i].ac_lock);
						array[i].transaction_tracter += money;
						array[i].balance -= money;
						pthread_mutex_unlock(&array[i].ac_lock);
						break;
					}
				}
			}
		}
	}
}


int main(int argc, char** argv) {
	if(argc != 2) {
		printf("ERROR: No file specified\n");
	}
	else {
		// checks the file and checks to see if its open
		FILE* f;
		f = fopen(argv[1], "r");
		if(f == NULL) {
			printf("ERROR: File cant be opened\n");
		}
		else {
			char* buf;
			size_t bufsiz = 128;

			buf = (char*)malloc(bufsiz);

			// this gets the total # of accounts
			getline(&buf, &bufsiz, f);
			account_total = atoi(buf);
			
			// begining to make the array of accounts
			// using malloc and then a for loop
			array = malloc(sizeof(account)*account_total);

			for(int i=0; i< account_total; i++) {
				getline(&buf, &bufsiz, f);
				fscanf(f, "%s", array[i].account_number);
				fscanf(f, "%s", array[i].password);
				fscanf(f, "%lf", &array[i].balance);
				fscanf(f, "%lf", &array[i].reward_rate);
				getline(&buf, &bufsiz, f);
				
				array[i].transaction_tracter = 0;
				pthread_mutex_init(&array[i].ac_lock, NULL);
			}
			
			tok_array = malloc(sizeof(command_line)*120000);

			int z = 0;
			int error;
			// gets the list of commands from the code
			while((getline(&buf, &bufsiz, f)) != -1) {
				tok_array[z] = str_filler(buf, " ");
				z ++;
			}

			// this is the code that sets the threads to the
			// right number of commands (got this from using
			// similar example shown in a lab)
			for(int i=0;i<account_total;i++) {
				int val = i*(z/10);
				error = pthread_create(&(tid[i]), NULL, transactions, (void*) (tok_array + val));
				if(error != 0) {
					printf("ERROR: Thread wasnt made\n");
				}
			}

			// merges the threads
			for(int i=0;i<account_total;i++) {
				pthread_join(tid[i], NULL);
			}

			// this creates the thread to make reward
			// update
			error = pthread_create(&simple_thread, NULL, reward_update, NULL);
			if(error != 0) {
				printf("ERROR: Thread wasnt made\n");
			}
			pthread_join(simple_thread, NULL);

			// prints the array of banks
			print_array();
			// close and free things
			//free(array);
			free(buf);
			fclose(f);
		}
	}
}


// these functions came from the string_parser.h from the lab

int count_token (char* buf, const char* delim) {
	/* this function counts the number of tokens in a buf */
	int token_val;
	char*copy = malloc(strlen(buf)+1);
	strcpy(copy, buf);
	if(copy != NULL || copy != delim) {
		if(&copy[0] == delim) {
			memmove(copy, copy+1, strlen(copy));
		}
		if(&copy[strlen(copy)-1] != delim) {
			copy[strlen(copy) -1] = 0;
		}
		token_val = 0;
		char*token;
		while(token = strtok_r(copy, delim, &copy)) {
			token_val = token_val +1;
		}
	}
	else {
		token_val = 0;
	}
	return token_val+1;
}


command_line str_filler(char* buf, const char* delim) {
	/* this completes the array of stringed tokens in command_line[] */
	command_line* command;
	command = malloc(sizeof(*command));
	char*save = malloc(strlen(buf)+1);
	strcpy(save, buf);
	command->num_token = count_token(save, delim)-1;
	command->command_list = (char**)malloc(sizeof(char*) * command->num_token+1);
	char*token;
	char*copy = malloc(strlen(buf)+1);
	strcpy(copy, buf);
	int i=0;
	while((token = strtok_r(copy, delim, &copy))) {
		if(token != NULL) {
			command->command_list[i] = token;
			i += 1;
		}
	}
	return *command;
}


void free_command_line(command_line* command) {
	/* this frees the command line array */
	for(int i=0;i<command->num_token;i++) {
		free(command->command_list[i]);
	}
	free(command->command_list);
}



















