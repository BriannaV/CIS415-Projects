#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/stat.h>

#include "string_parser.h"
#include "account.h"

#define MAX_THREAD 10

void* balance_update();
void* transactions();
void print_array();
void update_files();

account* array;
command_line* tok_array;
int account_total;

// from lab example
pthread_t tid[MAX_THREAD];
pthread_t simple_thread;
pthread_barrier_t barrier;

pthread_mutex_t set_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t set_condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

int wait_thread = 0;
int act_thread = 0;
int fin_thread = 0;
int value = 0;


/*void update_files() {
	* this prints the things to the files *
	FILE* f;
	for(int i=0; i<account_total; i++) {
		char* filename;
		sprintf(filename, "%s.txt", array[i].account_number);
		f = fopen(filename, "w");
		fprintf(f, "Current balance: %lf\n", array[i].balance);
		fclose(f);
	}
}*/


void print_array() {
	/* this function prints the balances of each of the files */
	for(int i=0;i<account_total;i++) {
		printf("%d balance: %0.2f\n", i, array[i].balance);
	}
}


void* reward_update() {
	/* this function updates the balance from the accounts using
	 * the transaction tracker */
	while((fin_thread > 0 ) ||  (act_thread > 0)) {
		pthread_cond_wait(&set_condition, &set_lock);
		pthread_mutex_unlock(&set_lock);
		
		pthread_mutex_lock(&lock);
		for(int i=0;i<account_total;i++) {
			array[i].balance += array[i].reward_rate * array[i].transaction_tracter;
	
			array[i].transaction_tracter = 0;
		}
		wait_thread = 0;
		fin_thread = 0;

		if(act_thread > 0) {
			pthread_cond_broadcast(&condition);
		}

		pthread_mutex_lock(&set_lock);
		pthread_mutex_unlock(&lock);
	}
	pthread_exit(NULL);
}


void* transactions(void* argument) {
	/* this function reads all of the transactions in the file and
	 * runs them */
	command_line* tok_array = (command_line*)(argument);

	// this is the threshold value from the project description
	int threshold = value/MAX_THREAD;

	// pthread_self() -> returns the ID of the thread thats
	// currently calling
	int current_thread = pthread_self();
	
	pthread_barrier_wait(&barrier);

	// for each of the things in their command list
	for(int z = 0;z<threshold;z++) {
		// if it got to the moment to reprint the balance
		if(fin_thread >= 5000) {
			pthread_mutex_lock(&lock);
			wait_thread++;
			if(act_thread == wait_thread) {
				pthread_mutex_lock(&set_lock);
				pthread_cond_signal(&set_condition);
				// update_files();
				pthread_mutex_lock(&set_lock);
			}
			pthread_cond_wait(&condition, &lock);
			pthread_mutex_unlock(&lock);
		}
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
								pthread_mutex_lock(&lock);
								fin_thread++;
								pthread_mutex_unlock(&lock);
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
						pthread_mutex_lock(&lock);
						fin_thread++;
						pthread_mutex_unlock(&lock);
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
						pthread_mutex_lock(&lock);
						fin_thread++;
						pthread_mutex_unlock(&lock);
						break;
					}
				}
			}
		}
	}
	act_thread -= 1;

	if(act_thread == wait_thread) {
		pthread_mutex_lock(&set_lock);
		pthread_cond_signal(&set_condition);
		pthread_mutex_lock(&set_lock);
	}
	pthread_exit(NULL);
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
			
			// this makes the directory for the output files
			// mkdir() -> makes the directory
			// 0777 -> default permission, widest access
			// possible given by this one
			// (learned from php.net)
			char*directory_name;
			directory_name = "Output";
			int make = mkdir(directory_name, 0777);
			char out[64];

			pthread_barrier_init(&barrier, NULL, (MAX_THREAD+1));

			for(int i=0; i< account_total; i++) {
				getline(&buf, &bufsiz, f);
				fscanf(f, "%s", array[i].account_number);
				fscanf(f, "%s", array[i].password);
				fscanf(f, "%lf", &array[i].balance);
				fscanf(f, "%lf", &array[i].reward_rate);
				getline(&buf, &bufsiz, f);
				
				array[i].transaction_tracter = 0;
				pthread_mutex_init(&array[i].ac_lock, NULL);

				// this code makes the files
				sprintf(out, "%s.txt", array[i].account_number);
				strcpy(array[i].out_file, out);
				strcpy(out, "");
				FILE*tempf;
				// this writes the first line as the
				// account index that is shown in the
				// example output in the project description
				tempf = fopen(array[i].out_file, "w");
				fprintf(tempf, "account: %d:\n", i);
				fclose(tempf);

			}
			
			tok_array = malloc(sizeof(command_line)*120000);
			pthread_barrier_init(&barrier, NULL, (MAX_THREAD+1));

			int error;
			// gets the list of commands from the code
			while((getline(&buf, &bufsiz, f)) != -1) {
				tok_array[value] = str_filler(buf, " ");
				value ++;
			}

			// this is the code that sets the threads to the
			// right number of commands (got this from using
			// similar example shown in a lab)
			for(int i=0;i<account_total;i++) {
				int val = i*(value/10);
				error = pthread_create(&(tid[i]), NULL, transactions, (void*) (tok_array + val));
				if(error != 0) {
					printf("ERROR: Thread wasnt made\n");
				}
			}

			// set active threads to max because the
			// thread creation worked
			act_thread = MAX_THREAD;

			// fixed lock locked and barrier wait used
			pthread_mutex_lock(&set_lock);
			pthread_barrier_wait(&barrier);

			// makes thread for reward update, i moved
			// it above the for below because i need
			// it to run while others are running
			error = pthread_create(&simple_thread, NULL, &reward_update, NULL);
			if(error !=0) {
				printf("ERROR: Thread wasnt made\n");
			}


			// merges the threads
			for(int i=0;i<account_total;i++) {
				pthread_join(tid[i], NULL);
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



















