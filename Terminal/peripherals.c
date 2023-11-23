#include "peripherals.h"
#include "execute.h"
#include <startup.h>
#include <stdio.h>
#include <stdlib.h>

pio_pin leds[] = {
	{ &(PIOC_BASE_ADDRESS->ODR), 6 },
	{ &(PIOC_BASE_ADDRESS->ODR), 7 },
	{ &(PIOC_BASE_ADDRESS->ODR), 8 },
	{ &(PIOC_BASE_ADDRESS->ODR), 9 },
	{ &(PIOE_BASE_ADDRESS->ODR), 4 },
	{ &(PIOD_BASE_ADDRESS->ODR), 3 },
	{ &(PIOE_BASE_ADDRESS->ODR), 5 },
	{ &(PIOE_BASE_ADDRESS->ODR), 6 }
};

pio_pin rgb[] = {
	{ &(PIOD_BASE_ADDRESS->ODR), 13 },
	{ &(PIOB_BASE_ADDRESS->ODR), 8 },
	{ &(PIOD_BASE_ADDRESS->ODR), 12 }
};

pio_pin joystick[] = {
	{ &(PIOE_BASE_ADDRESS->IDR), JOY_RT },
	{ &(PIOE_BASE_ADDRESS->IDR), JOY_LT },
	{ &(PIOE_BASE_ADDRESS->IDR), JOY_DN },
	{ &(PIOE_BASE_ADDRESS->IDR), JOY_UP },
	{ &(PIOE_BASE_ADDRESS->IDR), JOY_OK }
};

uint32_t digit[] = {BIT(DIG_1), BIT(DIG_2), BIT(DIG_3), BIT(DIG_4)};

uint32_t numbers[] = {
		BIT(SEG_A) | BIT(SEG_B) | BIT(SEG_C) | BIT(SEG_D) | BIT(SEG_E) | BIT(SEG_F),
		BIT(SEG_B) | BIT(SEG_C),
		BIT(SEG_A) | BIT(SEG_B) | BIT(SEG_D) | BIT(SEG_E) | BIT(SEG_G),
		BIT(SEG_A) | BIT(SEG_B) | BIT(SEG_C) | BIT(SEG_D) | BIT(SEG_G),
		BIT(SEG_B) | BIT(SEG_C) | BIT(SEG_F) | BIT(SEG_G),
		BIT(SEG_A) | BIT(SEG_C) | BIT(SEG_D) | BIT(SEG_F) | BIT(SEG_G),
		BIT(SEG_A) | BIT(SEG_C) | BIT(SEG_D) | BIT(SEG_E) | BIT(SEG_F) | BIT(SEG_G),
		BIT(SEG_A) | BIT(SEG_B) | BIT(SEG_C),
		BIT(SEG_A) | BIT(SEG_B) | BIT(SEG_C) | BIT(SEG_D) | BIT(SEG_E) | BIT(SEG_F) | BIT(SEG_G),
		BIT(SEG_A) | BIT(SEG_B) | BIT(SEG_C) | BIT(SEG_D) | BIT(SEG_F) | BIT(SEG_G)
};

uint32_t display[NODIGITS] = {0};

void initPeripherals(){
	  HAL_Init();
	  *PWR_CR2 |= IOSV;		//en power supply
	  SystemClock_Config();
	  *RCC_AHB2ENR |= (PIOB_EN | PIOC_EN | PIOD_EN | PIOE_EN | PIOG_EN);	//en clocks
	  InitGPIO();
	  InitSegments();
	  InitDigit();
	  InitTimer();
	  InitLpuart();
	  InitJoystick();
}

