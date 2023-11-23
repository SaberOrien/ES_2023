#include "main.h"

#define NODIGITS 4
#define DIG1 GPIO_PIN_2
#define DIG2 GPIO_PIN_3
#define DIG3 GPIO_PIN_4
#define DIG4 GPIO_PIN_5
#define DIG_PORT GPIOB

#define SEG_A GPIO_PIN_0
#define SEG_B GPIO_PIN_1
#define SEG_C GPIO_PIN_2
#define SEG_D GPIO_PIN_3
#define SEG_E GPIO_PIN_4
#define SEG_F GPIO_PIN_5
#define SEG_G GPIO_PIN_6
#define SEG_PORT GPIOG

enum DigitPosition {LEFT, CENTRAL_LEFT, CENTRAL_RIGHT, RIGHT};

uint32_t digit[] = {DIG1, DIG2, DIG3, DIG4};

uint32_t numbers[] = {
		SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,
		SEG_B | SEG_C,
		SEG_A | SEG_B | SEG_D | SEG_E | SEG_G,
		SEG_A | SEG_B | SEG_C | SEG_D | SEG_G,
		SEG_B | SEG_C | SEG_F | SEG_G,
		SEG_A | SEG_C | SEG_D | SEG_F | SEG_G,
		SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,
		SEG_A | SEG_B | SEG_C,
		SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,
		SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G
};

void SystemClock_Config(void);
void InitSegments();
void InitDigit(int choosenDigit);

int main(void)
{
	HAL_Init();
    SystemClock_Config();
    InitSegments();
    InitDigit(RIGHT);
    while (1)
    {
    	for(int number = 0; number < 10; ++number){
    		HAL_GPIO_WritePin(SEG_PORT, numbers[number], GPIO_PIN_SET);
    		HAL_Delay(1000);
    		HAL_GPIO_WritePin(SEG_PORT, numbers[number], GPIO_PIN_RESET);
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
	HAL_PWREx_EnableVddIO2();
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOG_CLK_ENABLE();

	HAL_GPIO_WritePin(SEG_PORT, SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(SEG_PORT, &GPIO_InitStruct);
}

void InitDigit(int choosenDigit){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOB_CLK_ENABLE();

	for(int number = 0; number < 4; ++number){
		if(number == choosenDigit){
			HAL_GPIO_WritePin(DIG_PORT, digit[number], GPIO_PIN_SET);
		}
		else{
			HAL_GPIO_WritePin(DIG_PORT, digit[number], GPIO_PIN_RESET);
		}
	}

	GPIO_InitStruct.Pin = DIG1 | DIG2 | DIG3 | DIG4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(DIG_PORT, &GPIO_InitStruct);
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
