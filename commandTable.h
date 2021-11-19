#define CODE_COMMANDS 27
#define DATA_COMMAND_LENGHT 8
#define DATA_COMMANDS 6

typedef struct codeCommand
{
	char command[30];
	char action;
	int validationCase;
	int funct;
	int opcode;

}codeCommand;

typedef struct dataCommand
{
	char name[DATA_COMMAND_LENGHT];
	int validationCase;
	int bytes;

}dataCommand;

codeCommand codeCommands[CODE_COMMANDS];
dataCommand dataCommands[DATA_COMMANDS];