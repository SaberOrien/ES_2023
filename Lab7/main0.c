#include "main.h"
#include "defines.h"
#include <stdlib.h>
#include <stdio.h>

#define FIFO_SIZE 15

typedef struct {
	char buffer[FIFO_SIZE];
	unsigned int head;
	unsigned int tail;
	int isEmpty;
} FIFO;

FIFO InitFifo();
int isFull(FIFO* queue);
int FifoPutChar(FIFO* queue, char data);
char FifoGetChar(FIFO* queue);
void FifoGetBuffer(FIFO* queue);

int main(void)
{
	HAL_Init();
    SystemClock_Config();

    InitTimer();
    InitSegments();
    InitDigit();
    InitJoystick();

    InitLpuart();
    FIFO queue = InitFifo();

    while (1)
    {

#if 1
    	char receivedData;
    	while(((receivedData = getChar()) != '\r')){
    		if((!FifoPutChar(&queue, receivedData)) || (isFull(&queue))){
    			break;
    		}

    	}
    	FifoGetBuffer(&queue);

#endif
/*
#if 0
    	//Task 1
    	for(char letter = 'a'; letter <= 'z'; letter++){
    		transmitChar(letter);
    		MyDelay(100);
    	}
    	for(char letter = 'A'; letter <= 'Z'; letter++){
    	    transmitChar(letter);
    		MyDelay(100);
    	}
    	//Task 2
    	transmitString(" A warm welcome message - <you, who's reading it>");

    	//Task 3
    	while(1){
    		char receivedChar = getChar();
    		if (receivedChar >= 'a' && receivedChar <= 'z') {
    	        char transmittedChar = receivedChar - 'a' + 'A';
    	        transmitChar(transmittedChar);
    	    } else if (receivedChar >= 'A' && receivedChar <= 'Z') {
    	    	char transmittedChar = receivedChar - 'A' + 'a';
    		    transmitChar(transmittedChar);
    		} else {
    		    transmitChar(receivedChar);
    		}
    	}
#endif
#if 0
    	int count = 0;
    	int directionCount = 0;
    	int number = 0;
    	while(number <= 9999){
    		separateDigits(number);
    		count = 0;
    		while(count < COUNT_DELAY){
    			displayCount();
    			count++;
    			directionCount = setCountDirection();
    		}
    		number += directionCount;

    		if(number == -1){
    			number = 9999;
    		}
    	}
#endif
*/
    }
}

void SystemClock_Config(void){
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  //HAL_RCC_GetSysClockFreq();
}

FIFO InitFifo(){
	FIFO queue;
	queue.head = 0;
	queue.tail = 0;
	queue.isEmpty = 1;
	return queue;
}

int isFull(FIFO* queue){
	return queue->head == queue->tail && !queue->isEmpty;
}

int FifoPutChar(FIFO* queue, char data){
	if(isFull(queue)){
		transmitString("FIFO full");
		return 0;
	}
	queue->buffer[queue->head] = data;
	queue->head = (queue->head + 1) % FIFO_SIZE;
	queue->isEmpty = 0;
	return 1;
}

char FifoGetChar(FIFO* queue){
	if(queue->isEmpty) {
		return '\0';
	}
	char data = queue->buffer[queue->tail];
	queue->tail = (queue->tail + 1) % FIFO_SIZE;
	queue->isEmpty = queue->head == queue->tail;
	return data;
}

void FifoGetBuffer(FIFO* queue){
	/*while(!queue->isEmpty){
		transmitChar(FifoGetChar(queue));
	}*/
	char receivedData;
	while((receivedData = FifoGetChar(queue)) != '\0'){
		transmitChar(receivedData);
	}
}

void InitLpuart(){
	*RCC_APB1ENR2 |= LPUART1_EN; //enable clock for LPUART
	*RCC_AHB2ENR |= PIOC_EN; //enable clock for port G

	//Set MODER to 10, which stands for Alternate Function - has to be for external interface to work (Tx and Rx)
	PIOC_BASE_ADDRESS->MODER |= ((1<<1) | (1<<3));
	PIOC_BASE_ADDRESS->MODER &= ~((1<<0) | (1<<2));
	//We're using RL, bc pins 0 & 1 are below 8??
	PIOC_BASE_ADDRESS->AFRL |= ((1<<7) | (1<<3));
	PIOC_BASE_ADDRESS->AFRL &= ~((1<<0) | (1<<1) | (1<<2) | (1<<4) | (1<<5) | (1<<6));

	LPUART_1->CR1 &= ~(M0 | M1);

	//256 ascii chars, the second is clock frequency, and third who knows
	//Baudrate no of signal/symbol changes per second. Measures the speed of data transmission. Formula slide 76
	LPUART_1->BRR = ((256 * 4000000)/115200);
	LPUART_1->CR2 &= ~(STOP);

	LPUART_1->CR1 |= UE; //LPUART enable
	LPUART_1->CR3 &= ~(DMAR | DMAT);
}

void transmitChar(char letter){
	LPUART_1->CR1 |= TE; //enable transmitter
	LPUART_1->TDR = ((uint32_t)letter); //transmit data reg - contains letter to be transmitted

	//while(!((LPUART_1->ISR) & TXE)){ //is set to 1 when data is transferred to shift register
	//}
	while(!((LPUART_1->ISR) & TC)){ //is set to 1 when transmission is complete
	}
}

void transmitString(char* string){
	while(*string != '\0'){
		transmitChar(*string);
		string++;
	}
}

