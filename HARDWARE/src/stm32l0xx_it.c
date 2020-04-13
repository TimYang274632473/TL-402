#include "includes.h"

/* External variables --------------------------------------------------------*/
extern UART_HandleTypeDef hlpuart1;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern RTC_HandleTypeDef hrtc;

/******************************************************************************/
/*            Cortex-M0+ Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles Non maskable Interrupt.
*/
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
	
	printf("\r\nNMI\r\n");
	
	while (1)
	{
		NVIC_SystemReset();
	}
}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
	printf("\r\nHardFault\r\n");
	
	while (1)
	{		
		NVIC_SystemReset();
	}
  /* USER CODE BEGIN HardFault_IRQn 1 */

  /* USER CODE END HardFault_IRQn 1 */
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
	HAL_IncTick();
	osSystickHandler();
}

/******************************************************************************/
/* STM32L0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l0xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles EXTI line 0 and line 1 interrupts.
*/

/* �¼����ݻص�					*/
void Event_MCU_Callback(unsigned int Event_Bit)
{
	if(!(xEventGroupGetBitsFromISR(EventGroupHandler_MCU) & Event_Bit))
	{
		BaseType_t xHigherPriorityTaskWoken_Bit;
		
		if(xEventGroupSetBitsFromISR(EventGroupHandler_MCU, Event_Bit,&xHigherPriorityTaskWoken_Bit) != pdFAIL)
		{
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken_Bit);
		}
	}
}

/* �¼����ݻص�					*/
void Event_OTH_Callback(unsigned int Event_Bit)
{
	if(!(xEventGroupGetBitsFromISR(EventGroupHandler_OTH) & Event_Bit))
	{
		BaseType_t xHigherPriorityTaskWoken_Bit;
		
		if(xEventGroupSetBitsFromISR(EventGroupHandler_OTH, Event_Bit,&xHigherPriorityTaskWoken_Bit) != pdFAIL)
		{
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken_Bit);
		}
	}
}

/* RTC������Ӧ�ӳ���						*/
#if RTC_Alarm

void RTC_IRQHandler(void)
{	
	HAL_RTC_AlarmIRQHandler(&hrtc);
	
	if(GSMB.b.RTC_Update_Flag == true)
	{
		Event_MCU_Callback(MCU_RTC_Alarm_Flag);
	}
}

#endif

/* �ж���Ӧ�ӳ���				*/
void EXTI0_1_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_IT(KEY_SOS_GPIO_Pin) != RESET) 		// EX0: SOS������Ӧ
	{ 		
		__HAL_GPIO_EXTI_CLEAR_IT(KEY_SOS_GPIO_Pin);
				
		Tim_F.b.Chk_Key_Flag = true;																	
	}
	
	if(__HAL_GPIO_EXTI_GET_IT(CHK_CH_Pin) != RESET) 			// EX1: ������
	{ 
		__HAL_GPIO_EXTI_CLEAR_IT(CHK_CH_Pin);
		
#if Bat_Full_ON
		if(USBinsert_chk()==SET)
		{
			if(CHK_CH()==SET)									// ������,��USB�Բ���,Ϊ������
			{
				Tim_F.b.Chk_Bat_Charge_Full_Flag = true;
				cnt_1ms.Chk_Bat_Full_Cnt = 0;	
			}
			else												// �½���,��ʼ�����
			{
				Tim_F.b.Chk_Bat_Charing_Flag = true;
				cnt_1ms.Chk_Bat_Charge_Cnt = 0;			
			}	
		}
#endif
	}
}

/**
* @brief This function handles EXTI line 2 and line 3 interrupts.
*/
/* �ж���Ӧ�ӳ���				*/
void EXTI2_3_IRQHandler(void)
{	
	if(__HAL_GPIO_EXTI_GET_IT(BMA250_GPIO_Pin) != RESET) 		// EX2: BMA250������Ӧ
	{ 		
		__HAL_GPIO_EXTI_CLEAR_IT(BMA250_GPIO_Pin);
		
		Event_MCU_Callback(BMA250_Int_Flag);
	}
	
	if(__HAL_GPIO_EXTI_GET_IT(KEY_CHKPWR_GPIO_Pin) != RESET) 	// EX3: ��Դ������Ӧ
	{ 		
		__HAL_GPIO_EXTI_CLEAR_IT(KEY_CHKPWR_GPIO_Pin);
	
		Tim_F.b.Chk_Key_Flag = true;
	}
}

