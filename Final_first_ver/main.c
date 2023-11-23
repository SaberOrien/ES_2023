#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "execute.h"
#include "peripherals.h"
#include "command.h"

UserInput userInput;
typedef enum {Ok, Overflow, InputFinished} InputConfirm;

void runTerminal();
void ledExecute();
void sevenLedExecute();
void buttonExecute();
void dgbuExecute();
void handleUserInput(Command** commands, unsigned int noOfCommands, char* lineFix);
void getUserInput(char* prefix);
InputConfirm readToBuffer(int bufferSize, char* buffer);
void printHelp(Command** commands, unsigned int noOfCommands);
void printFormatedHelp(char* cmdName, char* hint);
void printCommandNotFound(Command** commands, unsigned int noOfCommands);

int main(void)
{
  while (1)
  {
	  initPeripherals();
	  transmitString("A simple menu by Izabela Andrzejczyk\r\n");
	  transmitString("Write ‘help’ to obtain more information\r\n");
	  runTerminal();
  }
}

void runTerminal(){
	Command led={
			.execute = &ledExecute,
			.name = "led",
			.helpMsg = "turn On, Off, blink, toggle, get state of LED\r\n"
	};

	Command _7led = {
			.execute = &sevenLedExecute,
			.name = "7led",
			.helpMsg = "display or read from 7 segment display\r\n"
	};

	Command button = {
			.execute = &buttonExecute,
			.name = "button",
			.helpMsg = "all about button\r\n"
	};

	Command dgbu = {
			.execute = &dgbuExecute,
			.name = "dgbu",
			.helpMsg = "details about LPUART\r\n"
	};

	Command* commands[] = {&led, &_7led, &button, &dgbu};
	while(1){
		handleUserInput(commands, 4, "> ");
	}
}

void ledExecute(){
	Command setLed = {
			.execute = &exeSetLed,
			.helpMsg = "Turn on LED\r\n",
			.name = "setled"
	};

	Command clearLed = {
			.execute = &exeClearLed,
			.helpMsg = "Turn off LED\r\n",
			.name = "clearled"
	};

	Command blinkLed = {
			.execute = &exeBlinkLed,
			.helpMsg = "Blink LED 5 times\r\n",
			.name = "blinkled"
	};

	Command statusLed = {
			.execute = &exeStatusLed,
			.helpMsg = "Returns status of led\r\n",
			.name = "statusled"
	};

	Command invertLed = {
			.execute = &exeInvertLed,
			.helpMsg = "Inverts the led: as if ON => OFF\r\n",
			.name = "invertled"
	};

	Command goUp = {
			.execute = &runTerminal,
			.helpMsg = "returns one level up\r\n",
			.name = "up"
	};

	Command* commands[] = {&setLed, &clearLed, &blinkLed, &statusLed, &invertLed, &goUp};
	handleUserInput(commands, 6, "LED> LED \t");
}

void sevenLedExecute(){
	Command dispNum = {
			.execute = &exeDispNum,
			.helpMsg = "Displays number on 7 segment display\r\n",
			.name = "dispnum"
	};

	Command dispRead = {
			.execute = &exeDispRead,
			.helpMsg = "Gets number currently displayed\r\n",
			.name = "dispread"
	};

	Command goUp = {
			.execute = &runTerminal,
			.helpMsg = "returns one level up\r\n",
			.name = "up"
	};

	Command* commands[] = {&dispNum, &goUp, &dispRead};
	handleUserInput(commands, 3, "7LED> 7LED \t");
}

void buttonExecute(){
	Command readButton = {
			.execute = &exeReadButton,
			.helpMsg = "returns the current state of the button\r\n",
			.name = "readbutton"
	};

	Command pullupEn = {
			.execute = &exePullupEn,
			.helpMsg = "enables pullup on port E",
			.name = "pullupen"
	};

	Command pullupDis = {
			.execute = &exePullupDis,
			.helpMsg = "disable pullup on port E",
			.name = "pullupdis"
	};

	Command goUp = {
			.execute = &runTerminal,
			.helpMsg = "returns one level up\r\n",
			.name = "up"
	};

	Command* commands[] = {&readButton, &goUp, &pullupEn, &pullupDis};
	handleUserInput(commands, 4, "BUTTON> BUTTON \t");
}

void dgbuExecute(){
	Command deviceStatus = {
			.execute = &exeDeviceStatus,
			.helpMsg = "returns configuarion parameters for LPUART\r\n",
			.name = "status"
	};

	Command goUp = {
			.execute = &runTerminal,
			.helpMsg = "returns one level up\r\n",
			.name = "up"
	};

	Command* commands[] = {&deviceStatus, &goUp};
	handleUserInput(commands, 2, "DGBU> DGBU \t");
}

void handleUserInput(Command** commands, unsigned int noOfCommands, char* lineFix){
	while(1){
		getUserInput(lineFix);
		char* input = userInput.command;
		if(strcmp(input, "help") == 0){
			printHelp(commands, noOfCommands);
			continue;
		}

		int commandNotFound = 1;
		for(int i = 0; i < noOfCommands; i++){
			Command* cmd = commands[i];
			int selected = strcmp(cmd->name, input) == 0;
			if(selected){
				cmd->execute();
				commandNotFound = 0;
				break;
			}
		}
		if(commandNotFound){
			printCommandNotFound(commands, noOfCommands);
		}
	}
}

void getUserInput(char* prefix){
	char incorrectInput = 1;
	char* buffers[] = {userInput.command, userInput.argument};

	while(incorrectInput){
		incorrectInput = 0;
		transmitString("\r\n");
		transmitString(prefix);

		for(int id = 0; id < 2; id++){
			char* buffer = buffers[id];
			InputConfirm input = readToBuffer(BUFFER_SIZE, buffer);
			if(input == Ok && id == 0) continue;
			//if(input != InputFinished) flush();
			if(input == Overflow){
				transmitString("\r\n");
				transmitString("Buffer overflow");
				incorrectInput = 1;
				break;
			}
			return;
		}
	}
}

InputConfirm readToBuffer(int bufferSize, char* buffer){
	for(int charId = 0; charId < bufferSize; charId++){
		char c = getChar();
		//c = tolower(c);
		switch(c){
			case ' ':
				transmitChar(c);
				buffer[charId] = '\0';
				return Ok;
			case '\r':
			case '\n':
				buffer[charId] = '\0';
				return InputFinished;
			default:
				transmitChar(c);
				c = tolower(c);
				buffer[charId] = c;
		}
	}
	buffer[bufferSize - 1] = '\0';
	return Overflow;
}

void printHelp(Command** commands, unsigned int noOfCommands){
	transmitString("\r\nAvailable commands: \r\n");
	printFormatedHelp("help", "shows what to type at this level\r\n");
	//printFormatedHelp("up", "moves up one level in menu\r\n");
	for(int i = 0; i < noOfCommands; i++){
		Command* cmd = commands[i];
		printFormatedHelp(cmd->name, cmd->helpMsg);
	}
}

void printFormatedHelp(char* cmdName, char* hint){
	transmitString("\t- ");
	transmitString(cmdName);
	transmitString(": ");
	transmitString(hint);
}

void printCommandNotFound(Command** commands, unsigned int noOfCommands){
	transmitString("\r\nThe command '");
	transmitString(userInput.command);
	transmitString(" ");
	transmitString(userInput.argument);
	transmitString("' is not recognised.\r\n");
	printHelp(commands, noOfCommands);

}
