#pragma once
#include "string.h"

bool strsame(char* str1, char* str2) {
	return strcmp(str1, str2) == 0;
}

bool str_is_one_of(char* str, char** list, int n) {
	for(int i = 0; i < n; i++) {
		if(strsame(str, list[i])) return true;
	} return false;
}

bool str_starts_with(char* str, char* substr) {
	int str_len = strlen(str);
	int substr_len = strlen(substr);
	if (str_len < substr_len) return false;
	for(int i = 0; i < substr_len; i++) {
		if (str[i] != substr[i]) return false;
	} return true;
}
		

