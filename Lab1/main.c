#include "main.h"

#define TIMER 150

struct LED {
	uint16_t pin;
	GPIO_TypeDef* port;
	uint16_t shift;
};

struct LED LEDS[8];

void SystemClock_Config(void);
//static void MX_GPIO_Init(void);
void InitLEDs(void);
void moveLEDs();
void assignLEDs();

int main(void)
{
	HAL_Init();
	SystemClock_Config();
	InitLEDs();
	while(1){
		moveLEDs();
	}
}

void moveLEDs(){
	for(int N = 0; N < 8; N++){
		HAL_GPIO_WritePin(LEDS[N].port, LEDS[N].shift, GPIO_PIN_SET);
		HAL_Delay(TIMER);
		HAL_GPIO_WritePin(LEDS[N].port, LEDS[N].shift, GPIO_PIN_RESET);
	}
	for(int N = 6; N > 0; N--){
		HAL_GPIO_WritePin(LEDS[N].port, LEDS[N].shift, GPIO_PIN_SET);
		HAL_Delay(TIMER);
		HAL_GPIO_WritePin(LEDS[N].port, LEDS[N].shift, GPIO_PIN_RESET);
	}
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }


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
}

void InitLEDs(){
	assignLEDs();
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();

	for(int pinNo = 0; pinNo < 8; pinNo++){
		GPIO_InitStruct.Pin = LEDS[pinNo].pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(LEDS[pinNo].port, &GPIO_InitStruct);
	}
}

void assignLEDs(){
	uint16_t pins[] = {GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_4, GPIO_PIN_3, GPIO_PIN_5, GPIO_PIN_6};
	GPIO_TypeDef* ports[] = {GPIOC, GPIOC, GPIOC, GPIOC, GPIOE, GPIOD, GPIOE, GPIOE};
	uint16_t shifts[] = {6, 7, 8, 9, 4, 3, 5, 6};

	for(int pinNo = 0; pinNo < 8; pinNo++){
		LEDS[pinNo].pin = pins[pinNo];
		LEDS[pinNo].port = ports[pinNo];
		LEDS[pinNo].shift = 1<<shifts[pinNo];
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
