#include "main.h"
#include "defines.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct {
	uint32_t MODER;
	uint32_t OTYPER;
	uint32_t OSPEEDR;
	uint32_t PUPDR;
	uint16_t IDR;
	uint16_t reserved_1;
	uint16_t ODR;
	uint16_t reserved_2;
} *Init_Port;

#define PIOG_BASE_ADDRESS ((Init_Port)0x48001800)
#define PIOB_BASE_ADDRESS ((Init_Port)0x48000400)
#define PIOE_BASE_ADDRESS ((Init_Port)0x48001000)

void GPIO_SET_PIN(uint16_t* port, uint16_t pin);
void GPIO_RESET_PIN(uint16_t* port, uint16_t pin);
int GPIO_READ_PIN(uint16_t* port, uint16_t pin);

int main(void)
{
	HAL_Init();
    SystemClock_Config();
    InitSegments();
    InitDigit();
    InitJoystick();
    while (1)
    {
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

    }
}

void SystemClock_Config(void)
{
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
}

void InitSegments(){
	HAL_PWREx_EnableVddIO2(); //--this is also fine
	__HAL_RCC_GPIOG_CLK_ENABLE(); //keep it

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
	__HAL_RCC_GPIOB_CLK_ENABLE();
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
	__HAL_RCC_GPIOE_CLK_ENABLE();
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
		HAL_Delay(2);
	}
}

int setCountDirection(){

	if(GPIO_READ_PIN(&PIOE_BASE_ADDRESS->IDR, JOYSTICK_OK) == 0){
		return -1;
	}
	return 1;
}

void GPIO_RESET_PIN(uint16_t* port, uint16_t pin){
	*port &= ~pin;
}

void GPIO_SET_PIN(uint16_t* port, uint16_t pin){
	*port |= pin;
}

int GPIO_READ_PIN(uint16_t* port, uint16_t pin){
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