/**
* @brief This function handles EXTI line 4 to 15 interrupts.
*/
/* 	�ⲿ�ж���Ӧ�ӳ���			*/
void EXTI4_15_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_IT(KEY_CALL1_GPIO_Pin) != RESET) 	// EX4: CALL1������Ӧ
	{ 	
		__HAL_GPIO_EXTI_CLEAR_IT(KEY_CALL1_GPIO_Pin);
		
		Tim_F.b.Chk_Key_Flag = true;
	}
	
	if(__HAL_GPIO_EXTI_GET_IT(KEY_CALL2_GPIO_Pin) != RESET) 	// EX5: CALL2������Ӧ
	{ 	
		__HAL_GPIO_EXTI_CLEAR_IT(KEY_CALL2_GPIO_Pin);
		
		Tim_F.b.Chk_Key_Flag = true; 
	}
	
	if(__HAL_GPIO_EXTI_GET_IT(CHK_VIN_Pin) != RESET)			// EX13: CHK_VIN����USB������
	{		
		__HAL_GPIO_EXTI_CLEAR_IT(CHK_VIN_Pin);
				
		if(USBinsert_chk() == SET)								// ������,USB�ղ���,�����붨ʱ����ʵ	
		{
			Tim_F.b.Chk_USB_Connect_Flag = true;	
			cnt_1ms.Chk_USB_Connect_Cnt = 0;			
		}
		else													// �½���,USB�հ���
		{
			Tim_F.b.Chk_USB_Disconnect_Flag = true;
			cnt_1ms.Chk_USB_Connect_Cnt = 0;	
		}
	}
}

#if MODULE_UG96

/* Lpuart1 DMA ����, DMA�����⺯���ѹر�,������ж� */
void DMA1_Channel2_3_IRQHandler(void)
{		
	HAL_DMA_IRQHandler(&hdma_lpuart1_rx);
	
	HAL_UART_DMAPause(&hlpuart1);
	
	if(GPRS.b.OTA_Buf_Flag == false)					// ������յ���һ������
	{
		HAL_UART_Receive_DMA(&hlpuart1,SPI_FLASH_BUF+2048,2048);
		
		GPRS.b.OTA_Buf_Flag = true;
	}
	else
	{
		HAL_UART_Receive_DMA(&hlpuart1,SPI_FLASH_BUF,2048);
		
		GPRS.b.OTA_Buf_Flag = false;
	}
	
	HAL_UART_DMAResume(&hlpuart1);
	
	Event_OTH_Callback(Gprs_OTA_DMA_Flag);
}

#endif

/* uart1 DMA ����, DMA�����⺯���ѹر�,������ж� */
void DMA1_Channel4_5_6_7_IRQHandler(void)
{	
	HAL_DMA_IRQHandler(&hdma_usart1_rx);
	
	HAL_UART_DMAPause(&huart1);
	
	if(GPRS.b.OTA_Buf_Flag == false)				// ������յ���һ������
	{
		HAL_UART_Receive_DMA(&huart1,SPI_FLASH_BUF+2048,2048);
		
		GPRS.b.OTA_Buf_Flag = true;
	}
	else
	{
		HAL_UART_Receive_DMA(&huart1,SPI_FLASH_BUF,2048);
		
		GPRS.b.OTA_Buf_Flag = false;
	}
	
	HAL_UART_DMAResume(&huart1);
	
	Event_OTH_Callback(Gprs_OTA_DMA_Flag);
}

/**
* @brief This function handles USART1 global interrupt / USART1 wake-up interrupt through EXTI line 25.
*/
/* PC ����1�жϳ���				*/
void USART1_IRQHandler(void)									// ���ӵ�PC�Ĵ���,�����ⲿ���������ַ�������
{
	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_ORE) != RESET) 
	{
		__HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_ORE);
	}
	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_FE) != RESET) 
	{
		__HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_FE);
	}
	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_NE) != RESET) 
	{
		__HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_NE);
	}
	if(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_RXNE) != RESET)	//(__HAL_UART_GET_IT_SOURCE(&huart1, UART_IT_RXNE) != RESET)||
	{ 
		__HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_RXNE);
#if MODULE_SIM7500	
		if(MCU_F.b.Uart1_to_Luart1)
		{
			LUSART1_SendData((uint8_t)(huart1.Instance->RDR & (uint8_t)0x00FF));
						
			if(++Aduio_Cnt >= Audio_Length)
			{
				MCU_F.b.Uart1_to_Luart1 = false;
			}
		}
		else
		{
#endif
			USART1_DatHandle((uint8_t)(huart1.Instance->RDR & (uint8_t)0x00FF));
#if MODULE_SIM7500
		}			
