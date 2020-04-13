#include "includes.h"												 		

/* �����Ǵ���1���ձ���	*/
char 		USART1_RXD_DATA[USART_RXD_Group_Len][USART_RXD_Line_Len];	// ��Ŵ��ڽ��յ�����
uint8_t 	USART1_RXD_Group_Cnt=0;										// ��
uint8_t 	USART1_RXD_Line_Cnt=0;										// ��
#if MODULE_SIM7500
unsigned int Aduio_Cnt = 0;
unsigned int Audio_Length=0;
#endif

/**********************************************************************************************************
*	�� �� ��: USART1_DATA
*	����˵��: �жϽ��ձ��������ӳ���,���������жϳ������
*	��    �Σ�8λ�жϽ�������
*	�� �� ֵ: ��
**********************************************************************************************************/
void USART1_DatHandle(char dat)
{
	if(dat != 0x0A)								// Backspace ��Ӧ�ĺ��� 0x08
	{					 
		if(dat == 0x08)							// ��ɾ��  \b
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
		else if(ECHO_F.b.PC_MCU_Flag==ON)		// ��������1ֱ�ӻ���				// �ж��Ƿ���Ҫ����
		{
			USART1_SendData(dat);				// �����ַ�
		}
		if(dat == 0x0D)							// �س���, ���Ҳ�����س��ַ�
		{					      
			if(ECHO_F.b.PC_MCU_Flag==ON)		// ���Ի���									
			{
				USART1_SendData('\n');															// ����	
			}																					
			if(USART1_RXD_DATA[USART1_RXD_Group_Cnt][USART_RXD_Line_Len-1] == '\0')				// û��δ�����ָ��
			{
				BaseType_t xHigherPriorityTaskWoken = pdFALSE;
				
				USART1_RXD_DATA[USART1_RXD_Group_Cnt][USART1_RXD_Line_Cnt]='\0';
				USART1_RXD_DATA[USART1_RXD_Group_Cnt][USART_RXD_Line_Len-1] = 1;				// ���б�־λ
				if(USART1_RXD_Group_Cnt++ == USART_RXD_Group_Len-1)
				{
					USART1_RXD_Group_Cnt = 0;
				}
				USART1_RXD_Line_Cnt = 0;														// ��һ��,��һ��
				//���ֵ�ź�����ʹ�ã����ͷ�(�ÿ�)
				if(Semap_PcCmd != NULL)
				{
					xSemaphoreGiveFromISR(Semap_PcCmd,&xHigherPriorityTaskWoken);
					//����ͷŶ�ֵ�ź������и������ȼ���������������Ҫ����һ�������л�
					if(xHigherPriorityTaskWoken == pdTRUE)
					{
						portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
					}
				}
			}
		}
		else
		{
			if(USART1_RXD_DATA[USART1_RXD_Group_Cnt][USART_RXD_Line_Len-1] == '\0')				// û��δ�����ָ��
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
/* ��������		*/

void USART_Update(void)
{		
	SPI_Flash_Wakeup();													// �Ͽ�GPRS����,Flash���ֻ���

	printf("\r\nMCU Is Ready Now!\r\n");								// PC���յ���ָ���,���Կ�ʼ��������
	
	MX_UART1_DMA_Init();												// ����DMA
	
	HAL_UART_Receive_DMA(&huart1,SPI_FLASH_BUF,2048);					// �����״�DMA����,�⺯�����ѹرմ���һ���жϹ���
				
	if(OTA_Store() == false)											// ota�洢����	
	{
		GSM_OTA.OTA_errcount++; 
	}
	
	SPI_Flash_Write(otaaddr, (u8*)&GSM_OTA,GSM_OTA_size);				//�������ؽ��
	
	SPI_Flash_Sleep();													// Flash����
	
	HAL_UART_DMAStop(&huart1);											// �ر�DMA
									
	NVIC_SystemReset();													// ��λ	
}
#endif
