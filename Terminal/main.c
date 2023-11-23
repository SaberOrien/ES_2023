#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "execute.h"
#include "input.h"
#include "peripherals.h"

UserInput userInput;

void runTerminal();

void clearUserInput();
int readToBuffer(char* buffer, int* bufferLength, char* prevBuffer, int* prevBufferLength);
void getUserInput(char* currentLevel);
void processCommand(Command** commands, unsigned int noOfCommands, char* currentLevel);

void ledExecute();
void dispExecute();
void joystickExecute();
void dgbuExecute();

void printHelp(Command** commands, unsigned int noOfCommands);
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

	Command disp = {
			.execute = &dispExecute,
			.name = "disp",
			.helpMsg = "display or read from 7 segment display\r\n"
	};

	Command joystick = {
			.execute = &joystickExecute,
			.name = "joystick",
			.helpMsg = "reads state of joystick [ok, rt, lt, dn, up]\r\n"
	};

	Command dgbu = {
			.execute = &dgbuExecute,
			.name = "dgbu",
			.helpMsg = "details about LPUART\r\n"
	};

	Command* commands[] = {&led, &disp, &joystick, &dgbu};
	while(1){
		processCommand(commands, 4, "> ");
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
	processCommand(commands, 6, "LED> LED \t");
}

void dispExecute(){
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
	processCommand(commands, 3, "DISP> DISP \t");
}

void joystickExecute(){
	Command readJoystick = {
			.execute = &exeReadButton,
			.helpMsg = "returns the current state of the joystick\r\n",
			.name = "readjoystick"
	};

	Command pullupEn = {
			.execute = &exePullupEn,
			.helpMsg = "enables pullup on port E\r\n",
			.name = "pullupen"
	};

	Command pullupDis = {
			.execute = &exePullupDis,
			.helpMsg = "disable pullup on port E\r\n",
			.name = "pullupdis"
	};

	Command goUp = {
			.execute = &runTerminal,
			.helpMsg = "returns one level up\r\n",
			.name = "up"
	};

	Command* commands[] = {&readJoystick, &pullupEn, &pullupDis, &goUp};
	processCommand(commands, 4, "JOYSTICK> JOYSTICK \t");
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
	processCommand(commands, 2, "DGBU> DGBU \t");
}


void processCommand(Command** commands, unsigned int noOfCommands, char* currentLevel){
	while(1){
		getUserInput(currentLevel);
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
				if(((strcmp(cmd->name, "up") == 0) || (strcmp(currentLevel, "> ") == 0)) && (strlen(userInput.argument) != 0)){
					continue;
				}
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

void getUserInput(char* currentLevel){
    char* buffers[] = {userInput.command, userInput.argument};
    int bufferLengths[] = {0, 0};
    int currentBufferId = 0;
    do{
        clearUserInput();
        transmitString("\r\n");
        transmitString(currentLevel);
        while(currentBufferId < 2){
            char* buffer = buffers[currentBufferId];
            int input = readToBuffer(buffer, &bufferLengths[currentBufferId], currentBufferId > 0 ? buffers[currentBufferId-1] : NULL, &bufferLengths[currentBufferId-1]);
            if(input == 1 && currentBufferId == 0) {
                currentBufferId++;
                continue;
            }
            if(input == -1){
                transmitString("\r\n");
                transmitString("Buffer overflow");
                break;
            }
            if(input == -2){
                currentBufferId--;
                bufferLengths[1] = 0; // Clear the second buffer
                buffers[1][0] = '\0';
                continue;
            }
            return;
        }
    } while(currentBufferId == 0);
}

int readToBuffer(char* buffer, int* bufferLength, char* prevBuffer, int* prevBufferLength){
    int charId = *bufferLength;
    for(; charId < BUFFER_SIZE; charId++){
        char c = getChar();
        if(c == ' ' && buffer == userInput.command){
        	//transmitString("\r\nc == ' ' && buffer == userInput.command\t");
            transmitChar(c);
            buffer[charId] = '\0';
            *bufferLength = charId;
            return 1;
        }
        else if(c == '\r' || c == '\n'){
        	//transmitString("\r\nENTER");
            buffer[charId] = '\0';
            *bufferLength = charId;
            return 0;
        }
        else if(c == '\b' || c == 0x7F){ // Backspace or delete key
            if(charId > 0){
            	//transmitString("\r\nBACKSPACE");
                transmitString("\b \b"); // Delete character on terminal
                charId -= 2; // Go back one position in buffer
            } else if(prevBuffer != NULL && *prevBufferLength > 0){
            	//transmitString("\r\nMoving to previous buffer");
                transmitString("\b \b"); // Delete character on terminal
                return -2;
            } else {
            	transmitString("\r\nBeginning of line");
                charId--; // If at beginning of line, do not go back further
            }
        }
        else{
        	//transmitString("\r\nNormalchar");
            transmitChar(c);
            c = tolower(c);
            buffer[charId] = c;
        }
    }
    buffer[charId < BUFFER_SIZE ? charId : BUFFER_SIZE - 1] = '\0';
    *bufferLength = charId;
    return charId >= BUFFER_SIZE ? -1 : 0;
}



void clearUserInput() {
	memset(userInput.command, 0, sizeof(userInput.command));
	memset(userInput.argument, 0, sizeof(userInput.argument));
}

void printHelp(Command** commands, unsigned int noOfCommands){
	transmitString("\r\nAvailable commands: \r\n");
	//printFormatedHelp("help", "shows what to type at this level\r\n");
	for(int i = 0; i < noOfCommands; i++){
		Command* cmd = commands[i];
		transmitString("\t- ");
		transmitString(cmd->name);
		transmitString(": ");
		transmitString(cmd->helpMsg);
	}
}

void printCommandNotFound(Command** commands, unsigned int noOfCommands){
	transmitString("\r\nThe command '");
	transmitString(userInput.command);
	transmitString(" ");
	transmitString(userInput.argument);
	transmitString("' is not recognised.\r\n");
	printHelp(commands, noOfCommands);

}