#endif
	}
}

/**
* @brief This function handles USART2 global interrupt / USART2 wake-up interrupt through EXTI line 26.
*/
/* GPS ����2�жϳ���				*/

void USART2_IRQHandler(void)
{  
	char dat;
	
	if(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_ORE) != RESET) 
	{	
		__HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_ORE);
	}
	if(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET)
	{ 		
		__HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_RXNE);
		
		dat = (uint8_t)(huart2.Instance->RDR & (uint8_t)0x00FF);
				
		if(ECHO_F.b.GPS_MCU_Flag == ON)											// �����Ҫ��GPS��������ת����PC��
		{
			USART1_SendData(dat);
		}
				
		if(GPS_F.b.GPRMC_END_Flag && GPS_F.b.GPGGA_END_Flag) 
		{			
			return;																	
		}
		
		if(GPS_F.b.GPRMC_Flag)													// ��Ҫ����RMC����
		{
			GPS_RMC_Buf[GPS_Rx_Cnt]=dat;
			if(dat=='*')
			{
				GPS_RMC_Buf[GPS_Rx_Cnt]=0;										// ����RMC�������
				GPS_F.b.GPRMC_Flag=0;
				GPS_Rx_Cnt=0;
				GPS_F.b.GPRMC_END_Flag=1;
			}
			else
			{
				if(GPS_Rx_Cnt++ > (GPS_Buf_Len-1))								// ���������ַ�
				{
					GPS_F.b.GPRMC_Flag=0;GPS_Rx_Cnt=0;
				}
				return;				
			}
		}	
		
		if(GPS_F.b.GPGGA_Flag)													// ��Ҫ����GGA����
		{
			GPS_GGA_Buf[GPS_Rx_Cnt]=dat;
			if(dat=='*')														// ����GGA������ɣ�׼������RMC����
			{
				GPS_GGA_Buf[GPS_Rx_Cnt]=0;
				GPS_F.b.GPGGA_Flag=0;
				GPS_Rx_Cnt=0;
				GPS_F.b.GPGGA_END_Flag=1;
			}
			else
			{
				if(GPS_Rx_Cnt++ > (GPS_Buf_Len-1))								// ���������ַ�
				{
					GPS_F.b.GPGGA_Flag=0;GPS_Rx_Cnt=0;
				}
				return;				
			}
		}
		
		if(GPS_F.b.GPRMC_END_Flag && GPS_F.b.GPGGA_END_Flag)
		{			
			if(!ECHO_F.b.GPS_MCU_Flag)
			{
				NVIC_DisableIRQ(USART2_IRQn);
			}
			
			Event_OTH_Callback(Gps_GPRMC_GGA_OK_Flag);
		}
		
		GPS_Str_Buf[GPS_Rx_Cnt]=dat;
		
		if((GPS_Str_Buf[GPS_Rx_Cnt]=='A')&&(GPS_Str_Buf[GPS_Rx_Cnt-1]=='G')&&(GPS_Str_Buf[GPS_Rx_Cnt-2]=='G')&&(!GPS_F.b.GPGGA_Flag))			// GGA
		{
			GPS_GGA_Buf[0]='G';GPS_GGA_Buf[1]='G';GPS_GGA_Buf[2]='A';
			GPS_F.b.GPGGA_Flag=1;GPS_Rx_Cnt=3;
		}
		else if((GPS_Str_Buf[GPS_Rx_Cnt]=='C')&&(GPS_Str_Buf[GPS_Rx_Cnt-1]=='M')&&(GPS_Str_Buf[GPS_Rx_Cnt-2]=='R')&&(!GPS_F.b.GPRMC_Flag))		// RMC
		{
			GPS_RMC_Buf[0]='R';GPS_RMC_Buf[1]='M';GPS_RMC_Buf[2]='C';
			GPS_F.b.GPRMC_Flag=1;GPS_Rx_Cnt=3;
		}
		else if(dat == '$')																								
		{
			GPS_Rx_Cnt=0;
		}
		else
		{
			if((GPS_Rx_Cnt++) >= 5) GPS_Rx_Cnt=0;
		}		
	}
}


/* ����4/5 WIFI�жϳ���	*/

#if Wifi_Enable

