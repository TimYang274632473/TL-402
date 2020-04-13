#include <includes.h>
#include <core_cmFunc.h>  

GSM_OTA_def GSM_OTA;

/*
BIT7: 1: LOAD更新新代码成功										由LOAD置1, APP清除
BIT6: 1: APP下载新代码成功											由APP置1,  APP清除
BIT5: 1: 需要OTA升级，但电量不足，等待充电			由APP置1,  APP清除
BIT4: 1: APP升级成功后需要刷新参数区默认设置		由APP置1,  APP清除
BIT3: 1: GPRS通知从服务器下载新代码
BIT2: 1: 短信通知从服务器下载新代码
BIT1: 1: 串口通知从本地电脑下载新代码
BIT0: 1: 未格式化过
*/

bool OTA_Connect(void);
bool Http_Init(void);
bool Http_DownLoad(void);

#if OTA_F

void OTA(void)
{			
	SPI_Flash_Wakeup();								

#if MODULE_SIM7500
	
	OTA_Connect();																					
	
	SPI_Flash_Write(otaaddr, (u8*)&GSM_OTA,GSM_OTA_size);				//保存下载结果
	
	SPI_Flash_Sleep();	
	
#else 
	
	MX_LPUART1_DMA_Init();												

	if(OTA_Connect())													// 连接上HTTP服务器成功
	{ 			
		__HAL_UART_DISABLE_IT(&hlpuart1, UART_IT_RXNE);							
		 __HAL_LINKDMA(&hlpuart1,hdmarx,hdma_lpuart1_rx);				// 启动DMA
		HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 7, 0);
		HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);	
					
		HAL_UART_Receive_DMA(&hlpuart1,SPI_FLASH_BUF,2048);				
			
		OTA_Store();
	}	

	SPI_Flash_Write(otaaddr, (u8*)&GSM_OTA,GSM_OTA_size);				//保存下载结果
	
	SPI_Flash_Sleep();													
	
	HAL_UART_DMAStop(&hlpuart1);										// 关闭DMA
								
	__HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_RXNE);						

#endif
	
	send_str_to_gsm("+++");vTaskDelay(1000);	
	
#if  MODULE_SIM7500		
	Http_Disconnect();														
#endif								

	GSM_Sleep();

	NVIC_SystemReset();	
}	

void Readly_For_OTA(unsigned char usart)
{
	GPRS.b.OTA_NOW_Flag = true;											// 正在OTA
	
	GPS_Force_Off();
	
	if(!(GSM_OTA.OTA_state & 0x0C))										// 进入OTA从服务器下载程序
	{
		NVIC_DisableIRQ(LPUART1_IRQn);
		__HAL_UART_DISABLE_IT(&hlpuart1, UART_IT_RXNE);					
	}
	
	NVIC_DisableIRQ(USART1_IRQn);
	NVIC_DisableIRQ(USART2_IRQn);
	NVIC_DisableIRQ(USART4_5_IRQn);
	
	HAL_NVIC_DisableIRQ(EXTI0_1_IRQn);
	HAL_NVIC_DisableIRQ(EXTI2_3_IRQn);
	HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);
	
	__HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);						// 关闭串口中断
	__HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);						// 关闭串口中断		
	__HAL_UART_DISABLE_IT(&huart4, UART_IT_RXNE);						// 关闭串口中断	
}

//OTA连接
bool OTA_Connect(void)
{
	u8 b;	
		
	for(b=0;b<3;b++)
	{
#if  MODULE_SIM7500
		Http_Disconnect();					
#endif
		if(Http_Init())						
		{		
			if(Http_DownLoad())				
			{
				return 1;
			}
		}
	}
	
	GSM_OTA.OTA_errcount++; 
	
	return 0;
}

