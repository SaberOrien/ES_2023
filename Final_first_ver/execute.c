#include "execute.h"
#include "peripherals.h"
#include "command.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

pio_pin choosenLed;
pio_pin chosenButton;

typedef volatile struct {
	long number;
	int isOn;
} Display;
Display segmentDisplay = {.number = 0, .isOn = 0};

void exeSetLed(){
	setLed(ON);
}

void exeClearLed(){
	setLed(OFF);
}

void exeBlinkLed(){
	for(int i = 0; i < 5; i++){
		exeSetLed();
		HAL_Delay(500);
		exeClearLed();
		HAL_Delay(500);
	}
}

void exeStatusLed(){
	if(chooseLed()){
		transmitString("\r\nLed ");
		transmitString(userInput.argument);
		transmitString(" is ");
		if(GPIO_READ_PIN(choosenLed.port, BIT(choosenLed.pin))) {
			transmitString("ON\r\n");
		}else {
			transmitString("OFF\r\n");
		}
	}
}

void exeInvertLed(){
	if(chooseLed()){
		if(GPIO_READ_PIN(choosenLed.port, BIT(choosenLed.pin))){
			WritePin(choosenLed.port, BIT(choosenLed.pin), OFF);
		}
		else{
			WritePin(choosenLed.port, BIT(choosenLed.pin), ON);
		}
	}
	else{
		transmitString("No such led");
	}
}

void setLed(updateLed state){
	if(chooseLed()){
		WritePin(choosenLed.port, BIT(choosenLed.pin), state);
	}

}

int chooseLed(){
	char* argument = userInput.argument;
	if (isdigit((int)argument[0]) && argument[0] >= '0' && argument[0] <= '7') {
		int index = userInput.argument[0] - '0'; // Convert char to int
		choosenLed = leds[index];
		return 1;
	}
	else if (argument[0] == 'r' || argument[0] == 'g' || argument[0] == 'b') {
		int index = 0;
		switch(argument[0]) {
			case 'r':
				index = 0;
		        break;
		    case 'g':
		        index = 1;
		        break;
		    case 'b':
		        index = 2;
		        break;
		    default:
		        transmitString("\r\nInvalid RGB led identifier.\r\n");
		        return 0;
		    }
		choosenLed = rgb[index];
		return 1;
	} else {
		transmitString("\r\nInvalid led identifier.\r\n");
		return 0;
	}
}


void exeDispNum(){
	userInput.argument[4] = '\0';
	long number = atol(userInput.argument);
	segmentDisplay.number = number;
	segmentDisplay.isOn = 1;
	separateDigits(number);
	doAsyncWork = &updateDisplay;
}

void updateDisplay(){
	if(segmentDisplay.isOn){
		displayCount();
	}
}

void exeDispRead(){
	if(segmentDisplay.isOn){
		transmitString("Currently displayed: ");
		char number[5] = {0};
		sprintf(number, "%ld", segmentDisplay.number);
		transmitString(number);
		transmitString("\r\n");
	}
	else{
		transmitString("Sorry, but the display is off");
	}
}

void exeReadButton(){
	//transmitString("I'll get you a status of a joystick_ok");
	if(chooseButton()){
		transmitString("\r\nJoystick ");
		transmitString(userInput.argument);
		if(GPIO_READ_PIN(chosenButton.port, BIT(chosenButton.pin)) == 0){
			transmitString(" is pressed");
		}
		else{
			transmitString(" is not pressed");
		}
	}
}

int chooseButton() {
    if (strcmp(userInput.argument, "rt") == 0) {
    	transmitString("\r\nYou chose right\r\n");
        chosenButton = joystick[0];
    }
    else if (strcmp(userInput.argument, "lt") == 0) {
    	transmitString("\r\nYou chose left\r\n");
        chosenButton = joystick[1];
    }
    else if (strcmp(userInput.argument, "dn") == 0) {
    	transmitString("\r\nYou chose down\r\n");
        chosenButton = joystick[2];
    }
    else if (strcmp(userInput.argument, "up") == 0) {
    	transmitString("\r\nYou chose up\r\n");
        chosenButton = joystick[3];
    }
    else if (strcmp(userInput.argument, "ok") == 0) {
    	transmitString("\r\nYou chose middle\r\n");
        chosenButton = joystick[4];
    }
    else {
    	transmitString("\r\nNo such button\r\n");
    	return 0;
    }

    return 1;
}

void exePullupEn(){
	if(chooseButton()){
		updatePUPDR(1);
	}
}

void exePullupDis(){
	if(chooseButton()){
		updatePUPDR(0);
	}
}

void updatePUPDR(int enabled){
	if(enabled){
		//PUPDR 01
		PIOE_BASE_ADDRESS->PUPDR &= ~(3 << (2 * __builtin_ctz(chosenButton.pin)));
		PIOE_BASE_ADDRESS->PUPDR |= (1 << (2 * __builtin_ctz(chosenButton.pin)));
		//PIOE_BASE_ADDRESS->PUPDR &= ~(3 << (2 * chosenButton.pin));
		//PIOE_BASE_ADDRESS->PUPDR |= (1 << (2 * chosenButton.pin));
	}
	else{
		//PUPDR 00
		//PIOE_BASE_ADDRESS->PUPDR &= ~(3 << (2 * chosenButton.pin));
		PIOE_BASE_ADDRESS->PUPDR |= (2 << (2 * __builtin_ctz(chosenButton.pin)));
	}
}

void exeDeviceStatus(){
	//Display baudrate, number of databits and parity bits,
	//informations read from registers and then calculated baudrate
    // Display baudrate
    char baudrate[50];
    sprintf(baudrate, "\r\nBaudrate: %d\r\n", ((256 * 4000000)/115200));
    transmitString(baudrate);

    // Display number of databits
    char databits[50];
    int M0_status = (LPUART_1->CR1 & M0) > 0;
    int M1_status = (LPUART_1->CR1 & M1) > 0;
    if(!M0_status && !M1_status) {
        sprintf(databits, "Number of databits: 8\r\n");
    } else if (!M0_status && M1_status) {
        sprintf(databits, "Number of databits: 9\r\n");
    } else if (M0_status && !M1_status) {
        sprintf(databits, "Number of databits: 7\r\n");
    } else {
        sprintf(databits, "Number of databits: Error\r\n");
    }
    transmitString(databits);

    // Display stop bits
    char stopbits[50];
    if (LPUART_1->CR2 & STOP) {
        sprintf(stopbits, "Number of stop bits: 2\r\n");
    } else {
        sprintf(stopbits, "Number of stop bits: 1\r\n");
    }
    transmitString(stopbits);

    // Display parity bits
    char parity[50];
    if (LPUART_1->CR1 & PCE) {
        sprintf(parity, "Parity: Enabled\r\n");
    } else {
        sprintf(parity, "Parity: Disabled\r\n");
    }
    transmitString(parity);

}
