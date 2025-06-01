#include "calculator.h"
#include "command.h"

#define COMMAND_LENGTH 256

int main()
{
	DynArr* a = DynArr_new(sizeof(int), 1, DynArrIntFunc);
	int x = 1;
	DynArr_append(a, &x);
	DynArr_print(a);
	init_globals();
	while(true) {
		printf("> ");
		char *command = read_command_fgets(COMMAND_LENGTH);
		DynArr* words = split_by_space(command);
		if (words->len == 0) continue;
		DynArr* tokens = split_into_tokens(words->data, words->len);
		if (tokens->len == 0) continue;
		// DynArr_print(tokens);
		process_tokens(tokens->data, tokens->len);
		free(command);
		DynArr_free(words);
		DynArr_free(tokens);
	}
	
	return 0;
}