void InitGPIO(){
	  GPIO_RESET_PIN(&PIOB_BASE_ADDRESS->ODR, BIT(rgb[1].pin));
	  GPIO_RESET_PIN(&PIOC_BASE_ADDRESS->ODR, BIT(leds[0].pin) | BIT(leds[1].pin) | BIT(leds[2].pin) | BIT(leds[3].pin));
	  GPIO_RESET_PIN(&PIOD_BASE_ADDRESS->ODR, BIT(leds[5].pin) | BIT(rgb[0].pin) | BIT(rgb[2].pin));
	  GPIO_RESET_PIN(&PIOE_BASE_ADDRESS->ODR, BIT(leds[4].pin) | BIT(leds[6].pin) | BIT(leds[7].pin));
	  //OUTPUT MODER 01

	  PIOB_BASE_ADDRESS->MODER &= ~(3 << 2*rgb[1].pin);
	  PIOB_BASE_ADDRESS->MODER |= (1 << 2*rgb[1].pin);
	  PIOC_BASE_ADDRESS->MODER &= ~((3 << 2 * leds[0].pin) | (3 << 2 * leds[1].pin) | (3 << 2 * leds[2].pin) | (3 << 2 * leds[3].pin));
	  PIOC_BASE_ADDRESS->MODER |= ((1 << 2 * leds[0].pin) | (1 << 2 * leds[1].pin) | (1 << 2 * leds[2].pin) | (1 << 2 * leds[3].pin));
	  PIOD_BASE_ADDRESS->MODER &= ~((3 << 2 * leds[5].pin) | (3 << 2 * rgb[2].pin) | (3 << 2 * rgb[0].pin));
	  PIOD_BASE_ADDRESS->MODER |= ((1 << 2 * leds[5].pin) | (1 << 2 * rgb[2].pin) | (1 << 2 * rgb[0].pin));
	  PIOE_BASE_ADDRESS->MODER &= ~((3 << 2 * leds[4].pin) | (3 << 2 * leds[6].pin) | (3 << 2 * leds[7].pin));
	  PIOE_BASE_ADDRESS->MODER |= ((1 << 2 * leds[4].pin) | (1 << 2 * leds[6].pin) | (1 << 2 * leds[7].pin));

	  //PUPD 10
	  PIOB_BASE_ADDRESS->PUPDR &= ~(3 << 2*rgb[1].pin);
	  PIOB_BASE_ADDRESS->PUPDR |= (2 << 2*rgb[1].pin);
	  PIOC_BASE_ADDRESS->PUPDR &= ~((3 << 2 * leds[0].pin) | (3 << 2 * leds[1].pin) | (3 << 2 * leds[2].pin) | (3 << 2 * leds[3].pin));
	  PIOC_BASE_ADDRESS->PUPDR |= ((2 << 2 * leds[0].pin) | (2 << 2 * leds[1].pin) | (2 << 2 * leds[2].pin) | (2 << 2 * leds[3].pin));
	  PIOD_BASE_ADDRESS->PUPDR &= ~((3 << 2 * leds[5].pin) | (3 << 2 * rgb[2].pin) | (3 << 2 * rgb[0].pin));
	  PIOD_BASE_ADDRESS->PUPDR |= ((2 << 2 * leds[5].pin) | (2 << 2 * rgb[2].pin) | (2 << 2 * rgb[0].pin));
	  PIOE_BASE_ADDRESS->PUPDR &= ~((3 << 2 * leds[4].pin) | (3 << 2 * leds[6].pin) | (3 << 2 * leds[7].pin));
	  PIOE_BASE_ADDRESS->PUPDR |= ((2 << 2 * leds[4].pin) | (2 << 2 * leds[6].pin) | (2 << 2 * leds[7].pin));
}

void GPIO_RESET_PIN(volatile uint16_t* port, uint16_t pin){
	*port &= ~pin;
}

void GPIO_SET_PIN(volatile uint16_t* port, uint16_t pin){
	*port |= pin;
}

int GPIO_READ_PIN(volatile uint16_t* port, uint16_t pin){
	if(*port & pin){
		return 1;
	}
	return 0;
}


