#include "includes.h"

#if DEBUG
	char *HS_VER = ",Ver:H-TL402,S-T0001-190517,";		// 定义硬件软件版本号
#else	
	char *HS_VER = ",Ver:H-TL402,S-0001-190611,";		// 定义硬件软件版本号
#endif

void MX_FREERTOS_Init(void); 
void SystemClock_Config(void);
void Error_Handler(void);

int main(void)
{
	HAL_Init();
	SystemClock_Config();							// 系统时钟配置	
	MX_GPIO_Init();									// GPIO配置
	MX_SPI1_Init();									// 初始化配置SPI1,用于读写外部W2564
	MX_RTC_Init();									// 初始化配置RTC	
	MX_USART1_UART_Init();							// 初始化配置串口1,用于PC通信
	MX_USART2_UART_Init();							// 初始化配置串口2,用于GPS通信
	MX_LPUART1_UART_Init(115200);					// 初始化配置低功耗串口1,用于MCU_F通信
	MX_USART4_UART_Init();							// TIM-WIFI发送数据和定位
													// TIM-各个标志位位域清零
	SMS.w  = 0;					
	CALL.w = 0;
	GPRS.w = 0;
	MCU_F.w = 0;
	GPS_F.w = 0;
	
	MX_FREERTOS_Init();								// TIM-任务创建、UART_RXNE、EXIT
	
	osKernelStart();

	while (1)
	{

	}
}

/** System Clock Configuration
*/
void SystemClock_Config(void)						// 配置系统时钟
{	
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	__HAL_RCC_PWR_CLK_ENABLE();

	__HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_LSI;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.HSIState = RCC_HSI_DIV4;
	RCC_OscInitStruct.HSICalibrationValue = 16;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
	RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	} 

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_LPUART1|RCC_PERIPHCLK_RTC;
	PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
	PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
	PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_SYSCLK;
	PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}
		
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/10);

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
