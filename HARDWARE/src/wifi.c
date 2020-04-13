#include "includes.h"

unsigned char wifi_rx_cnt = 0;
unsigned char wifi_row_cnt = 0;
unsigned char wifi_low_cnt = 0;

char wifi_rx_buf[wifi_rx_len] ={0};
char wifi_mac[wifi_row_len][wifi_low_len]={0,0,0};	

// TIM add
WIFI_set_msg WIFI;


/*清除wifi_rx_buf，wifi_rx_cnt归零*/
void clr_wifi_buf(void)	
{	
	wifi_rx_cnt = 0; 
	
	memset(wifi_rx_buf,0,wifi_rx_len);

	xEventGroupClearBits(EventGroupHandler_OTH,Wifi_Finish_Flag);
}

/*	开启WIFIO	*/
void WIFI_on(void)
{
	HAL_GPIO_WritePin(WIFI_GPIO_Port, WIFI_Pin, GPIO_PIN_SET);vTaskDelay(300);
}

/*	开启WIFIO	*/
void WIFI_off(void)
{
	HAL_GPIO_WritePin(WIFI_GPIO_Port, WIFI_Pin, GPIO_PIN_RESET);
}

/*	发送字符串				*/
void send_str_to_wifi(char *pStr)
{	
	if(ECHO_F.b.MCU_WIFI_Flag == true)						
	{
		HAL_UART_Transmit(&huart1,(uint8_t*)pStr,strlen((char*)pStr),1000);
	}
	
	HAL_UART_Transmit(&huart4,(uint8_t*)pStr,strlen((char*)pStr),1000);
}

/*	sstr:发送字符串			*/
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

/* 获取附近路由器MAC		*/
bool Wifi_Check_mac(bool Force)
{
	bool result = false;
	
	if((Force == false) && (wifi_row_cnt != 0) && (cnt_1s.Track_wifi_Cnt < 180)) {return false;}
		
	cnt_1s.Track_wifi_Cnt = 0;
	
	wifi_row_cnt = 0; wifi_low_cnt = 0;

	memset(wifi_mac,0,wifi_row_len * wifi_low_len);
	
	WIFI_on();
	
	MCU_F.b.Wifi_mac_Flag = false;
											//WIFI_AT_Send函数都包含有等待事件组确认wifi返回情况
	WIFI_AT_Send("E0");						//关闭回显
	
	WIFI_AT_Send("+CWMODE=1");				//station  client模式
	
	WIFI_AT_Send("+CWLAPOPT=1,14");			//CWLAP指令的输出格式（rssi强度排列，并输出ssid，rssi，mac）
	
	MCU_F.b.Wifi_mac_Flag = true;
	
	result = WIFI_AT_Send("+CWLAP");		//扫描附近wifi
	
	WIFI_off();
	
	if(result == true)						//扫描到附近wifi
	{		
		for(wifi_row_cnt = 0; wifi_row_cnt < wifi_row_len; wifi_row_cnt++)
		{
			// TIM wifi_msg解析提取
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
				if(wifi_mac[wifi_row_cnt][wifi_low_cnt] >= 'a')		//mac地址转化为大写
				{
					wifi_mac[wifi_row_cnt][wifi_low_cnt] = wifi_mac[wifi_row_cnt][wifi_low_cnt] - 0x20;
				}
			}
		}
	}
	
	return result;
}	

/* TIM-目标wifi搜索				*/
bool WIFI_Search(void)
{
	Wifi_Check_mac(1);
	return(MCU_F.b.WIFI_Search_Flag);
}
/* TIM-通过wifi建立TCP连接平台	*/
bool WIFI_Tcp_Connect(void)
{
	if(MCU_F.b.WIFI_Connected_Flag == true)
	{
		
	}
	return true;
}
/* TIM-终端设备连接客户路由器	*/
bool WIFI_Connect_Router(void)
{
	char temp_str[50]="";
	
	/*以下部分如未调整wifi配置可注释*/
	WIFI_AT_Send("CWMODE=3");	
	WIFI_AT_Send("+RST");
	/*以上部分如未调整wifi配置可注释*/
	WIFI_Generat_Str();
	
	/*等待wifi返回ok*/										//pdFALSE是否要清除
	xEventGroupWaitBits(EventGroupHandler_OTH,Wifi_Finish_Flag, pdFALSE, pdFALSE,5000);
	if(strstr(wifi_rx_buf,"OK") != NULL)
   	{
		/*WIFI_Connetced_Flag   是否使用事件组标志*/
		MCU_F.b.WIFI_Connected_Flag = true;
   	}	
   	else																	
   	{			
   		vTaskDelay(100);
		
	}
	return(MCU_F.b.WIFI_Connected_Flag);
	
}

/* TIM- wifi_msg解析提取	     	*/
void WIFI_Msg_Cmp(char (*p)[wifi_low_len])
{
	for(int i=0;i<=wifi_row_len;i++)
	{
		if(NULL != strstr(*(p+i),WIFI.wifi_ssid))
		{
			//确保ssid完全匹配
			if( *(*(p+i) + strlen(WIFI.wifi_ssid) + 1) == '\"' )
			{
				MCU_F.b.WIFI_Search_Flag = true;
			}
		}
	}
}
/* TIM- wifi连接字符串生成	     	*/
void WIFI_Generat_Str(void)
{
	send_str_to_wifi("AT+CWJAP=\"");
	send_str_to_wifi(WIFI.wifi_ssid);
	send_str_to_wifi(",");
	send_str_to_wifi(WIFI.wifi_key);
	send_str_to_wifi("\"");
	/*	视情况添加是否需要发送mac	*/
}