void USART4_5_IRQHandler(void)
{
	char dat;
	
	if(__HAL_UART_GET_FLAG(&huart4, UART_FLAG_ORE) != RESET) 
	{	
		__HAL_UART_CLEAR_FLAG(&huart4, UART_FLAG_ORE);
	}
	if(__HAL_UART_GET_FLAG(&huart4, UART_FLAG_RXNE) != RESET)
	{
		__HAL_UART_CLEAR_FLAG(&huart4, UART_FLAG_RXNE);
		
		dat = (uint8_t)(huart4.Instance->RDR & (uint8_t)0x00FF);
	
		if(ECHO_F.b.WIFI_MCU_Flag == ON)	// �����Ҫ��WIFI��������ת����PC��
		{
			USART1_SendData(dat);
		}	
		
		wifi_rx_buf[wifi_rx_cnt] = dat;
		
		if(++wifi_rx_cnt >= wifi_rx_len) {wifi_rx_cnt = 0;}
		
		// TIM-add WIFI_Tcp_Connected_Flag�жϽ���wifi���ݣ��������ݣ���������),�練������ʧ�ܣ���������x������
		
		if(MCU_F.b.Wifi_mac_Flag == true)						//��ȡMAC��ַ
		{
			if(MCU_F.b.Wifi_mac_str_Flag == true)
			{
				if(dat == '\r')									//һ��ssid���ݽ���
				{
					if((wifi_low_cnt > 26) && (wifi_mac[wifi_row_cnt][wifi_low_cnt-2] == '"'))
					{
						wifi_mac[wifi_row_cnt][wifi_low_cnt-1] = 0; wifi_row_cnt++;
					}
					else
					{
						wifi_mac[wifi_row_cnt][0] = 0;
					}
					
					wifi_low_cnt = 0; MCU_F.b.Wifi_mac_str_Flag = false;	
				}
				else if(wifi_low_cnt >= wifi_low_len)			//һ��ssid���ݳ���60B
				{
					wifi_low_cnt = 0; MCU_F.b.Wifi_mac_str_Flag = false;
				}
				else											//������������
				{
					wifi_mac[wifi_row_cnt][wifi_low_cnt] = dat;
					
					if(++wifi_low_cnt >= wifi_low_len){wifi_low_cnt = 0;MCU_F.b.Wifi_mac_str_Flag = false;}
				}
					
				return;				
			}
			//һ��ssid���ݼ�⵽����'('��ʼ�洢
			if((dat == '(') && (MCU_F.b.Wifi_mac_str_Flag == false) && (wifi_row_cnt < wifi_row_len))
			{	
				wifi_rx_cnt = 0; 
				wifi_low_cnt = 0; 
				MCU_F.b.Wifi_mac_str_Flag = true;
			}			
			else if(((wifi_rx_buf[wifi_rx_cnt-1] == '\r') && (wifi_rx_buf[wifi_rx_cnt-2] == 'K') && (wifi_rx_buf[wifi_rx_cnt-3] == 'O'))		// OK
				  ||((wifi_rx_buf[wifi_rx_cnt-1] == '\r') && (wifi_rx_buf[wifi_rx_cnt-2] == 'R') && (wifi_rx_buf[wifi_rx_cnt-3] == 'O')))		// ERROR
			{
				Event_OTH_Callback(Wifi_Finish_Flag);	//WIFI����OK��error������¼���													
			}
			else if(((wifi_rx_buf[wifi_rx_cnt] == 'W') && (wifi_rx_buf[wifi_rx_cnt-1] == 'C') && (wifi_rx_buf[wifi_rx_cnt-2] == '+'))
				|| ((wifi_rx_buf[wifi_rx_cnt] == '\n') && (wifi_rx_buf[wifi_rx_cnt-1] == '\r')))
			{
				wifi_rx_cnt = 0; 
				wifi_low_cnt = 0;
			}
		}
		else
		{
			if(wifi_rx_cnt >= 6)
			{
				Event_OTH_Callback(Wifi_Finish_Flag);	//û�л�ȡmacʱ������6���ֽ����¼����־��λ
			}
		}
	}
}

#endif

/**
* @brief This function handles LPUART1 interrupt / LPUART1 wake-up interrupt through EXTI line 28.
*/

#if CALLIN_ENABLE
static u8 ringnub_count=0;					// �������洢����
#endif
#if SMS_ENABLE
static unsigned char SMS_TMP=0;
#endif