/* OTA存储数据	*/
bool OTA_Store(void)
{
	EventBits_t Event_OTA;
	
	unsigned int i=0;
	
	GPRS.b.OTA_Buf_Flag = false;
	
	xEventGroupClearBits(EventGroupHandler_OTH,Gprs_OTA_DMA_Flag);
	
	xEventGroupWaitBits(EventGroupHandler_OTH, Gprs_OTA_DMA_Flag, pdFALSE,pdFALSE,10000);
		
	while(1)
	{			
		Event_OTA = xEventGroupWaitBits(EventGroupHandler_OTH, Gprs_OTA_DMA_Flag, pdFALSE,pdFALSE,2000);
		
		if(Event_OTA & Gprs_OTA_DMA_Flag)											// DMA接收到数据
		{									
			xEventGroupClearBits(EventGroupHandler_OTH,Gprs_OTA_DMA_Flag);
			
			if(GPRS.b.OTA_Buf_Flag == true)											// 如果接收到第一数组数据
			{
				SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,SPI_StartAddr_New+i,2048);
			}
			else																	// 如果接收到第二数组数据
			{
				SPI_Flash_Write_NoCheck(SPI_FLASH_BUF+2048,SPI_StartAddr_New+i,2048);
			}
			
			i+=2048;										
			
			if(i > 0x1B000) 
			{		
				return false;
			}
		}
		else
		{
			if(GPRS.b.OTA_Buf_Flag == true)											// 如果接收到第二数组数据
			{
				SPI_Flash_Write_NoCheck(SPI_FLASH_BUF+2048,SPI_StartAddr_New+i,2048);
			}
			else																	// 如果接收到第一数组数据
			{
				SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,SPI_StartAddr_New+i,2048);
			}
					
			if(i < 0x2000)															// 下载长度小于8K,则认为下载失败
			{
				return false;
			}
			else
			{
				GSM_OTA.OTA_state = GSM_OTA.OTA_state | 0x50;						// 下载成功,需要刷新参数区	
				return true;
			}						
		}
	}	
}

#endif

#if  MODULE_UG96

/* Http初始化 */
bool Http_Init(void)
{	
	if(GSM_AT_Send("+QHTTPCFG=\"contextid\",1",6,2,1,0x0001) == false)	{return false;	}
	
	vTaskDelay(500);
		
	if(GPRS_APN_Connect() == false)		{return false;	}
	
	vTaskDelay(500);
	
	if(QIACT_P() == false) 				{return false;	}
		
	vTaskDelay(500);
		
	return true;
}

/* Http启动下载 */
bool Http_DownLoad(void)
{
	snprintf(dtbuf,55,"+QHTTPURL=%d,30,",48+strlen(GSM_OTA.program_ver));
	if(GSM_AT_Send(dtbuf,11,5,1,0x0400) == false)			{return 0;	}
	snprintf(dtbuf,55,"http://www.toplovo.com/download/client/TL402%s.bin",GSM_OTA.program_ver);
	
	clr_gsm_buf();gsm_rx_req = 6;

	MCU_F.b.Chk_AT_Cnt_Flag = true;
	
	send_str_to_gsm(dtbuf);send_str_to_gsm("\r\n");
	
	xEventGroupWaitBits(EventGroupHandler_OTH,AT_Cnt_OK_Flag, pdFALSE, pdTRUE,5000);
	xEventGroupClearBits(EventGroupHandler_OTH,AT_Cnt_OK_Flag);
	
	MCU_F.b.Chk_AT_Cnt_Flag = false;
	
	if(!find_str("OK"))	{return 0;		}
	
	if(GSM_AT_Send("+QHTTPGET=80",20,80,1,0x0010) == false) {return 0;	}
	
	vTaskDelay(500);
		
	cnt_1s.WTD_Cnt=0;
	
#if WATCHDOG_ON
	HAL_IWDG_Refresh(&hiwdg);											
#endif
		
	sprintf(dtbuf,"AT+QHTTPREAD=80");
	
	clr_gsm_buf();
	
	send_str_to_gsm(dtbuf);send_str_to_gsm("\r\n");
	
	while((gsm_rx_sta < 11)&&(cnt_1s.WTD_Cnt < 10));
	
	if(cnt_1s.WTD_Cnt >= 10)	{return 0;	}
	
	else 						{return 1;	}
}


#elif MODULE_SIM7500

