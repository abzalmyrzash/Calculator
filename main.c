#include "command.h"

#define COMMAND_LENGTH 256

int main()
{
	init_globals();
	char* command = malloc(COMMAND_LENGTH);
	while(true) {
		printf("> ");
		read_command(command, COMMAND_LENGTH);
		process_command(command);
		if (quit) {
			free_globals();
			free(command);
			break;
		}
	}
	
	return 0;
}
