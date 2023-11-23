#include "main.h"

typedef struct {
	GPIO_TypeDef* Port;
	uint32_t Pin;
} gpio_pin;

#define LED_NO 8
#define LED_DELAY 1000
#define JOYSTICK_DELAY 50

int rgb_state[3] = {0, 0, 0};

#define LED0 GPIO_PIN_6
#define LED1 GPIO_PIN_7
#define LED2 GPIO_PIN_8
#define LED3 GPIO_PIN_9
#define LED4 GPIO_PIN_4
#define LED5 GPIO_PIN_3
#define LED6 GPIO_PIN_5
#define LED7 GPIO_PIN_6

#define LED_R GPIO_PIN_13
#define LED_G GPIO_PIN_8
#define LED_B GPIO_PIN_12

#define JOY_RT GPIO_PIN_0
#define JOY_LT GPIO_PIN_1
#define JOY_DN GPIO_PIN_2
#define JOY_OK GPIO_PIN_15


void SystemClock_Config(void);
void InitLeds(gpio_pin leds[]);
void InitRgb(gpio_pin rgb[]);
void InitJoystick(gpio_pin joystick[]);
void JoystickToRGB(gpio_pin rgb[], gpio_pin joystick[]);

int main(void)
{
	int previousLed = 0;
	gpio_pin leds[8];
	gpio_pin rgb[3];
	gpio_pin joystick[4];

    HAL_Init();
    SystemClock_Config();
    InitLeds(leds);
    InitRgb(rgb);
    InitJoystick(joystick);

    while (1)
    {
    	int joystickTick;
    	previousLed = 0;
    	for(int currentLed = 0; currentLed < LED_NO; ++currentLed){
    		joystickTick = 0;
    		previousLed = currentLed > 0 ? currentLed-1 : 1;
    		HAL_GPIO_WritePin(leds[previousLed].Port, leds[previousLed].Pin, GPIO_PIN_RESET);
    		HAL_GPIO_WritePin(leds[currentLed].Port, leds[currentLed].Pin, GPIO_PIN_SET);
    		while(joystickTick < JOYSTICK_DELAY){
    			JoystickToRGB(rgb, joystick);
        		HAL_Delay(LED_DELAY / JOYSTICK_DELAY);
        		joystickTick++;
    		}
    	}
    	for(int currentLed = 6; currentLed > 0; --currentLed){
    		joystickTick = 0;
    		previousLed = currentLed + 1;
    		HAL_GPIO_WritePin(leds[previousLed].Port, leds[previousLed].Pin, GPIO_PIN_RESET);
    		HAL_GPIO_WritePin(leds[currentLed].Port, leds[currentLed].Pin, GPIO_PIN_SET);
    		while(joystickTick < JOYSTICK_DELAY){
    			JoystickToRGB(rgb, joystick);
        		HAL_Delay(LED_DELAY / JOYSTICK_DELAY);
        		joystickTick++;
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

void InitLeds(gpio_pin leds[])
{
  uint32_t leds_07[] = {LED0, LED1, LED2, LED3, LED4, LED5, LED6, LED7};
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOC, LED0 | LED1 | LED2 | LED3, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = LED0 | LED1 | LED2 | LED3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOD, LED5, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = LED5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOE, LED4 | LED6 | LED7, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = LED4 | LED6 | LED7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  for(int countLed = 0; countLed < LED_NO; ++countLed){
	  leds[countLed].Pin = leds_07[countLed];
	  if(countLed >= 0 && countLed <= 3){
		  leds[countLed].Port = GPIOC;
	  }
	  else if(countLed == 5){
		  leds[countLed].Port = GPIOD;
	  }
	  else{
		  leds[countLed].Port = GPIOE;
	  }
  }

}

void InitRgb(gpio_pin rgb[]){
	  GPIO_InitTypeDef GPIO_InitStruct = {0};

	  __HAL_RCC_GPIOB_CLK_ENABLE();
	  __HAL_RCC_GPIOD_CLK_ENABLE();

	  HAL_GPIO_WritePin(GPIOB, LED_G, GPIO_PIN_RESET);
	  GPIO_InitStruct.Pin = LED_G;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	  HAL_GPIO_WritePin(GPIOD, LED_R | LED_B, GPIO_PIN_RESET);
	  GPIO_InitStruct.Pin = LED_R | LED_B;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	  rgb[0].Pin = LED_R;
	  rgb[0].Port = GPIOD;
	  rgb[1].Pin = LED_G;
	  rgb[1].Port = GPIOB;
	  rgb[2].Pin = LED_B;
	  rgb[2].Port = GPIOD;
}

void InitJoystick(gpio_pin joystick[]){
	  GPIO_InitTypeDef GPIO_InitStruct = {0};

	  __HAL_RCC_GPIOE_CLK_ENABLE();

	  HAL_GPIO_WritePin(GPIOE, JOY_RT | JOY_LT | JOY_DN | JOY_OK, GPIO_PIN_RESET);
	  GPIO_InitStruct.Pin = JOY_RT | JOY_LT | JOY_DN | JOY_OK;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	  joystick[0].Pin = JOY_LT;
	  joystick[0].Port = GPIOE;
	  joystick[1].Pin = JOY_DN;
	  joystick[1].Port = GPIOE;
	  joystick[2].Pin = JOY_RT;
	  joystick[2].Port = GPIOE;
	  joystick[3].Pin = JOY_OK;
	  joystick[3].Port = GPIOE;
}

void JoystickToRGB(gpio_pin rgb[], gpio_pin joystick[]){
	for(int counter = 0; counter < 3; ++counter){
		if(HAL_GPIO_ReadPin(joystick[counter].Port, joystick[counter].Pin) == GPIO_PIN_RESET){
			HAL_GPIO_WritePin(rgb[counter].Port, rgb[counter].Pin, GPIO_PIN_SET);
			rgb_state[counter] = 1;
		}
		else{
			HAL_GPIO_WritePin(rgb[counter].Port, rgb[counter].Pin, GPIO_PIN_RESET);
			rgb_state[counter] = 0;
		}
	}
	if(HAL_GPIO_ReadPin(joystick[3].Port, joystick[3].Pin) == GPIO_PIN_RESET){
		HAL_GPIO_WritePin(rgb[0].Port, rgb[0].Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(rgb[1].Port, rgb[1].Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(rgb[2].Port, rgb[2].Pin, GPIO_PIN_SET);

	}
	else{
		for(int counter = 0; counter < 3; ++counter){
			if(rgb_state[counter] == 0){
				HAL_GPIO_WritePin(rgb[counter].Port, rgb[counter].Pin, GPIO_PIN_RESET);
			}
		}
		//HAL_GPIO_WritePin(rgb[0].Port, rgb[0].Pin, GPIO_PIN_RESET);
		//HAL_GPIO_WritePin(rgb[1].Port, rgb[1].Pin, GPIO_PIN_RESET);
		//HAL_GPIO_WritePin(rgb[2].Port, rgb[2].Pin, GPIO_PIN_RESET);
	}

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
