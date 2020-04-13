#include "includes.h"

/* ��ѯ����1���յ���ָ��		*/
void PC_Cmd_Check(void)
{
	unsigned char i,j;

	for(i=0;i<USART_RXD_Group_Len;i++)
	{		
		if(USART1_RXD_DATA[i][USART_RXD_Line_Len-1] == 1)
		{	//���һ���ַ�Ϊ1�����ʾ�������������(�س�����ʾ���)
			PC_Cmd_Handle(USART1_RXD_DATA[i]);							//���ν���ָ��
			for(j=0;j<USART_RXD_Line_Len;j++){USART1_RXD_DATA[i][j]=0;}	//��ս�������
		}
	}
}

/* ִ�д��ڵ�ָ��				*/
void PC_Cmd_Handle(char *str)
{	
	// ����Ϊ���û��Թ���
	if(memcmp(str,"ECHO_",5)==0)
	{
		// ����PC���ڵ�MCU֮������ݻ��Թ���
		if(strcmp(str+5,"PC_MCU_ON")==0)
		{
			ECHO_F.b.PC_MCU_Flag=true;
		}
		// ����GPS��MCU֮������ݻ��Թ���
		else if(strcmp(str+5,"GPS_MCU_ON")==0)
		{
			ECHO_F.b.GPS_MCU_Flag=true;
		}	
		// ����MCU��MCU_F֮������ݻ��Թ���
		else if(strcmp(str+5,"MCU_GSM_ON")==0)
		{
			ECHO_F.b.MCU_GSM_Flag=true;
		}
		// ����GMS��MCU֮������ݻ��Թ���
		else if(strcmp(str+5,"GSM_MCU_ON")==0)
		{
			ECHO_F.b.GSM_MCU_Flag=true;
		}
		// ����WIFI��MCU֮������ݻ��Թ���
		else if(strcmp(str+5,"WIFI_MCU_ON")==0)
		{
			ECHO_F.b.WIFI_MCU_Flag = true;
		}
		// ����MCU��WIFI֮������ݻ��Թ���
		else if(strcmp(str+5,"MCU_WIFI_ON")==0)
		{
			ECHO_F.b.MCU_WIFI_Flag = true;
		}
		// ������PCת��MCU_F����
		else if(strcmp(str+5,"BYPASS_ON")==0)		
		{
			ECHO_F.b.BYPASS_GSM_Flag=true;
			ECHO_F.b.BYPASS_WIFI_Flag=false;
		}
		// ������PCת��WIFI����
		else if(strcmp(str+5,"BYPASS2_ON")==0)		
		{
			ECHO_F.b.BYPASS_GSM_Flag=false;
			ECHO_F.b.BYPASS_WIFI_Flag=true;
		}
		// �������д��ڻ��Թ���
		else if(strcmp(str+5,"ALL_ON")==0)
		{
			ECHO_F.w|=0x1F;
		}
		// �ر�PC���ڵ�MCU֮������ݻ��Թ���
		else if(strcmp(str+5,"PC_MCU_OFF")==0)
		{
			ECHO_F.b.PC_MCU_Flag=false;
		}
		// �ر�GPS��MCU֮������ݻ��Թ���
		else if(strcmp(str+5,"GPS_MCU_OFF")==0)
		{
			ECHO_F.b.GPS_MCU_Flag=false;
		}	
		// �ر�MCU��MCU_F֮������ݻ��Թ���
		else if(strcmp(str+5,"MCU_GSM_OFF")==0)
		{
			ECHO_F.b.MCU_GSM_Flag=false;
		}
		// �ر�GMS��MCU֮������ݻ��Թ���
		else if(strcmp(str+5,"GSM_MCU_OFF")==0)
		{
			ECHO_F.b.GSM_MCU_Flag=false;
		}
		// ����GMS��MCU֮������ݻ��Թ���
		else if(strcmp(str+5,"WIFI_MCU_OFF")==0)
		{
			ECHO_F.b.WIFI_MCU_Flag = false;
		}
		// �ر�MCU��WIFI֮������ݻ��Թ���
		else if(strcmp(str+5,"MCU_WIFI_OFF")==0)
		{
			ECHO_F.b.MCU_WIFI_Flag = false;
		}
		// �رմ���1ֱ�ӻ���
		else if(strcmp(str+5,"BYPASS_OFF")==0)				
		{
			ECHO_F.b.BYPASS_GSM_Flag=false;
		}
		// �ر�PCת��wifi����
		else if(strcmp(str+5,"BYPASS2_OFF")==0)		
		{
			ECHO_F.b.BYPASS_WIFI_Flag=false;
		}
		// �ر����д��ڻ��Թ���
		else if(strcmp(str+5,"ALL_OFF")==0)
		{
			ECHO_F.w = 0x00;
		}
		else
		{
			return;
		}
		Task_SPI_Flash(Flash_Write,7,&ECHO_F.w,1);												
		printf(" OK!\r\n");
	}
	// ��ȡ����
	else if(memcmp(str,"GET",3) == 0)
	{
		// ��ȡA�绰����
		if((str[3] == 'A') && (str[4] >= '1') && (str[4] <= '8'))
		{
			char *ph,*phname;
			
			USART1_SendData('A');
			
			switch(str[4])
			{
				case '1':ph=GSM_ph.temp_ph1;phname=GSM_phname.name_ph1;USART1_SendData('1');break;	
				case '2':ph=GSM_ph.temp_ph2;phname=GSM_phname.name_ph2;USART1_SendData('2');break;	
				case '3':ph=GSM_ph.temp_ph3;phname=GSM_phname.name_ph3;USART1_SendData('3');break;	
				case '4':ph=GSM_ph.temp_ph4;phname=GSM_phname.name_ph4;USART1_SendData('4');break;	
				case '5':ph=GSM_ph.temp_ph5;phname=GSM_phname.name_ph5;USART1_SendData('5');break;	
				case '6':ph=GSM_ph.temp_ph6;phname=GSM_phname.name_ph6;USART1_SendData('6');break;	
				case '7':ph=GSM_ph.temp_ph7;phname=GSM_phname.name_ph7;USART1_SendData('7');break;	
				case '8':ph=GSM_ph.temp_ph8;phname=GSM_phname.name_ph8;USART1_SendData('8');break;				
			}
			
			printf("=%s",ph);
			if(phname[0] != '\0'){printf(",%s",phname);}
			printf("\r\n");
		}
		// ��ȡB�汾
		else if(str[3] == 'B')
		{
			printf("B=%s%s\r\n",GSM_P.IMEI,HS_VER);	
		}
		// ��ȡIP��ַ
		else if(str[3] == 'C')
		{
			printf("C=%s,%s:%s\r\n",GSM_P.apn,GSM_P.ip_addr,GSM_P.ip_port);
		}
		// ��ȡ�Ƿ���GPRS
		else if(str[3] == 'D')
		{
			printf("D=%d\r\n",GSMA.b.GPRS_flag);
		}
		// ��ȡ�Ƿ�ر�GPRS
		else if(str[3] == 'E')
		{
			printf("E=%d\r\n",(GSMA.b.GPRS_flag? 0:1));
		}
		// ��ȡ����
		else if(str[3] == 'H')			
		{
			printf("H=%s\r\n",GSM_P.cmd_key);
		}
		// ��ȡGEO����
		else if((str[3] == 'I') && (str[4] >= '1') && (str[4] <= '5'))				
		{
			char set_latlog[40]={0};
			unsigned short int GEOaddress;
			
			GEOaddress = ((str[4] & 0x0F) - 1) * 40 + setgeoaddr;

			Task_SPI_Flash (Flash_Read,GEOaddress,(u8*)set_latlog,setgeo_size); 
			
			printf("I%c:%s\r\n",str[4],set_latlog);
		}
		// ��ȡ���ٱ�������
		else if(str[3] == 'J')				
		{
			printf("J=%d,%03d\r\n",GSMA.b.ALM_OverSpeed_Flag,GSM_P.set_speed);
		}
		// ��ȡBMA����
		else if(str[3] == 'K')				
		{
			switch(str[4])
			{
				case 'L':printf("KL=%03d\r\n",GSM_P.lowg);break;
				case 'H':printf("KH=%03d\r\n",GSM_P.higg);break;
			}
		}
		// ��ȡʱ������
		else if(str[3] == 'L')				
		{
			switch(str[4])		
			{
				case '4':printf("L4=%d\r\n",GSMB.b.RTC_Update_Flag);break;
				case '5':printf("L5=%d,%02d:%02d\r\n",GSMC.b.RTC_Alarm_Flag,GSM_P.RTC_Alarm_Hour,GSM_P.RTC_Alarm_Min);break;
				default :
						if(GSM_P.TimeZone&0x80)	{printf("L=+");}
						else					{printf("L=-");}
						printf("%02d:%02d\r\n",GSM_P.TimeZone&0x1F,((GSM_P.TimeZone&0x60)>>5)*15);
						break;	
			}
		}
		// ��ȡ������λʱ�估����ʱ��
		else if(str[3] == 'M')				
		{
			switch(str[4])		
			{
				case '1':printf("M1=%d,%03d%c\r\n",GSMA.b.Track_Flag,GSM_P.Track_Set_Time,GSM_P.Track_Set_Unit);break;
				case '2':printf("M2=%03d\r\n",GSM_P.heartbeatset);break;
				case '4':printf("M4=%d\r\n",GSMC.b.GPS_Tran_Mode_Flag);break;
			}
		}
		// ��ȡ�͵籨������
		else if(str[3] == 'N')				
		{
			switch(str[4])		
			{
				case '2':printf("N2=%d\r\n",GSMB.b.ALM_Bat_Full_On_Flag);break;
				default :printf("N=%d,%02d\r\n",GSMA.b.ALM_LowBat_Flag,GSM_P.set_soc&0x7F);break;
			}	
		}
		// ��ȡAPN
		else if(str[3] == 'O')				
		{
			printf("O=%s,%s\r\n",GSM_P.APN_username,GSM_P.APN_password);
		}
		// ��ȡλ������
		else if(str[3] == 'P')				
		{
			printf("P=%d,%03d\r\n",GSMA.b.ALM_Circle_GEO_Flag,GSM_P.geo_radius);
		}
		// ��ȡQ����
		else if(str[3] == 'Q')
		{
			switch(str[4])
			{
				case '1':printf("Q1=%d\r\n",GSMA.b.SOS_CALL_ENABLE_Flag);break;
				case '2':printf("Q2=%d\r\n",GSMA.b.Any_CallIn_Flag);break;
				case '3':printf("Q3=%d\r\n",GSMA.b.CallIn_ByMe_Flag);break;
				case '4':printf("Q4=%d\r\n",GSMC.b.DTMF_Det_Flag);break;
				case '5':printf("Q5=%d\r\n",GSMC.b.SMS_Check_psw_Flag);break;
			}						
		}
		// ��ȡ��������
		else if(str[3] == 'R')				
		{
			printf("R=http://%s;%s\r\n",GSM_P.domain_name,GSM_P.domain_port);
		}
		// ��ȡģʽ
		else if(str[3] == 'U')				
		{
			printf("U=%d,%d,%d,%d,%d\r\n",(GSMB.b.Callin_SpkMode_Flag? 0:1),GSMB.b.Callout_LisMode_Flag,GSMB.b.GPS_PowerOffMode_Flag,GSMB.b.GPRS_SaveMode_Flag,GSMB.b.GPS_StrongMode_Flag);
		}
		// ��ȡZָ��
		else if(str[3] == 'Z')
		{
			switch(str[4])
			{
				case '2':printf("Z2=%s\r\n",GSM_OTA.Device);break;
			}
		}
	}		
	// �����ַ�ǰ��λΪ"SET",��ֱ�Ӵ���
	else if(memcmp(str,"SET",3)==0)
	{
		analysis_sms_content(str+3, RPLY_TO_PC);	 
	}
	// ģ��SLEEP
	else if(strcmp(str,"GSM_SLEEP")==0)
	{
		printf("%s_OK\r\n",str);
		GSM_Sleep();
	}
	// ģ��WAKEUP
	else if(strcmp(str,"GSM_WAKEUP")==0)
	{
		printf("%s_OK\r\n",str);
		GSM_Wakeup();
	}
#if Wifi_Enable
	else if(strcmp(str,"TEST_WIFI") == 0)
	{
		if((Wifi_Check_mac(1) == true) && (wifi_row_cnt >=1))
		{
			vTaskDelay(100);
			printf("%s_OK\r\n",str);
		}
		else
		{
			vTaskDelay(100);
			printf("%s_NG\r\n",str);
		}
	}
#endif
	/*	���½�Ϊ���Գ�������					*/
#if DEBUG
	// ָ������BMA250,xx,yy
	else if(memcmp(str,"BMA250",6)==0)							
	{
		unsigned char add=0,data=0;
		
		if((str[6]!=',')||(str[9]!=',')||(Isxdigit(str+7,2) == false)||(Isxdigit(str+10,2) == false))								
		{
			printf("CMD_ERR!\r\n"); return;
		}	
		
		add	 = (unsigned char)Char_To_Hex(str+7, 2);
		data = (unsigned char)Char_To_Hex(str+10,2);
		BMA250_Write_Register(add,data);
		printf("BMA250,%02X,%02X_OK\r\n",add,data);
	}
	
	// ����SOS
	else if(strcmp(str,"TEST_KEY_SOS")==0)			
	{
		if(KEY_SOS_READ()) 
		{
			printf("%s_NG\r\n",str);
		}
		else
		{
			EventBits_t Event_Key;
			
			CALL.b.Calling_Flag = true;	
	
			Event_Key = xEventGroupWaitBits(EventGroupHandler_MCU, Event_Key_All, pdFALSE,pdFALSE,5000);
	
			xEventGroupClearBits(EventGroupHandler_MCU,Event_Key_All);
		
			if(Event_Key & Key_SOS_S_Flag)						
			{				
				printf("%s_OK\r\n",str);
			}
			else
			{
				printf("%s_NG\r\n",str);
			}
			
			CALL.b.Calling_Flag = false;
		}
	}
	// ���Ե�Դ��
	else if(strcmp(str,"TEST_KEY_PWR")==0)			
	{
		if(KEY_CHKPWR_READ()) 
		{
			printf("%s_NG\r\n",str);
		}
		else
		{
			EventBits_t Event_Key;
			
			CALL.b.Calling_Flag = true;	
			
			Event_Key = xEventGroupWaitBits(EventGroupHandler_MCU, Event_Key_All, pdFALSE,pdFALSE,5000);
	
			xEventGroupClearBits(EventGroupHandler_MCU,Event_Key_All);
		
			if(Event_Key & Key_Power_S_Flag)						
			{				
				printf("%s_OK\r\n",str);
			}
			else
			{
				printf("%s_NG\r\n",str);
			}
			
			CALL.b.Calling_Flag = false;	
		}
	}
	// ����CALL��
	else if(strcmp(str,"TEST_KEY_CALL1")==0)			
	{
		if(KEY_CALL1_READ()) 						// ������·
		{
			printf("%s_NG\r\n",str);
		}
		else
		{
			EventBits_t Event_Key;
			
			CALL.b.Calling_Flag = true;
	
			Event_Key = xEventGroupWaitBits(EventGroupHandler_MCU, Event_Key_All, pdFALSE,pdFALSE,5000);
	
			xEventGroupClearBits(EventGroupHandler_MCU,Event_Key_All);
		
			if(Event_Key & (Key_Call1_S_Flag|Key_Call1_L_Flag))						
			{				
				printf("%s_OK\r\n",str);
			}
			else
			{
				printf("%s_NG\r\n",str);
			}
			
			CALL.b.Calling_Flag = false;			
		}
	}
	// ����CALL2��
	else if(strcmp(str,"TEST_KEY_CALL2")==0)			
	{
		if(KEY_CALL1_READ()) 						// ������·
		{
			printf("%s_NG\r\n",str);
		}
		else
		{
			EventBits_t Event_Key;
			
			CALL.b.Calling_Flag = true;
	
			Event_Key = xEventGroupWaitBits(EventGroupHandler_MCU, Event_Key_All, pdFALSE,pdFALSE,5000);
	
			xEventGroupClearBits(EventGroupHandler_MCU,Event_Key_All);
		
			if(Event_Key & (Key_Call2_S_Flag|Key_Call2_L_Flag))						
			{				
				printf("%s_OK\r\n",str);
			}
			else
			{
				printf("%s_NG\r\n",str);
			}
			
			CALL.b.Calling_Flag = false;			
		}
	}
	// ����CSQ
	else if(strcmp(str,"TEST_CSQ")==0)				
	{
		Gsm_Get_Parameters(0x80);
		GSM_Sleep();
		printf("CSQ=%d\r\n",gpsx.sig_strength);
	}
	// �����ǰ�����ٷֱ�
	else if(strcmp(str,"TEST_CBC")==0)
	{
		Gsm_Get_Parameters(0x80);		
		printf("CBC=%d%%\r\n",gpsx.soc);
	}
	// ��ȡ���״̬
	else if(strcmp(str,"TEST_CHARGE")==0)			
	{
		if(CHK_CH() == RESET)						// �����
		{
			printf("%s_ON\r\n",str);
		}
		else										// ������
		{
			printf("%s_OFF\r\n",str);
		}
	}
	// ����W25����
	else if(strcmp(str,"TEST_W25")==0)				
	{
		Task_SPI_Flash(Flash_Read_ID,0,0,0);
	}
	// ���Է�����
	else if(strcmp(str,"TEST_SOUND")==0)
	{
		SOUND_BI(BIBI);
		printf("%s_OK\r\n",str);
	}
	// �������
	else if(strcmp(str,"TEST_MOTOR_ON")==0)
	{
		motor_on();
		printf("%s_OK\r\n",str);
	}
	// �ر����
	else if(strcmp(str,"TEST_MOTOR_OFF")==0)
	{
		motor_off();
		printf("%s_OK\r\n",str);
	}
	// ����BMA DTAP
	else if(strcmp(str,"TEST_BMA")==0)
	{
		EventBits_t Event_BMA;
		unsigned char i = 0;
		
		BMA250_Clear_INT();							// ����ж�
		
		Event_BMA = xEventGroupWaitBits(EventGroupHandler_MCU,BMA250_Int_Flag, pdFALSE,pdFALSE,5000);

		xEventGroupClearBits(EventGroupHandler_MCU,BMA250_Int_Flag);
			
		if(Event_BMA & BMA250_Int_Flag)				// ���ж�
		{
			if(BMA250_Is_SlopINT() == true)			// �����˫���ж�
			{
				while((BMA250_INT_READ() == 0)&&(i++ < 250))
				{
					BMA250_Clear_INT();				// ����ж�
					vTaskDelay(10);
				}
							
				if(BMA250_INT_READ() == 1)			// ����жϳɹ�
				{
					i=254;
					printf("%s_OK\r\n",str);
				}
			}
		}
		
		if(i != 254)								// ����ʧ��
		{
			printf("%s_NG\r\n",str);
		}
	}
	
#endif	
	// ����GSM������
	else if(memcmp(str,"GSM=",4)==0)
	{
		if(Isdigit(str+4,strlen(str)-4))
		{
			MX_LPUART1_UART_Init(Char_To_Dig(str+4,strlen(str)-4));
			printf("%s_OK\r\n",str);
		}
	}
	// ָ��򿪹���
	else if(strcmp(str,"SPK_ON")==0)
	{
		printf("%s_OK\r\n",str); Speak_Amp_On();
	}
	// ָ��رչ���
	else if(strcmp(str,"SPK_OFF")==0)
	{
		printf("%s_OK\r\n",str); Speak_Amp_Off();
	}
	// ���������PC����ת��MCU_F
	else if(ECHO_F.b.BYPASS_GSM_Flag==true)
	{	
		if(memcmp(str,"$PMTK",5)==0)
		{
			GPS_Force_On();
			HAL_UART_Transmit(&huart2,(uint8_t*)str,strlen((char*)str),1000);
			USART2_SendData('\r');USART2_SendData('\n');
		}
		else
		{
#if MODULE_SIM7500
			char *p1;
#endif
			
			GSM_Wakeup();
			clr_gsm_buf();
			HAL_UART_Transmit(&hlpuart1,(uint8_t*)str,strlen((char*)str),1000);
			LUSART1_SendData('\r');LUSART1_SendData('\n');
	
#if MODULE_SIM7500
			p1 = strstr(str,"+CSENDDATA2AP");	// AT+CSENDDATA2AP="/data/media/Call.wav",����
			if(p1 != NULL)
			{				
				p1 = strstr(p1+20,"\"");
				Aduio_Cnt = 0;Audio_Length = Char_To_Dig(p1+2,strlen(p1+2));
				MCU_F.b.Uart1_to_Luart1 = true;	
			}
#endif
		}	
	}
#if Wifi_Enable
	else if(ECHO_F.b.BYPASS_WIFI_Flag == true)
	{
		WIFI_on();
		HAL_UART_Transmit(&huart4,(uint8_t*)str,strlen((char*)str),1000);
		USART4_SendData('\r');USART4_SendData('\n');						//���WIFI����͸��ģʽ�˳���Ҫ����
	}
#endif
	else
	{
		;
	}
}
