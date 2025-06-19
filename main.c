#include "command.h"

#define COMMAND_LENGTH 256

int main()
{
	init_globals();
	while(true) {
		printf("> ");
		char *command = read_command(COMMAND_LENGTH);
		process_command(command);
		free(command);
		if (quit) {
			free_globals();
			break;
		}
	}
	
	return 0;
}
