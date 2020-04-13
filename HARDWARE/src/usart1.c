#include "includes.h"												 		

/* 以下是串口1接收变量	*/
char 		USART1_RXD_DATA[USART_RXD_Group_Len][USART_RXD_Line_Len];	// 存放串口接收的数据
uint8_t 	USART1_RXD_Group_Cnt=0;										// 组
uint8_t 	USART1_RXD_Line_Cnt=0;										// 列
#if MODULE_SIM7500
unsigned int Aduio_Cnt = 0;
unsigned int Audio_Length=0;
#endif

/**********************************************************************************************************
*	函 数 名: USART1_DATA
*	功能说明: 中断接收保存数据子程序,仅被串口中断程序调用
*	形    参：8位中断接收数据
*	返 回 值: 无
**********************************************************************************************************/
void USART1_DatHandle(char dat)
{
	if(dat != 0x0A)								// Backspace 对应的函数 0x08
	{					 
		if(dat == 0x08)							// 回删键  \b
		{					 
			if(USART1_RXD_Line_Cnt > 0) 
			{
				USART1_RXD_DATA[USART1_RXD_Group_Cnt][USART1_RXD_Line_Cnt--]='\0';	
				USART1_SendData('\b');
				USART1_SendData(' ');
				USART1_SendData('\b');
			}
			return;
		}
		else if(ECHO_F.b.PC_MCU_Flag==ON)		// 开启串口1直接回显				// 判断是否需要回显
		{
			USART1_SendData(dat);				// 回显字符
		}
		if(dat == 0x0D)							// 回车键, 并且不保存回车字符
		{					      
			if(ECHO_F.b.PC_MCU_Flag==ON)		// 回显换行									
			{
				USART1_SendData('\n');															// 回显	
			}																					
			if(USART1_RXD_DATA[USART1_RXD_Group_Cnt][USART_RXD_Line_Len-1] == '\0')				// 没有未处理的指令
			{
				BaseType_t xHigherPriorityTaskWoken = pdFALSE;
				
				USART1_RXD_DATA[USART1_RXD_Group_Cnt][USART1_RXD_Line_Cnt]='\0';
				USART1_RXD_DATA[USART1_RXD_Group_Cnt][USART_RXD_Line_Len-1] = 1;				// 换行标志位
				if(USART1_RXD_Group_Cnt++ == USART_RXD_Group_Len-1)
				{
					USART1_RXD_Group_Cnt = 0;
				}
				USART1_RXD_Line_Cnt = 0;														// 下一行,第一列
				//如二值信号量被使用，则释放(置空)
				if(Semap_PcCmd != NULL)
				{
					xSemaphoreGiveFromISR(Semap_PcCmd,&xHigherPriorityTaskWoken);
					//如果释放二值信号量后有更高优先级的任务满足则需要进行一次任务切换
					if(xHigherPriorityTaskWoken == pdTRUE)
					{
						portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
					}
				}
			}
		}
		else
		{
			if(USART1_RXD_DATA[USART1_RXD_Group_Cnt][USART_RXD_Line_Len-1] == '\0')				// 没有未处理的指令
			{
				USART1_RXD_DATA[USART1_RXD_Group_Cnt][USART1_RXD_Line_Cnt++] = dat;
				if(USART1_RXD_Line_Cnt == USART_RXD_Line_Len-2 )
				{
					unsigned char i;
					for(i=0;i<USART1_RXD_Line_Cnt-1;i++)
					{
						USART1_RXD_DATA[USART1_RXD_Group_Cnt][i]='\0';
					}
					USART1_RXD_Line_Cnt = 0;
				}
			}
		}
	}
}

#if OTA_F
/* 串口升级		*/

void USART_Update(void)
{		
	SPI_Flash_Wakeup();													// 断开GPRS连接,Flash保持唤醒

	printf("\r\nMCU Is Ready Now!\r\n");								// PC接收到此指令后,可以开始发送数据
	
	MX_UART1_DMA_Init();												// 配置DMA
	
	HAL_UART_Receive_DMA(&huart1,SPI_FLASH_BUF,2048);					// 启动首次DMA接收,库函数中已关闭传送一半中断功能
				
	if(OTA_Store() == false)											// ota存储数据	
	{
		GSM_OTA.OTA_errcount++; 
	}
	
	SPI_Flash_Write(otaaddr, (u8*)&GSM_OTA,GSM_OTA_size);				//保存下载结果
	
	SPI_Flash_Sleep();													// Flash休眠
	
	HAL_UART_DMAStop(&huart1);											// 关闭DMA
									
	NVIC_SystemReset();													// 复位	
}
#endif
