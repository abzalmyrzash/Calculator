#pragma once
#include "stdio.h"
#include "stdlib.h"
#include "dynarr.h"

#define COMMAND_LENGTH 64

char* read_command() {
	size_t bufsize = COMMAND_LENGTH;
	char* buffer = (char *)malloc(bufsize * sizeof(char));
	if(buffer == NULL) {
		perror("Unable to allocate buffer");
		exit(1);
	}
	ssize_t length = getline(&buffer, &bufsize, stdin);
	if(length == -1) {
		perror("Unable to read line");
		exit(1);
	}
	return buffer;
}

DynArr* split_command(char* cmd, int max_word_number, int max_word_length)
{
	DynArr* words = DynArr_new(sizeof(char*), max_word_number, DynArrStringFunc);
	int i = 0;
	while(cmd[i] == ' ') { i++; }
	if (cmd[i] == '\0') {
		printf("ERROR: command is empty!\n");
		return NULL;
	}
	char *word = malloc(max_word_length+1);
	if(word == NULL) {
		perror("Failed to allocate word");
		exit(1);
	}
	int wordlen = 0;
	for (; ; i++) {
		if(cmd[i] == ' ' || cmd[i] == '\0' || cmd[i] == '\n') {
			*(word+wordlen) = '\0';
			DynArr_append(words, &word);
			while(cmd[i] == ' ') { i++; }
			if (cmd[i] == '\0' || cmd[i] == '\n') break;
			if (words->len == max_word_number) {
				printf("ERROR: too many words!\n");
				DynArr_free(words);
				return NULL;
			}
			word = malloc(max_word_length+1);
			if(word == NULL) {
				perror("Failed to allocate word");
				exit(1);
			}
			wordlen = 0;
		}
		if(wordlen == max_word_length) {
			printf("ERROR: a word is too long!\n");
			free(word);
			DynArr_free(words);
			return NULL;
		}
		*(word + wordlen) = cmd[i];
		wordlen++;
	}
	return words;
}

