#ifndef __COMMAND
#define __COMMAND

#define BUFFER_SIZE 16
typedef void (*Execute)(void);
typedef volatile struct {
	Execute execute;
	char* name;
	char* helpMsg;
} Command;

typedef struct {
	char command[BUFFER_SIZE];
	char argument[BUFFER_SIZE];
} UserInput;

extern UserInput userInput;

#endif
