#pragma once
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

char* str_get_null_terminated(char* str, int len) {
	char* str2 = malloc(len + 1);
	memcpy(str2, str, len);
	str2[len] = '\0';
	return str2;
}

bool strsame(char* str1, char* str2) {
	return strcmp(str1, str2) == 0;
}

int str_find_in_list(char* str, char** list, int n) {
	for(int i = 0; i < n; i++) {
		if (list[i] == NULL) return -1;
		if(strsame(str, list[i])) return i;
	}
	return -1;
}

bool str_is_one_of(char* str, char** list, int n) {
	return str_find_in_list(str, list, n) != -1;
}

bool char_is_one_of(char c, char* str) {
	return strchr(str, c) != NULL;
}

bool str_starts_with(char* str, char* substr) {
	int str_len = strlen(str);
	int substr_len = strlen(substr);
	if (str_len < substr_len) return false;
	for(int i = 0; i < substr_len; i++) {
		if (str[i] != substr[i]) return false;
	} return true;
}

// returns number of subsequent characters that can be part of a name
// name must start with a letter and can be followed by other letters
// or digits or underscores
int str_is_name_len(char* str) {
	int i = 0;
	char c = str[i];
	if(isalpha(c)) {
		c = str[++i];
	} else return 0;
	while(isalpha(c) || isdigit(c) || c == '_') {
		c = str[++i];
	}
	return i;
}

bool str_is_name(char* str) {
	return str_is_name_len(str) == strlen(str);
}

// returns number of subsequent characters that can be part of an int
int str_is_int_len(char* str) {
	int i = 0;
	char c = str[i];
	if (c == '-' && isdigit(str[i+1])) {
		c = str[i+=2];
	}
	while(isdigit(c)) {
		c = str[++i];
	}
	return i;
}

bool str_is_int(char* str) {
	return str_is_int_len(str) == strlen(str);
}

// returns number of subsequent characters that can be part of a number
int str_is_num_len(char* str) {
	int i = 0;
	char c = str[i];
	if (c == '-' && (isdigit(str[i+1]) || str[i+1] == '.')) {
		c = str[i+=2];
	}
	if (isdigit(c)) {
		c = str[++i];
	}
	while(isdigit(c)) {
		c = str[++i];
	}
	if (c == '.' && isdigit(str[i+1])) {
		c = str[++i];
	} else return i;
	while(isdigit(c)) {
		c = str[++i];
	}
	return i;
}

bool str_is_num(char* str) {
	return str_is_num_len(str) == strlen(str);
}