void Http_Disconnect(void)
{
	GSM_AT_Send("+HTTPTERM",6,20,2,0x0800);
}


/* Http初始化			*/
bool Http_Init(void)
{
	if(GPRS_APN_Connect() == false)						{return false;	}
	
	vTaskDelay(500);
	
	GSM_AT_Send("+HTTPINIT",6,5,1,0x0001);
	
	vTaskDelay(500);
		
	return true;
}
	
/* Http启动下载 		*/
bool Http_DownLoad(void)
{
	unsigned int i=0,j=0;
	EventBits_t Event_OTA;
	
	snprintf(dtbuf,80,"+HTTPPARA=\"URL\",\"http://www.toplovo.com/download/client/TL402%s.bin\"",GSM_OTA.program_ver);

	if(GSM_AT_Send(dtbuf,6,5,1,0x0001) == false) 	
	{	
		return false;	
	}
	
	vTaskDelay(500);
	
	if(GSM_AT_Send("+HTTPACTION=0",32,60,1,0x0010) == NULL)
	{		
		return false;
	}
	
	vTaskDelay(500);
		
	if(!(GSM_OTA.OTA_errcount & 0x01))
	{
		GSM_AT_Send("+FSCD=E:/",20,2,1,0x0010);				// e对应3，F对应1
		
		GSM_AT_Send("+FSDEL=\"TL401.bin\"",6,2,1,0x0800);
		
		GSM_AT_Send("+HTTPHEAD",11,2,1,0x0011);
		
		vTaskDelay(500);
		
		if(GSM_AT_Send("+HTTPREADFILE=\"TL401.bin\",3",26,60,1,0x0012) == NULL)
		{	
			return false;
		}
		
		send_str_to_gsm("AT+cftrantx=\"E:/TL401.bin\"\r\n");
		
		clr_gsm_buf();
		
		GPRS.b.OTA_Buf_Flag = true;GPRS.b.OTA_Get_Head_Flag = true; 
		
		while(1)
		{					
			Event_OTA = xEventGroupWaitBits(EventGroupHandler_OTH, Gprs_OTA_DMA_Flag, pdFALSE,pdFALSE,2000);
			
			xEventGroupClearBits(EventGroupHandler_OTH,Gprs_OTA_DMA_Flag);
			
			SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,SPI_StartAddr_New+i,512);
			
			if(Event_OTA & Gprs_OTA_DMA_Flag)											// DMA接收到数据
			{									
				i+=512;										
				
				if(i > 0x1B000) 
				{		
					return false;
				}
			}
			else
			{
				GPRS.b.OTA_Buf_Flag = false;	
				
				if(i < 0x2000)															// 下载长度小于8K,则认为下载失败
				{
					return false;
				}
				else
				{
					GSM_OTA.OTA_state |= 0x50; return true;
				}
			}
		}	
	}
	else
	{
		while(1)
		{	
			clr_gsm_buf();
			
			for(j=0;j<1024;j++){SPI_FLASH_BUF[j]=0;}
			
			GPRS.b.OTA_Buf_Flag = true;GPRS.b.OTA_Get_Head_Flag = true; 
			
			snprintf(dtbuf,30,"AT+HTTPREAD=%d,1024\r\n",i);
			
			send_str_to_gsm(dtbuf);
			
			Event_OTA = xEventGroupWaitBits(EventGroupHandler_OTH, Gprs_OTA_DMA_Flag, pdFALSE,pdFALSE,1000);
						
			SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,SPI_StartAddr_New+i,1024);
			
			if(Event_OTA & Gprs_OTA_DMA_Flag)											// DMA接收到数据
			{									
				i+=1024;										
				
				if(i > 0x1B000) 
				{		
					return false;
				}
			}
			else
			{
				GPRS.b.OTA_Buf_Flag = false;	
				
				if(i < 0x2000)															// 下载长度小于8K,则认为下载失败
				{
					return false;
				}
				else
				{
					GSM_OTA.OTA_state |= 0x50; return true;
				}
			}
		}	
	}
}


#endif