char getChar(){
	LPUART_1->CR1 |= RE; //enable receiver
	while(!((LPUART_1->ISR) & RXNE)){ //is set to 0 when data is still being received and set to 1 when it's ready to be read
	}
	char inputChar = LPUART_1->RDR;
	return inputChar;
}

void InitTimer(){
    *RCC_APB1ENR1 |= TIM6EN; //enable clock for timer 6
    *TIM6_PSC = 4000;
}

void InitSegments(){
	HAL_PWREx_EnableVddIO2(); //--this is also fine
	//__HAL_RCC_GPIOG_CLK_ENABLE(); //keep it
	*RCC_AHB2ENR |= (1<<6);
	GPIO_RESET_PIN(&PIOG_BASE_ADDRESS->ODR, numbers[8]);
	//OUTPUT MODER 01
	PIOG_BASE_ADDRESS->MODER &= ~((3 << SEG_A * 2) | (3 << SEG_B * 2) | (3 << SEG_C * 2) |
            					  (3 << SEG_D * 2) | (3 << SEG_E * 2) | (3 << SEG_F * 2) |
								  (3 << SEG_G * 2));
	PIOG_BASE_ADDRESS->MODER |= (1 << SEG_A * 2) | (1 << SEG_B * 2) | (1 << SEG_C * 2) |
            					(1 << SEG_D * 2) | (1 << SEG_E * 2) | (1 << SEG_F * 2) |
								(1 << SEG_G * 2);

	//PUPD 00
	PIOG_BASE_ADDRESS->PUPDR &= ~((3 << SEG_A * 2) | (3 << SEG_B * 2) | (3 << SEG_C * 2) |
			  	  	  	  	  	  (3 << SEG_D * 2) | (3 << SEG_E * 2) | (3 << SEG_F * 2) |
								  (3 << SEG_G * 2));
}

void InitDigit(){
	//__HAL_RCC_GPIOB_CLK_ENABLE();
	*RCC_AHB2ENR |= (1<<1);
	GPIO_RESET_PIN(&PIOB_BASE_ADDRESS->ODR, DIGIT_1 | DIGIT_2 | DIGIT_3 | DIGIT_4);
	//OUTPUT MODER 01
	PIOB_BASE_ADDRESS->MODER &= ~((3 << DIG_1 * 2) | (3 << DIG_2 * 2) | (3 << DIG_3 * 2) |
            					  (3 << DIG_4 * 2));
	PIOB_BASE_ADDRESS->MODER |= (1 << DIG_1 * 2) | (1 << DIG_2 * 2) | (1 << DIG_3 * 2) | (1 << DIG_4 * 2);
	//PUPDR 00
	PIOB_BASE_ADDRESS->PUPDR &= ~((3 << DIG_1 * 2) | (3 << DIG_2 * 2) | (3 << DIG_3 * 2) |
	            				  (3 << DIG_4 * 2));
}

void InitJoystick(){
	//__HAL_RCC_GPIOE_CLK_ENABLE();
	*RCC_AHB2ENR |= (1<<4);
	//OUTPUT MODER 00
	PIOE_BASE_ADDRESS->MODER &= ~(3<< JOY_OK * 2);
	//PUPDR 10
	PIOE_BASE_ADDRESS->PUPDR &= ~(3 << JOY_OK * 2);
	PIOE_BASE_ADDRESS->PUPDR |= (2 << JOY_OK * 2);
}

void separateDigits(int number){
	display[0] = numbers[number % 10];
	number /= 10;
	display[1] = (number > 0) ? numbers[number % 10] : 0;
	number /= 10;
	display[2] = (number > 0) ? numbers[number % 10] : 0;
	number /= 10;
	display[3] = (number > 0) ? numbers[number % 10] : 0;
}

void displayCount(){
	for(int noDisp = 0; noDisp < 4; ++noDisp){
		GPIO_RESET_PIN(&PIOB_BASE_ADDRESS->ODR, DIGIT_1 | DIGIT_2 | DIGIT_3 | DIGIT_4);
		GPIO_RESET_PIN(&PIOG_BASE_ADDRESS->ODR, numbers[8]);
		GPIO_SET_PIN(&PIOG_BASE_ADDRESS->ODR, display[abs(noDisp-3)]);
		GPIO_SET_PIN(&PIOB_BASE_ADDRESS->ODR, digit[noDisp]);
		MyDelay(1);
	}
}

void MyDelay(int time){
	*TIM6_ARR = time;
	*TIM6_CR1 |= CEN;
	while(!(*TIM6_SR & UIF)){
	}
	*TIM6_SR &= ~UIF;
	*TIM6_CR1 &= ~CEN;

}

int setCountDirection(){

	if(GPIO_READ_PIN(&PIOE_BASE_ADDRESS->IDR, JOYSTICK_OK) == 0){
		return -1;
	}
	return 1;
}

void GPIO_RESET_PIN(volatile uint16_t* port, volatile uint16_t pin){
	*port &= ~pin;
}

void GPIO_SET_PIN(volatile uint16_t* port, volatile uint16_t pin){
	*port |= pin;
}

int GPIO_READ_PIN(volatile uint16_t* port, volatile uint16_t pin){
	if(*port & pin){
		return 1;
	}
	return 0;
}

void Error_Handler(void)
{
}

#ifdef  USE_FULL_ASSERT

void assert_failed(char *file, uint32_t line)
{
	while(1);
}
#endif
