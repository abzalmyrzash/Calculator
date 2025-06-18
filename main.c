#include "command.h"

#define COMMAND_LENGTH 256

int main()
{
	printf("%d\n", sizeof(Matrix));
	init_globals();
	while(true) {
		printf("> ");
		char *command = read_command_fgets(COMMAND_LENGTH);
		process_command(command);
		free(command);
	}
	
	return 0;
}
