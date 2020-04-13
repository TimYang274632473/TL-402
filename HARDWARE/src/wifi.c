#include "includes.h"

unsigned char wifi_rx_cnt = 0;
unsigned char wifi_row_cnt = 0;
unsigned char wifi_low_cnt = 0;

char wifi_rx_buf[wifi_rx_len] ={0};
char wifi_mac[wifi_row_len][wifi_low_len]={0,0,0};	

// TIM add
WIFI_set_msg WIFI;


/*���wifi_rx_buf��wifi_rx_cnt����*/
void clr_wifi_buf(void)	
{	
	wifi_rx_cnt = 0; 
	
	memset(wifi_rx_buf,0,wifi_rx_len);

	xEventGroupClearBits(EventGroupHandler_OTH,Wifi_Finish_Flag);
}

/*	����WIFIO	*/
void WIFI_on(void)
{
	HAL_GPIO_WritePin(WIFI_GPIO_Port, WIFI_Pin, GPIO_PIN_SET);vTaskDelay(300);
}

/*	����WIFIO	*/
void WIFI_off(void)
{
	HAL_GPIO_WritePin(WIFI_GPIO_Port, WIFI_Pin, GPIO_PIN_RESET);
}

/*	�����ַ���				*/
void send_str_to_wifi(char *pStr)
{	
	if(ECHO_F.b.MCU_WIFI_Flag == true)						
	{
		HAL_UART_Transmit(&huart1,(uint8_t*)pStr,strlen((char*)pStr),1000);
	}
	
	HAL_UART_Transmit(&huart4,(uint8_t*)pStr,strlen((char*)pStr),1000);
}

/*	sstr:�����ַ���			*/
bool WIFI_AT_Send(char *sstr)
{  
	for(u8 i=0;i<2;i++)
	{
		clr_wifi_buf();
		
		send_str_to_wifi("AT"); send_str_to_wifi(sstr); send_str_to_wifi("\r\n");	
		
		xEventGroupWaitBits(EventGroupHandler_OTH,Wifi_Finish_Flag, pdFALSE, pdFALSE,5000);
															
		if(strstr(wifi_rx_buf,"OK") != NULL)
		{
			return true;
		}
		else																	
		{			
			vTaskDelay(100);
		}
	}
	
	return false;
}

/* ��ȡ����·����MAC		*/
bool Wifi_Check_mac(bool Force)
{
	bool result = false;
	
	if((Force == false) && (wifi_row_cnt != 0) && (cnt_1s.Track_wifi_Cnt < 180)) {return false;}
		
	cnt_1s.Track_wifi_Cnt = 0;
	
	wifi_row_cnt = 0; wifi_low_cnt = 0;

	memset(wifi_mac,0,wifi_row_len * wifi_low_len);
	
	WIFI_on();
	
	MCU_F.b.Wifi_mac_Flag = false;
											//WIFI_AT_Send�����������еȴ��¼���ȷ��wifi�������
	WIFI_AT_Send("E0");						//�رջ���
	
	WIFI_AT_Send("+CWMODE=1");				//station  clientģʽ
	
	WIFI_AT_Send("+CWLAPOPT=1,14");			//CWLAPָ��������ʽ��rssiǿ�����У������ssid��rssi��mac��
	
	MCU_F.b.Wifi_mac_Flag = true;
	
	result = WIFI_AT_Send("+CWLAP");		//ɨ�踽��wifi
	
	WIFI_off();
	
	if(result == true)						//ɨ�赽����wifi
	{		
		for(wifi_row_cnt = 0; wifi_row_cnt < wifi_row_len; wifi_row_cnt++)
		{
			// TIM wifi_msg������ȡ
			WIFI_Msg_Cmp(wifi_mac);
			
			if(wifi_mac[wifi_row_cnt][0] == 0){break;}
			
			for(wifi_low_cnt = 0; wifi_low_cnt < wifi_low_len; wifi_low_cnt++)
			{
				if((wifi_mac[wifi_row_cnt][wifi_low_cnt-1] == ',') && (wifi_mac[wifi_row_cnt][wifi_low_cnt] == '\"'))
				{
					break;
				}
			}
			
			for(;wifi_low_cnt < wifi_low_len; wifi_low_cnt++)
			{
				if(wifi_mac[wifi_row_cnt][wifi_low_cnt] >= 'a')		//mac��ַת��Ϊ��д
				{
					wifi_mac[wifi_row_cnt][wifi_low_cnt] = wifi_mac[wifi_row_cnt][wifi_low_cnt] - 0x20;
				}
			}
		}
	}
	
	return result;
}	

/* TIM-Ŀ��wifi����				*/
bool WIFI_Search(void)
{
	Wifi_Check_mac(1);
	return(MCU_F.b.WIFI_Search_Flag);
}
/* TIM-ͨ��wifi����TCP����ƽ̨	*/
bool WIFI_Tcp_Connect(void)
{
	if(MCU_F.b.WIFI_Connected_Flag == true)
	{
		
	}
	return true;
}
/* TIM-�ն��豸���ӿͻ�·����	*/
bool WIFI_Connect_Router(void)
{
	char temp_str[50]="";
	
	/*���²�����δ����wifi���ÿ�ע��*/
	WIFI_AT_Send("CWMODE=3");	
	WIFI_AT_Send("+RST");
	/*���ϲ�����δ����wifi���ÿ�ע��*/
	WIFI_Generat_Str();
	
	/*�ȴ�wifi����ok*/										//pdFALSE�Ƿ�Ҫ���
	xEventGroupWaitBits(EventGroupHandler_OTH,Wifi_Finish_Flag, pdFALSE, pdFALSE,5000);
	if(strstr(wifi_rx_buf,"OK") != NULL)
   	{
		/*WIFI_Connetced_Flag   �Ƿ�ʹ���¼����־*/
		MCU_F.b.WIFI_Connected_Flag = true;
   	}	
   	else																	
   	{			
   		vTaskDelay(100);
		
	}
	return(MCU_F.b.WIFI_Connected_Flag);
	
}

/* TIM- wifi_msg������ȡ	     	*/
void WIFI_Msg_Cmp(char (*p)[wifi_low_len])
{
	for(int i=0;i<=wifi_row_len;i++)
	{
		if(NULL != strstr(*(p+i),WIFI.wifi_ssid))
		{
			//ȷ��ssid��ȫƥ��
			if( *(*(p+i) + strlen(WIFI.wifi_ssid) + 1) == '\"' )
			{
				MCU_F.b.WIFI_Search_Flag = true;
			}
		}
	}
}
/* TIM- wifi�����ַ�������	     	*/
void WIFI_Generat_Str(void)
{
	send_str_to_wifi("AT+CWJAP=\"");
	send_str_to_wifi(WIFI.wifi_ssid);
	send_str_to_wifi(",");
	send_str_to_wifi(WIFI.wifi_key);
	send_str_to_wifi("\"");
	/*	���������Ƿ���Ҫ����mac	*/
}
