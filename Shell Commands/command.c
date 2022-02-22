#include "command.h"
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>


void listDir() {
	/* This function prints out the things inside of a directory.
	 * It is used for the ls command.*/
	DIR* current_dir;
	char cwd[255];
	struct dirent* dir;
	char* error = "cwd error";
	
	// if the cwd doesnt equal null then open the directory
	if(getcwd(cwd, sizeof(cwd)) != NULL) {
		current_dir = opendir(cwd);
		// while the directory still has things in it write them out
		while((dir = readdir(current_dir)) != NULL) {
			write(1, dir->d_name, strlen(dir->d_name));
			write(1, " ", 1);
		}
	}
	// else write out an error
	else {
		write(1, error, strlen(error));
		write(1, "\n", 1);
	}
	// close the directory after done
	closedir(current_dir);
	write(1, "\n", 1);
}


void showCurrentDir() {
	/* This function shows the current directory. This is used for the
	 * pwd command.*/
	char cwd[255];
	// if the current directory isnt null then write what the current
	// directory is
	if(getcwd(cwd, sizeof(cwd)) != NULL) {
		write(1, cwd, strlen(cwd));
		write(1, "\n", 1);
	}
}


void makeDir(char* dirName) {
	/* This function makes a new directory. This is used for the mkdir
	 * command.*/
	int len = strlen(dirName);
	char new_dir[len];
	memset(new_dir, '\0', sizeof(char)* len);
	
	// for loops through as many times as length of directory name given
	for(int i=0;i<len;i++) {
		// if that specific thing in directory name is not an endline
		// put that into a new directory name
		if(strcmp(&dirName[i], "\n") != 0) {
			strncat(new_dir, &dirName[i], 1);
		}
	}
	// make a new directory based on new_dir
	mkdir(new_dir, 0777);
}


void changeDir(char* dirName) {
	/* This function makes changes the directory we are in. This is used
	 * for the cd command.*/
	int len = strlen(dirName);
	char new_dir[len];
	memset(new_dir, '\0', sizeof(char)* len);
	char *error = "Error changing directory";
	
	// for loops through as many items as length of directory name given
	for(int i=0; i<len; i++) {
		// if that specific thing in directory name is not an
		// endline then put that into a new directory name
		if(strcmp(&dirName[i], "\n") != 0) {
			strncat(new_dir, &dirName[i], 1);
		}
	}
	// if the change directory doesnt work then write an error
	if(chdir(new_dir) != 0) {
		write(1, error, strlen(error));
		write(1, "\n", 1);
	}
}


void copyFile(char* sourcePath, char* destinationPath) {
	/* This function copies a file. This is used for the cp command.*/
	
	// new destination
	int dest_len = strlen(destinationPath);
	char new_dest[dest_len];
	memset(new_dest, '\0', sizeof(char)*dest_len);
	// new source
	int source_len = strlen(sourcePath);
	char new_source[source_len];
	memset(new_source, '\0', sizeof(char)* source_len);
	// final destination
	char final_dest[source_len + dest_len];
	memset(final_dest, '\0', sizeof(char)* (source_len + dest_len));

	// for loops through the length of source path
	for(int i=0;i<source_len;i++) {
		// if that specific place in source path isnt an endline
		// then put that in new source
		if(strcmp(&sourcePath[i], "\n") != 0) {
			strncat(new_source, &sourcePath[i], 1);
		}
	}

	// for loops through the length of destination path
	for(int i=0;i<dest_len;i++) {
		// if that specific place in destination path isnt an endline
		// then put that in new destination
		if(strcmp(&destinationPath[i], "\n") != 0) {
			strncat(new_dest, &destinationPath[i], 1);
		}
	}

	ssize_t chars;
	char* buf = malloc(sizeof(char)*255);
	char* cwd = malloc(sizeof(char)*255);
	int s = open(new_source, O_RDONLY);
	getcwd(cwd, 255);

	char* dest_name;
	char* name;

	// if the new destination isnt a dot
	if (strcmp(new_dest, ".") != 0) {
		// set new to a tokenized version of new destination
		char* new = strtok(new_dest, "/");
		// while new doesnt equal null, set name to new and set
		// new to tokenized version
		while(new != NULL) {
			name = new;
			new = strtok(NULL, "/");
		}
	}
	// else
	else {
		// set new to a tokenized version of new source
		char* new = strtok(new_source, "/");
		// while new doesnt equal null, set name to new and set
		// new to tokenized version
		while(new != NULL) {
			name = new;
			new = strtok(NULL, "/");
		}
	}
	// change directory to final destination
	chdir(final_dest);
	
	// open the name file
	int dest = open(name, O_CREAT);
	chmod(name, 0770);
	close(dest);
	// read source
	chars = read(s, buf, 255);
	// open name file write only
	dest = open(name, O_WRONLY);
	// while chars is more than zero write to the file
	while(chars > 0) {
		write(dest, buf, chars);
		chars = read(s, buf, 255);
	}
	
	// free and close needed things
	free(buf);
	free(cwd);
	close(dest);
	close(s);
}


void moveFile(char* sourcePath, char* destinationPath) {
	/* This function moves a file from one place to another. This is used
	 * with the mv command.*/
	
	// This uses copy file to put the file in another place. It also
	// unlinks the file from original place
	copyFile(sourcePath, destinationPath);
	unlink(sourcePath);
}


void deleteFile(char* filename) {
	/* This function deletes a file. This is used with the rm command.*/
	int len = strlen(filename);
	char error[] = "Error deleting file\n";
	char done[] = "Deleted file: ";	
	char new_filename[len];
	memset(new_filename, '\0', sizeof(char)* len);
	int file;

	// for loops through the length of the filename
	for(int i=0;i<len;i++) {
		// if the filename location is not an endline then put 
		// that into the new filename
		if(strcmp(&filename[i], "\n") != 0) {
			strncat(new_filename, &filename[i], 1);
		}
	}
	// if the unlink doesnt work then write out an error
	if(unlink(new_filename) != 0) {
		write(1, error, strlen(error));
	}
	// else write that it worked
	else {
		write(1, done, strlen(done));
		write(1, new_filename, strlen(new_filename));
		write(1, "\n", 1);
	}
}


void displayFile(char* filename) {
	/* This function displays a file on the screen. This is used for the
	 * cat command.*/
	ssize_t numChar;
	int filename_len = strlen(filename) +1;
	char buf[255];
	char new_filename[filename_len];
	char* error = "Error opening file: ";

	memset(new_filename, '\0', sizeof(char)* filename_len);
	memset(buf, '\0', sizeof(char)* 255);

	// for loops through as many times as filename length plus one
	for(int i=0;i< filename_len;i++) {
		// if the filename location isnt an endline then
		// put that into the new filename
		if(strcmp(&filename[i], "\n") != 0) {
			strncat(new_filename, &filename[i], 1);
		}
	}
	// open the file
	int catFile = open(new_filename, O_RDONLY);
	
	// if it doesnt open then print out an error
	if(catFile == -1) {
		write(1, error, strlen(error));
		write(1, new_filename, strlen(new_filename));
		write(1, "\n", 1);
	}
	// else read the file and write out each line
	else {
		numChar = read(catFile, buf, 255);
		while(numChar != 0) {
			write(1, buf, numChar);
			numChar = read(catFile, buf, 255);
		}
		// close the file
		close(catFile);
	}
	write(1, "\n", 1);
}

