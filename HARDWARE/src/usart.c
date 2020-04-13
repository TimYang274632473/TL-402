/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

#include "gpio.h"

UART_HandleTypeDef hlpuart1;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart4;
#if MODULE_UG96
DMA_HandleTypeDef hdma_lpuart1_rx;
#endif
DMA_HandleTypeDef hdma_usart1_rx;

/* 低功耗串口1初始化配置 */
void MX_LPUART1_UART_Init(unsigned int Band)
{
	hlpuart1.Init.BaudRate 						= Band;
	hlpuart1.Instance 							= LPUART1;
	hlpuart1.Init.WordLength 					= UART_WORDLENGTH_8B;
	hlpuart1.Init.StopBits 						= UART_STOPBITS_1;
	hlpuart1.Init.Parity 						= UART_PARITY_NONE;
	hlpuart1.Init.Mode 							= UART_MODE_TX_RX;
	hlpuart1.Init.HwFlowCtl 					= UART_HWCONTROL_NONE;
	hlpuart1.Init.OneBitSampling 				= UART_ONE_BIT_SAMPLE_DISABLE;
	hlpuart1.AdvancedInit.AdvFeatureInit		= UART_ADVFEATURE_NO_INIT;
	hlpuart1.AdvancedInit.Swap					= UART_ADVFEATURE_SWAP_DISABLE;

	if (HAL_UART_Init(&hlpuart1) != HAL_OK)
	{
		Error_Handler();
	}
	UART_WakeUpTypeDef Selection;
	Selection.WakeUpEvent = UART_WAKEUP_ON_STARTBIT;
	HAL_UARTEx_StopModeWakeUpSourceConfig(&hlpuart1, Selection );
	HAL_UARTEx_EnableClockStopMode(&hlpuart1);
	HAL_UARTEx_EnableStopMode(&hlpuart1);
}

/* 串口1初始化配置 	*/
void MX_USART1_UART_Init(void)
{	
	huart1.Instance 							= USART1;
	huart1.Init.BaudRate 						= 115200;
	huart1.Init.WordLength 						= UART_WORDLENGTH_8B;
	huart1.Init.StopBits 						= UART_STOPBITS_1;
	huart1.Init.Parity 							= UART_PARITY_NONE;
	huart1.Init.Mode 							= UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl 						= UART_HWCONTROL_NONE;
	huart1.Init.OverSampling 					= UART_OVERSAMPLING_16;
	huart1.Init.OneBitSampling 					= UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.AdvancedInit.AdvFeatureInit 			= UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
}

#if MODULE_UG96

/* 开启LPUART1 DMA */
void MX_LPUART1_DMA_Init(void)
{	
	__HAL_RCC_DMA1_CLK_ENABLE();

	hdma_lpuart1_rx.Instance 					= DMA1_Channel3;
	hdma_lpuart1_rx.Init.Request 				= DMA_REQUEST_5;
	hdma_lpuart1_rx.Init.Direction 				= DMA_PERIPH_TO_MEMORY;
	hdma_lpuart1_rx.Init.PeriphInc 				= DMA_PINC_DISABLE;
	hdma_lpuart1_rx.Init.MemInc 				= DMA_MINC_ENABLE;
	hdma_lpuart1_rx.Init.PeriphDataAlignment	= DMA_PDATAALIGN_BYTE;
	hdma_lpuart1_rx.Init.MemDataAlignment 		= DMA_MDATAALIGN_BYTE;
	hdma_lpuart1_rx.Init.Mode					= DMA_NORMAL;
	hdma_lpuart1_rx.Init.Priority 				= DMA_PRIORITY_VERY_HIGH;
	if (HAL_DMA_Init(&hdma_lpuart1_rx) != HAL_OK)
	{
		Error_Handler();
	}
}

#endif

/* 开启UART1 DMA */
void MX_UART1_DMA_Init(void)
{
	__HAL_RCC_DMA1_CLK_ENABLE();

	hdma_usart1_rx.Instance 					= DMA1_Channel5;
	hdma_usart1_rx.Init.Request 				= DMA_REQUEST_3;
	hdma_usart1_rx.Init.Direction 				= DMA_PERIPH_TO_MEMORY;
	hdma_usart1_rx.Init.PeriphInc 				= DMA_PINC_DISABLE;
	hdma_usart1_rx.Init.MemInc 					= DMA_MINC_ENABLE;
	hdma_usart1_rx.Init.PeriphDataAlignment	 	= DMA_PDATAALIGN_BYTE;
	hdma_usart1_rx.Init.MemDataAlignment 		= DMA_MDATAALIGN_BYTE;
	hdma_usart1_rx.Init.Mode 					= DMA_NORMAL;
	hdma_usart1_rx.Init.Priority 				= DMA_PRIORITY_VERY_HIGH;

	if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
	{
		Error_Handler();
	}

    __HAL_LINKDMA(&huart1,hdmarx,hdma_usart1_rx);
	
	HAL_NVIC_SetPriority(DMA1_Channel4_5_6_7_IRQn, 7, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel4_5_6_7_IRQn);
}

/* 串口2初始化配置 	*/
void MX_USART2_UART_Init(void)
{
	huart2.Instance 							= USART2;
	huart2.Init.BaudRate 						= 9600;
	huart2.Init.WordLength 						= UART_WORDLENGTH_8B;
	huart2.Init.StopBits 						= UART_STOPBITS_1;
	huart2.Init.Parity 							= UART_PARITY_NONE;
	huart2.Init.Mode 							= UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl 						= UART_HWCONTROL_NONE;
	huart2.Init.OverSampling 					= UART_OVERSAMPLING_16;
	huart2.Init.OneBitSampling 					= UART_ONE_BIT_SAMPLE_DISABLE;
	huart2.AdvancedInit.AdvFeatureInit 			= UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
}