/* MCU_F �͹��Ĵ���1�жϳ���				*/
void LPUART1_IRQHandler(void)
{	
	if(__HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_WUF) != RESET)
	{ 
		__HAL_UART_CLEAR_FLAG(&hlpuart1, UART_FLAG_WUF);
	}
	if(__HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_ORE) != RESET) 
	{
		__HAL_UART_CLEAR_FLAG(&hlpuart1, UART_FLAG_ORE);
	}
	if(__HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_RXNE) != RESET)
	{ 
		char dat;
				
		dat =(uint8_t)(hlpuart1.Instance->RDR & (uint8_t)0x00FF);
		
		__HAL_UART_CLEAR_FLAG(&hlpuart1, UART_FLAG_RXNE);

#if MODULE_SIM7500 && GPRS_ENABLE
				
		if(GPRS.b.OTA_Buf_Flag)																							// ����4K OTA����
		{
			if(GPRS.b.OTA_Get_Head_Flag == true)
			{
				if((++gsm_rx_sta > 18) && (dat == '\n'))
				{
					gsm_rx_sta = 0;GPRS.b.OTA_Get_Head_Flag = false;
				}
				
				if(ECHO_F.b.GSM_MCU_Flag == ON)																					// ���MCU_F����������Ҫ���Ե�PC��
				{
					USART1_SendData(dat);
				}
		
				return;
			}
			
			SPI_FLASH_BUF[gsm_rx_sta++] = dat;
			
			if((gsm_rx_sta >= 512) && (!(GSM_OTA.OTA_errcount & 0x01)))			// readfile
			{		
				GPRS.b.OTA_Get_Head_Flag = true; gsm_rx_sta = 0; Event_OTH_Callback(Gprs_OTA_DMA_Flag);
			}
			else if((gsm_rx_sta >= 1024) && (GSM_OTA.OTA_errcount & 0x01))
			{				
				GPRS.b.OTA_Buf_Flag = false;Event_OTH_Callback(Gprs_OTA_DMA_Flag);
			}
					
			return;
		}
		
#endif	
		
		if(ECHO_F.b.GSM_MCU_Flag == ON)																					// ���MCU_F����������Ҫ���Ե�PC��
		{
			USART1_SendData(dat);
		}	
		
		gsm_rx_buf[gsm_rx_sta]=dat;
										
#if CALLIN_ENABLE																										// �ѿ�������绰����	
		if(CALL.b.Ring_Flag)																							// ������
		{	
			if((!CALL.b.CallInPhone_Save_Flag) && Ring_Clip_Count)														// ���δ����绰�����������������0
			{		
				if((gsm_rx_buf[gsm_rx_sta]!='"')&&(ringnub_count<20))
				{
					Ring_Phone_Number[ringnub_count]=gsm_rx_buf[gsm_rx_sta];											// �洢����	
					ringnub_count++;
				}
				else																									// ���յ���һ��",����绰����������,���ٽ��յ绰����
				{		
					cnt_1s.Ring_Time = 0;
					CALL.b.Ring_Cnt_Flag = true;					
					CALL.b.CallInPhone_Save_Flag = true;
					Ring_Phone_Number[ringnub_count]=0;		
					ringnub_count=0;
					Task_AT_GSM_Callback(Gsm_Call_In);										
				}	
			}
		}
		
		if((gsm_rx_buf[gsm_rx_sta]=='G')&&(gsm_rx_buf[gsm_rx_sta-1]=='N')&&(gsm_rx_buf[gsm_rx_sta-2]=='I')&&(gsm_rx_buf[gsm_rx_sta-3]=='R'))	
		{
			if(CALL.b.Ring_Cnt_Flag){cnt_1s.Ring_Time = 0;}
		}
		
		if((gsm_rx_buf[gsm_rx_sta]=='"')&&(gsm_rx_buf[gsm_rx_sta-2]==':')&&(gsm_rx_buf[gsm_rx_sta-3]=='P')&&(gsm_rx_buf[gsm_rx_sta-5]=='L'))  // ͳ��"+CLIP:"���ִ���,�����������
		{	
			Ring_Clip_Count++;			
			cnt_1s.Ring_Time = 0;	
			CALL.b.Ring_Flag = true;
			if(CALL.b.CallInPhone_Save_Flag) {gsm_rx_sta = 0;}			
#if MODULE_SIM7500
			CALL.b.Play_Ring_Flag = true;
#endif
		}
#endif

#if CALLOUT_ENABLE	&& MODULE_SIM7500
		
		if(CALL.b.Dtmf_Flag)
		{
			if((gsm_rx_buf[gsm_rx_sta-2]=='1')&&(gsm_rx_buf[gsm_rx_sta-5]=='F')&&(gsm_rx_buf[gsm_rx_sta-10]=='R'))		// +RXDTMF: 1
			{
				CALL.b.Dtmf_Flag = false;Event_OTH_Callback(DTMF_OK_Flag);
			}
		}
		
#endif
		
#if MODULE_SIM7500 && (CALLIN_ENABLE || CALLOUT_ENABLE)
		
		if(CALL.b.Play_Audio_Flag)														// +AUDIOSTATE: audio play stop
		{
			if((gsm_rx_buf[gsm_rx_sta-2]=='p')&&(gsm_rx_buf[gsm_rx_sta-5]=='s')&&(gsm_rx_buf[gsm_rx_sta-7]=='y'))		
			{
				CALL.b.Play_Audio_Flag = false;Event_OTH_Callback(Audio_Finish_Flag);
			}
		}
		
#endif
		
#if SMS_ENABLE
		if((gsm_rx_buf[gsm_rx_sta-2]==',')&&(gsm_rx_buf[gsm_rx_sta-4]=='M')&&(gsm_rx_buf[gsm_rx_sta-5]=='S'))			//+CMTI: "SM",1	������������, ������99������
		{   
			SMS_TMP=gsm_rx_buf[gsm_rx_sta-1]&0x0F;
			if((gsm_rx_buf[gsm_rx_sta] >= '0') && (gsm_rx_buf[gsm_rx_sta] <= '9'))
			{
				SMS_TMP = (SMS_TMP * 10) + (gsm_rx_buf[gsm_rx_sta] & 0x0F);
			}
			if(SMS_TMP > SMS_NUM)		{SMS_NUM=SMS_TMP;}																// ��ȡ�����µĶ���
			if(SMS_NUM > 99) SMS_NUM=99;																				// һ��������99������ 
			SMS.b.New_SMS_Flag = true;
			Task_AT_GSM_Callback(Gsm_New_SMS);
		}
		
		if((gsm_rx_buf[gsm_rx_sta]=='\n')&&(gsm_rx_buf[gsm_rx_sta-4]=='U')&&(gsm_rx_buf[gsm_rx_sta-8]=='M')&&(gsm_rx_buf[gsm_rx_sta-10]=='+'))	// +SMS_FULL\r\n
		{
			Task_AT_GSM_Callback(Gsm_SMS_Full);
		}
		
		if(SMS.b.Chk_CMGL_Flag)																							// �����Ҫ��ȡ����δ������
		{	
			if((gsm_rx_buf[gsm_rx_sta-3]==':')&&(gsm_rx_buf[gsm_rx_sta-4]=='L')&&(gsm_rx_buf[gsm_rx_sta-8]=='+'))		// +CMGL: 4, ��ȡ��δ���������
			{   
				SMS_TMP=gsm_rx_buf[gsm_rx_sta-1]&0x0F;
				if((gsm_rx_buf[gsm_rx_sta] >= '0') && (gsm_rx_buf[gsm_rx_sta] <= '9'))
				{
					SMS_TMP = (SMS_TMP * 10) + (gsm_rx_buf[gsm_rx_sta] & 0x0F);
				}
				if(SMS_TMP > SMS_NUM)	{SMS_NUM = SMS_TMP;}															// ��ȡ�����µĶ���
				if(SMS_NUM > 99) SMS_NUM=99;																			// һ��������99������ 
				SMS.b.New_SMS_Flag = true;
				gsm_rx_sta = 0;																							// �������ָ����0,�������̫��,������Χ	
			}
			else if((gsm_rx_buf[gsm_rx_sta]=='\n')&&(gsm_rx_buf[gsm_rx_sta-2]=='K')&&(gsm_rx_buf[gsm_rx_sta-3]=='O'))	// \r\nOK\r\n ���յ��¶������,��û���¶���
			{
				SMS.b.Chk_CMGL_Flag = false;Event_OTH_Callback(CMGL_OK_Flag);		
			}
		}
		
		if(SMS.b.Chk_CMGS_Flag)																							// ������������Ͷ�������	+CMGS:
		{
			if((gsm_rx_buf[gsm_rx_sta]=='S')&&(gsm_rx_buf[gsm_rx_sta-1]=='G')&&(gsm_rx_buf[gsm_rx_sta-3]=='C'))							
			{			
				SMS.b.Chk_CMGS_Flag = false;Event_OTH_Callback(CMGS_Send_OK_Flag);
			}
			else if((gsm_rx_buf[gsm_rx_sta]=='R')&&(gsm_rx_buf[gsm_rx_sta-1]=='O')&&(gsm_rx_buf[gsm_rx_sta-4]=='E'))
			{																											// ���ŷ���ʧ��
				SMS.b.Chk_CMGS_Flag = false;Event_OTH_Callback(CMGS_Send_NG_Flag);
			}
		}
#endif	
	
#if GPRS_ENABLE	 	
		
		if(GSMA.b.GPRS_flag)																							// �ѿ���GPRS
		{
			if(GPRS.b.Chk_Notif_Flag)																					// GPRS����ƽ̨���ݺ�,��Ҫ��ȡ���ͳɹ���Ϣ
			{
#if MODULE_UG96
				if((gsm_rx_buf[gsm_rx_sta]=='\n')&&(gsm_rx_buf[gsm_rx_sta-2]=='K')&&(gsm_rx_buf[gsm_rx_sta-5]=='D')&&(gsm_rx_buf[gsm_rx_sta-6]=='N'))	// SEND OK\r\n
#elif MODULE_SIM7500
				if((gsm_rx_buf[gsm_rx_sta]=='0')&&(gsm_rx_buf[gsm_rx_sta-2]==':')&&\
				   (gsm_rx_buf[gsm_rx_sta-5]=='E')&&(gsm_rx_buf[gsm_rx_sta-6]=='S'))	// +CIPSEND: 0,127,127\r\n
#endif			 
				{
					GPRS.b.Chk_Notif_Flag = false;
					Event_OTH_Callback(Gprs_Send_OK_Flag);
				}

#if MODULE_UG96
				else if((gsm_rx_buf[gsm_rx_sta]=='\n')&&(gsm_rx_buf[gsm_rx_sta-2]=='L')&&(gsm_rx_buf[gsm_rx_sta-7]=='D')&&(gsm_rx_buf[gsm_rx_sta-8]=='N'))	// SEND FAIL\r\n
				{
					GPRS.b.Chk_Notif_Flag = false;Event_OTH_Callback(Gprs_Send_NG_Flag);
				}	
#endif		
			} 
				
			if(GPRS.b.Chk_Connected_Flag)																				// ��Ҫ��ȡGPRS����״̬�����������Ƿ�ɹ�,CONNECT�ظ�
			{
#if MODULE_UG96
				if((gsm_rx_buf[gsm_rx_sta]=='\n')&&(gsm_rx_buf[gsm_rx_sta-4]=='1')&&(gsm_rx_buf[gsm_rx_sta-6]==':')&&(gsm_rx_buf[gsm_rx_sta-7]=='N'))	// +QIOPEN: 1,0\r\n		GPRS����״̬����
#elif MODULE_SIM7500
				if((gsm_rx_buf[gsm_rx_sta]=='\n')&&(gsm_rx_buf[gsm_rx_sta-4]=='0')&&(gsm_rx_buf[gsm_rx_sta-6]==':')&&(gsm_rx_buf[gsm_rx_sta-7]=='N'))	// +CIPOPEN: 0,0\r\n
#endif					
				{											
					GPRS.b.Chk_Connected_Flag = false;Event_OTH_Callback(Gprs_Connected_OK_Flag);
				}

#if  MODULE_UG96
				else if((gsm_rx_buf[gsm_rx_sta]=='\n')&&(gsm_rx_buf[gsm_rx_sta-6]=='1')&&(gsm_rx_buf[gsm_rx_sta-8]==':')&&(gsm_rx_buf[gsm_rx_sta-9]=='N'))	// +QIOPEN: 1,565\r\n	
#elif MODULE_SIM7500
				else if((gsm_rx_buf[gsm_rx_sta]=='\n')&&(gsm_rx_buf[gsm_rx_sta-5]=='0')&&(gsm_rx_buf[gsm_rx_sta-7]==':')&&(gsm_rx_buf[gsm_rx_sta-8]=='N'))	// +CIPOPEN: 0,11\r\n
#endif					
				{											
					GPRS.b.Chk_Connected_Flag = false;
					Event_OTH_Callback(Gprs_Connected_NG_Flag);
				}			
			}		

			if(GPRS.b.Chk_Ciprxget_Flag)																				// ��������ȡƽ̨����������
			{
#if MODULE_UG96
				if((gsm_rx_buf[gsm_rx_sta-4]=='\"')&&(gsm_rx_buf[gsm_rx_sta-5]=='v')&&(gsm_rx_buf[gsm_rx_sta-7]=='e'))	// +QIURC: "recv",1\r\n
#elif MODULE_SIM7500
				if((gsm_rx_buf[gsm_rx_sta-4]=='1')&&(gsm_rx_buf[gsm_rx_sta-6]==':')&&(gsm_rx_buf[gsm_rx_sta-11]=='R'))	// +CIPRXGET: 1,0\r\n
#endif
				{
					GPRS.b.Chk_Ciprxget_Flag = false;
					Task_AT_GSM_Callback(Gprs_New_Msg);																	// ������ȡGPRSָ��
				}
			}
				
			if(GPRS.b.Connected_OK_Flag)
			{
#if MODULE_UG96
				if((gsm_rx_buf[gsm_rx_sta-4]=='\"')&&(gsm_rx_buf[gsm_rx_sta-5]=='d')&&(gsm_rx_buf[gsm_rx_sta-7]=='s'))	// +QIURC: "closed",1\r\n		+QIURC: "pdpdeact",1\r\n
#elif MODULE_SIM7500
				if(((gsm_rx_buf[gsm_rx_sta-4]=='0')&&(gsm_rx_buf[gsm_rx_sta-6]==':')&&(gsm_rx_buf[gsm_rx_sta-7]=='E')&&(gsm_rx_buf[gsm_rx_sta-14]=='+'))	// +IPCLOSE: 0,1\r\n
					
				|| ((gsm_rx_buf[gsm_rx_sta-4]=='D')&&(gsm_rx_buf[gsm_rx_sta-6]=='T')&&(gsm_rx_buf[gsm_rx_sta-16]=='E')&&(gsm_rx_buf[gsm_rx_sta-20]=='C')))	// +CIPEVENT: NETWORK CLOSED UNEXPECTEDLY\r\n
#endif
				{
					GPRS.b.Connected_OK_Flag = false;cnt_1s.GPRS_Reconnect_Cnt = 0;					
					if(GPRS.b.Chk_Notif_Flag)	
					{	
						GPRS.b.Chk_Notif_Flag = false;
						Event_OTH_Callback(Gprs_Send_NG_Flag);	
					}
					else
					{
						Task_AT_GSM_Callback(Gprs_ReConnect_Now);								
					}
				}
			}
		}
#endif	
	
#if MODULE_UG96	
		if((MCU_F.b.GSM_Init_Flag == false) && (MCU_F.b.No_Simcard_Flag == false))
		{																												// +CPIN: NOT READY\r\n ����
			if((gsm_rx_buf[gsm_rx_sta-2]=='Y')&&(gsm_rx_buf[gsm_rx_sta-10]=='N')&&(gsm_rx_buf[gsm_rx_sta-14]=='I')&&(gsm_rx_buf[gsm_rx_sta-16]=='C'))	
			{
				Task_AT_GSM_Callback(Gsm_cfun_reset);
			}
		}			
#elif MODULE_SIM7500																											// +SIMCARD: NOT AVAILABLE\r\n����
		if(MCU_F.b.GSM_Init_Flag == false)
		{
			if((gsm_rx_buf[gsm_rx_sta-2]=='Y')&&(gsm_rx_buf[gsm_rx_sta-3]=='D')&&(gsm_rx_buf[gsm_rx_sta-4]=='R'))		// RDY\r\n
			{
				Task_SPI_Flash_Callback(Flash_MCU_Reset);
			}
			
			if(MCU_F.b.No_Simcard_Flag == false)
			{
				if((gsm_rx_buf[gsm_rx_sta-2]=='E')&&(gsm_rx_buf[gsm_rx_sta-14]=='N')&&(gsm_rx_buf[gsm_rx_sta-17]=='D')&&(gsm_rx_buf[gsm_rx_sta-23]=='S'))	
				{
					Task_AT_GSM_Callback(Gsm_cfun_reset);
				}
			}
		}
#endif
		
		if((++gsm_rx_sta) > gsm_rx_len){gsm_rx_sta=0;}	
		
		if(MCU_F.b.Chk_AT_Cnt_Flag)																						// ����ѿ�����⴮���ַ�����																					
		{
			if(gsm_rx_sta >= gsm_rx_req)
			{
				MCU_F.b.Chk_AT_Cnt_Flag = false;Event_OTH_Callback(AT_Cnt_OK_Flag);
			}
		}
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
