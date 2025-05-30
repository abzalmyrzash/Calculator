#include "calculator.h"

#define MAX_WORD_NUMBER 4
#define MAX_WORD_LENGTH 15

int main()
{
	init_globals();
	while(true) {
		printf("> ");
		char *command = read_command();
		DynArr* words = split_command(command, MAX_WORD_NUMBER, MAX_WORD_LENGTH);
		if (words == NULL) continue;
		process_command((char**)words->data, words->len);
		DynArr_free(words);
	}
	
	return 0;
}
