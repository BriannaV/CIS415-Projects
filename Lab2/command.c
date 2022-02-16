#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "command.h"


void lfcat() {
	FILE *file_out = fopen("output.txt", "w");
	FILE *file_in;

	char *buff = (char*)malloc(sizeof(char)*1024);
	char *pwd = (char*)malloc(sizeof(char)*1024);

	ssize_t line_size = 0;
	size_t buf_size = 1024;
	int count_file = 1;
	DIR *dir;
	struct dirent *dp;

	getcwd(pwd, 1024);
	dir = opendir(pwd);

	while(dir) {
		if((dp = readdir(dir)) != NULL) {
			if(strcmp(dp->d_name, "command.c") && strcmp(dp->d_name,"command.h") && strcmp(dp->d_name, "output.txt") && strcmp(dp->d_name, "lab2.c") && strcmp(dp->d_name, "command.o") && strcmp(dp->d_name, "main.c") && strcmp(dp->d_name, "main.o") && strcmp(dp->d_name, "Makefile") && strcmp(dp->d_name, ".") && strcmp(dp->d_name, "..") && strcmp(dp->d_name, "lab2.exe")) {
				file_in = fopen(dp->d_name, "r");
				fprintf(file_out, "File %d: %s\n", count_file, dp->d_name);
				line_size = getline(&buff, &buf_size, file_in);
				while(line_size >=0) {
					fprintf(file_out, "%s\n", buff);
					line_size = getline(&buff, &buf_size, file_in);
				}
				fclose(file_in);
				fprintf(file_out, "--------------------------------------------------------------------------------");
				count_file++;
			}
			
		}
		else{
			closedir(dir);
			break;
		}
	}
	fclose(file_out);
	free(pwd);
	free(buff);
}