//-------------DISPLAY-------------
void InitSegments(){
	//HAL_PWREx_EnableVddIO2(); //--this is also fine

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
	GPIO_RESET_PIN(&PIOB_BASE_ADDRESS->ODR, BIT(DIG_1) | BIT(DIG_2) | BIT(DIG_3) | BIT(DIG_4));
	//OUTPUT MODER 01
	PIOB_BASE_ADDRESS->MODER &= ~((3 << DIG_1 * 2) | (3 << DIG_2 * 2) | (3 << DIG_3 * 2) |
            					  (3 << DIG_4 * 2));
	PIOB_BASE_ADDRESS->MODER |= (1 << DIG_1 * 2) | (1 << DIG_2 * 2) | (1 << DIG_3 * 2) | (1 << DIG_4 * 2);
	//PUPDR 00
	PIOB_BASE_ADDRESS->PUPDR &= ~((3 << DIG_1 * 2) | (3 << DIG_2 * 2) | (3 << DIG_3 * 2) |
	            				  (3 << DIG_4 * 2));
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
		GPIO_RESET_PIN(&PIOB_BASE_ADDRESS->ODR, BIT(DIG_1) | BIT(DIG_2) | BIT(DIG_3) | BIT(DIG_4));
		GPIO_RESET_PIN(&PIOG_BASE_ADDRESS->ODR, numbers[8]);
		GPIO_SET_PIN(&PIOG_BASE_ADDRESS->ODR, display[abs(noDisp-3)]);
		GPIO_SET_PIN(&PIOB_BASE_ADDRESS->ODR, digit[noDisp]);
		MyDelay(1);
	}
}


//-------------JOYSTICK-------------
void InitJoystick(){
	//OUTPUT MODER 00
	PIOE_BASE_ADDRESS->MODER &= ~((3<< JOY_RT * 2) | (3<< JOY_LT * 2) | (3<< JOY_DN * 2) | (3<< JOY_UP * 2) | (3<< JOY_OK * 2));
	//PUPDR 00
	PIOE_BASE_ADDRESS->PUPDR &= ~((3<< JOY_RT * 2) | (3<< JOY_LT * 2) | (3<< JOY_DN * 2) | (3<< JOY_UP * 2) | (3<< JOY_OK * 2));
}


//-------------TIMER-------------
void InitTimer(){
    *RCC_APB1ENR1 |= TIM6EN; //enable clock for timer 6
    *RCC_APB1ENR1 |= TIM7EN;
    TIMER_6->PSC = 4000;
    TIMER_7->PSC = 4000;
}

void DelayAndDisplay(int time){
	TIMER_6->ARR = time;
	TIMER_6->CR1 |= CEN;
	while(!(TIMER_6->SR & UIF)){
		updateDisplay();
	}
	TIMER_6->SR &= ~UIF;
	TIMER_6->CR1 &= ~CEN;
}

void MyDelay(int time){
	TIMER_7->ARR = time;
	TIMER_7->CR1 |= CEN;
	while(!(TIMER_7->SR & UIF)){
	}
	TIMER_7->SR &= ~UIF;
	TIMER_7->CR1 &= ~CEN;
}


//-------------LPUART-------------
void InitLpuart(){
	*RCC_APB1ENR2 |= LPUART1_EN; //enable clock for LPUART
	*RCC_AHB2ENR |= PIOC_EN; //enable clock for port G

	//Set MODER to 10, which stands for Alternate Function - has to be for external interface to work (Tx and Rx)
	PIOC_BASE_ADDRESS->MODER |= ((1<<1) | (1<<3));
	PIOC_BASE_ADDRESS->MODER &= ~((1<<0) | (1<<2));
	//We're using RL, bc pins 0 & 1 are below 8??
	PIOC_BASE_ADDRESS->AFRL |= ((1<<7) | (1<<3));
	PIOC_BASE_ADDRESS->AFRL &= ~((1<<0) | (1<<1) | (1<<2) | (1<<4) | (1<<5) | (1<<6));

	LPUART_1->CR1 &= ~(PCE);

	LPUART_1->CR1 &= ~(M0 | M1);

	LPUART_1->BRR = ((256 * 4000000)/115200);
	LPUART_1->CR2 &= ~(STOP);

	LPUART_1->CR1 |= UE; //LPUART enable
	LPUART_1->CR3 &= ~(DMAR | DMAT);
}

void transmitChar(char letter){
	LPUART_1->CR1 |= TE; //enable transmitter
	LPUART_1->TDR = ((uint32_t)letter); //transmit data reg - contains letter to be transmitted

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
		//displayCount();
		updateDisplay();
	}

	char inputChar = LPUART_1->RDR;
	return inputChar;
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
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
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}


#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{

}
#endif
