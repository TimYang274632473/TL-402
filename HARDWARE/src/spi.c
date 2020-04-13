#include "includes.h"

SPI_HandleTypeDef hspi1;

/* SPI1初始化配置	 */
void MX_SPI1_Init(void)
{
	hspi1.Instance					= SPI1;
	hspi1.Init.Mode					= SPI_MODE_MASTER;
	hspi1.Init.Direction			= SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize				= SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity			= SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase				= SPI_PHASE_1EDGE;
	hspi1.Init.NSS					= SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler	= SPI_BAUDRATEPRESCALER_4;
	hspi1.Init.FirstBit				= SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode				= SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation		= SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial		= 10;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		Error_Handler();
	}
}

/* 启动SPI初始化配置	 */
void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	if(spiHandle->Instance==SPI1)
	{
		__HAL_RCC_SPI1_CLK_ENABLE();

		GPIO_InitStruct.Pin			= SCLK_W25_Pin;
		GPIO_InitStruct.Mode		= GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull		= GPIO_NOPULL;
		GPIO_InitStruct.Speed		= GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate	= GPIO_AF0_SPI1;
		HAL_GPIO_Init(SCLK_W25_GPIO_Port, &GPIO_InitStruct);
		
		GPIO_InitStruct.Pin = MISO_W25_Pin;
		HAL_GPIO_Init(MISO_W25_GPIO_Port, &GPIO_InitStruct);
		
		GPIO_InitStruct.Pin = MOSI_W25_Pin;
		HAL_GPIO_Init(MOSI_W25_GPIO_Port, &GPIO_InitStruct);
	}
}

/* 关闭SPI初始化配置	 */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{
	if(spiHandle->Instance==SPI1)
	{
		__HAL_RCC_SPI1_CLK_DISABLE();
		HAL_GPIO_DeInit(SCLK_W25_GPIO_Port, SCLK_W25_Pin);
		HAL_GPIO_DeInit(MISO_W25_GPIO_Port, MISO_W25_Pin);
		HAL_GPIO_DeInit(MOSI_W25_GPIO_Port, MOSI_W25_Pin);
	}
} 