/*	串口4初始化配置	*/
void MX_USART4_UART_Init(void)
{
	huart4.Instance								= USART4;
	huart4.Init.BaudRate						= 115200;
	huart4.Init.WordLength						= UART_WORDLENGTH_8B;
	huart4.Init.StopBits						= UART_STOPBITS_1;
	huart4.Init.Parity							= UART_PARITY_NONE;
	huart4.Init.Mode							= UART_MODE_TX_RX;
	huart4.Init.HwFlowCtl						= UART_HWCONTROL_NONE;
	huart4.Init.OverSampling					= UART_OVERSAMPLING_16;
	huart4.Init.OneBitSampling					= UART_ONE_BIT_SAMPLE_DISABLE;
	huart4.AdvancedInit.AdvFeatureInit			= UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart4) != HAL_OK)
	{
		Error_Handler();
	}
}

/* 启动串口初始化配置	*/
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	if(uartHandle->Instance==LPUART1)
	{
		__HAL_RCC_LPUART1_CLK_ENABLE();
		
		GPIO_InitStruct.Pin			= TX_GSM_Pin;
		GPIO_InitStruct.Mode		= GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull		= GPIO_NOPULL;
		GPIO_InitStruct.Speed		= GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate	= GPIO_AF4_LPUART1;
		HAL_GPIO_Init(TX_GSM_GPIO_Port, &GPIO_InitStruct);
		
		GPIO_InitStruct.Pin = RX_GSM_Pin;
		HAL_GPIO_Init(RX_GSM_GPIO_Port, &GPIO_InitStruct);
			
		HAL_NVIC_SetPriority(AES_RNG_LPUART1_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(AES_RNG_LPUART1_IRQn);
	}
	else if(uartHandle->Instance==USART1)
	{
		__HAL_RCC_USART1_CLK_ENABLE();
		
		GPIO_InitStruct.Pin			= TX_PC_Pin;
		GPIO_InitStruct.Mode		= GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull		= GPIO_NOPULL;
		GPIO_InitStruct.Speed		= GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate	= GPIO_AF4_USART1;
		HAL_GPIO_Init(TX_PC_GPIO_Port, &GPIO_InitStruct);
		
		GPIO_InitStruct.Pin = RX_PC_Pin;
		HAL_GPIO_Init(RX_PC_GPIO_Port, &GPIO_InitStruct);
		
		HAL_NVIC_SetPriority(USART1_IRQn, 7, 0);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
	}
	else if(uartHandle->Instance==USART2)
	{
		__HAL_RCC_USART2_CLK_ENABLE();

		GPIO_InitStruct.Pin			= RX_GPS_Pin;
		GPIO_InitStruct.Mode		= GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull		= GPIO_NOPULL;
		GPIO_InitStruct.Speed		= GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate	= GPIO_AF4_USART2;
		HAL_GPIO_Init(RX_GPS_GPIO_Port, &GPIO_InitStruct);
		
		GPIO_InitStruct.Pin = TX_GPS_Pin;
		HAL_GPIO_Init(TX_GPS_Port, &GPIO_InitStruct);
		
		HAL_NVIC_SetPriority(USART2_IRQn, 7, 0);
//		HAL_NVIC_EnableIRQ(USART2_IRQn);
	}
	else if(uartHandle->Instance==USART4)
	{
		__HAL_RCC_USART4_CLK_ENABLE();
	
		GPIO_InitStruct.Pin			= GPIO_PIN_0|GPIO_PIN_1;
		GPIO_InitStruct.Mode		= GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull		= GPIO_PULLUP;
		GPIO_InitStruct.Speed		= GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate	= GPIO_AF6_USART4;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		HAL_NVIC_SetPriority(USART4_5_IRQn, 7, 0);
		HAL_NVIC_EnableIRQ(USART4_5_IRQn);
	}
}

/* 关闭串口初始化配置	*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{
	if(uartHandle->Instance==LPUART1)
	{
		__HAL_RCC_LPUART1_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOC, TX_GSM_Pin|RX_GSM_Pin);
		HAL_NVIC_DisableIRQ(AES_RNG_LPUART1_IRQn);
	}
	else if(uartHandle->Instance==USART1)
	{
		__HAL_RCC_USART1_CLK_DISABLE();
		HAL_GPIO_DeInit(TX_PC_GPIO_Port, TX_PC_Pin);
		HAL_GPIO_DeInit(RX_PC_GPIO_Port, RX_PC_Pin);
		HAL_NVIC_DisableIRQ(USART1_IRQn);
	}
	else if(uartHandle->Instance==USART2)
	{
		__HAL_RCC_USART2_CLK_DISABLE();		
		HAL_GPIO_DeInit(TX_GPS_Port,		TX_GPS_Pin);
		HAL_GPIO_DeInit(RX_GPS_GPIO_Port,	RX_GPS_Pin);
		HAL_NVIC_DisableIRQ(USART2_IRQn);
	}
	else if(uartHandle->Instance==USART4)
	{
		__HAL_RCC_USART4_CLK_DISABLE();
		HAL_GPIO_DeInit(TX_WIFI_GPIO_Port,	TX_WIFI_Pin);
		HAL_GPIO_DeInit(RX_WIFI_GPIO_Port,	RX_WIFI_Pin);
		HAL_NVIC_DisableIRQ(USART4_5_IRQn);
	}
} 
