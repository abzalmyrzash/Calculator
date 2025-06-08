#pragma once
#include "stdio.h"
#include "stdlib.h"

// char* read_command_getline() {
// 	size_t bufsize = 64; // getline reallocs anyway if it's bigger
// 	char* buffer = (char *)malloc(bufsize);
// 	if(buffer == NULL) {
// 		perror("Unable to allocate buffer");
// 		exit(1);
// 	}
// 	ssize_t length = getline(&buffer, &bufsize, stdin);
// 	if(length == -1) {
// 		perror("Unable to read line");
// 		exit(1);
// 	}
// 	strtok(buffer, "\n");
// 	return buffer;
// }

char* read_command_fgets(size_t bufsize) {
	char* buffer = (char *)malloc(bufsize);
	if (fgets(buffer, bufsize, stdin) == NULL) {
		perror("Failed to fgets");
	}
	strtok(buffer, "\n");
	return buffer;
}
