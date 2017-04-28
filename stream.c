/*Domenic Bianchi
CIS 2750 Assignment 2
February 19, 2017
This program creates files neccessary to create a stream, add a user to a user file, or remove a user from a user file. Also, this program adds posts to the appropriate stream*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stream.h"
#include "addauthor.h"

void updateStream(userPost * st) {

	char command[7500];
	clearArray(command, 7500);

	strcat(command, "./db -addPost \"");
	strcat(command, st->streamname);
	strcat(command, "\" \"");
	strcat(command, st->username);
	strcat(command, "\" \"");
	strcat(command, st->date);
	strcat(command, "\" \"");
	strcat(command, st->text);
	strcat(command, "\"");

	system(command);
}

void addUser(char * username, char * list, char * statusMessage) {

	char * streamToAdd;

	/*Tokenize all streams that the user is being added to*/
	streamToAdd = strtok(list,",");

	while (streamToAdd != NULL) {

		char command[7500];
		clearArray(command, 7500);

		strcat(command, "./db -addAuthor \"");
		strcat(command, streamToAdd);
		strcat(command, "\" \"");
		strcat(command, username);
		strcat(command, "\"");

		system(command);

		streamToAdd = strtok(NULL, " ,");
	}
}

void removeUser(char * username, char * list) {

	char * streamToRemove;

	streamToRemove = strtok(list,",");

	/*Loop through each stream the user entered*/
	while (streamToRemove != NULL) {

		char command[7500];
		clearArray(command, 7500);

		strcat(command, "./db -removeAuthor \"");
		strcat(command, streamToRemove);
		strcat(command, "\" \"");
		strcat(command, username);
		strcat(command, "\"");

		system(command);

		streamToRemove = strtok(NULL, " ,");
	}
}
