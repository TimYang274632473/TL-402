#include <includes.h>

//�豣�浽SPIFLASH�Ĳ���
GSM_A_def GSMA;
GSM_B_def GSMB;
GSM_C_def GSMC;
GSM_D_def GSMD;

GSM_para 		GSM_P;
GSM_phone 		GSM_ph;
GSM_phonename	GSM_phname;

//������ʱ��־ 
GSM_sms_def  	SMS;
GSM_call_def	CALL;
GSM_gprs_def 	GPRS;

char *_ptcmdx;
char socalm[12];
char gsm_rx_buf[gsm_rx_len];  

#if MODULE_SIM7500
unsigned short int gsm_rx_sta=0;						// USART1�������ݴ��������λ��
#else
unsigned char gsm_rx_sta=0;								// USART1�������ݴ��������λ��
#endif
unsigned char gsm_rx_req=0;								// USART1Ҫ����յ��ַ�����

#define company1 "TOPLOVO INDUSTRIAL CO., LTD\nCHINA SHENZHEN HENGYUETONG ELECTRONICS CO., LTD\nTL403,Copyright !"
		
#if CALLIN_ENABLE
unsigned char Ring_Clip_Count=0;						// �����������
#endif
	
u8  gsmerr=0;

unsigned char network=0;								// 2G����, 3G����, 4G����

signed char temperature=0;

unsigned char SMS_NUM=0;								// ���ڱ�����յ��Ķ��Ŵ洢λ��

char SMS_Phone_Number[24];
char SMS_Phone_Number_Backup[24],Ring_Phone_Number[20];	// �洢�������

	
/* �ж��ַ��Ƿ�Ϊ����		*/
bool Isdigit(char *Str,unsigned char Num)
{
	unsigned char i;
	
	for(i=0;i<Num;i++)
	{
		if(!isdigit(Str[i])) return false;
	}
	
	return true;
}

/* �ж��Ƿ�Ϊʮ�������ַ�	*/
bool Isxdigit(char *Str,unsigned char Num)
{
	unsigned char i;
	
	for(i=0;i<Num;i++)
	{
		if(!isxdigit(Str[i])) return false;
	}
	
	return true;
}

/* ���ַ�����ʮ����ת��		*/
unsigned int Char_To_Dig(char *Str,unsigned char Num)
{
	unsigned char i;
	unsigned int Result=0;
	
	for(i=0;i<Num;i++)
	{
		Result = (Result * 10) + (Str[i] & 0x0F);
	}
	
	return Result;
}

/* ���ַ�����ʮ������ת��		*/
unsigned int Char_To_Hex(char *Str,unsigned char Num)
{
	unsigned char i;
	unsigned int Result=0;
	
	for(i=0;i<Num;i++)
	{
		if((Str[i]>='0')&&(Str[i]<='9'))
		{
			Result = (Result * 16) + (Str[i] & 0x0F);
		}
		else if((Str[i]>='a')&&(Str[i]<='f'))
		{
			Result =  (Result * 16) + (Str[i] - 'a' + 10);
		}
		else
		{
			Result =  (Result * 16) + (Str[i] - 'A' + 10);
		}
	}
	
	return Result;
}

/*���gsm_rx_buf�� gsm_rx_sta����*/
void clr_gsm_buf(void)	
{	
	gsm_rx_sta = 0; 
	
	for(u8 i=0;i<gsm_rx_len;i++){gsm_rx_buf[i]=0;}

	xEventGroupClearBits(EventGroupHandler_OTH,Event_Gsm_All);
}

/*��gsm_rx_buf�в����ַ���,�ҵ�����1��δ�ҵ�����0*/
u8 find_str(char *a)								 
{ 
	return strstr(gsm_rx_buf,a)==NULL? 0:1;
}

/*���͵����ֽ�*/
void uartTxByte (UART_HandleTypeDef *huart,uint8_t ucData)
{
#if MODULE_SIM7500
	if(MCU_F.b.Uart1_to_Luart1) {return;}
#endif
	
	if(ECHO_F.b.MCU_GSM_Flag == ON)						// �����Ҫ��ATָ����Ե�PC��
	{
		HAL_UART_Transmit(&huart1,&ucData,1,1000);
	}
	
	HAL_UART_Transmit(huart,&ucData,1,1000);
}

/*�����ַ���*/
void send_str_to_gsm (char *pStr)
{
#if MODULE_SIM7500
	if(MCU_F.b.Uart1_to_Luart1) {return;}
#endif	
	
	if(ECHO_F.b.MCU_GSM_Flag == ON)						// �����Ҫ��ATָ����Ե�PC��
	{
		HAL_UART_Transmit(&huart1,(uint8_t*)pStr,strlen((char*)pStr),1000);
	}
	
	HAL_UART_Transmit(&hlpuart1,(uint8_t*)pStr,strlen((char*)pStr),1000);
}

/*�Ƚ�������룬һ�·���1�����򷵻�0*/
unsigned char Compare_phone_number(char*_mm)	
{	
	unsigned char In;
	
	if(*_mm == 0){return 0;}
	
	In = strlen(_mm);
	
	if(In > 5)		{In = In / 2;	}
	else			{In = 0;		}
						
	if((GSM_ph.temp_ph1[0] != '\0') && (strstr(GSM_ph.temp_ph1,_mm+In) != NULL))
	{
		return 1;	
	}
	
	if((GSM_ph.temp_ph2[0] != '\0') && (strstr(GSM_ph.temp_ph2,_mm+In) != NULL))
	{
		return 2;	
	}
	
	if((GSM_ph.temp_ph3[0] != '\0') && (strstr(GSM_ph.temp_ph3,_mm+In) != NULL))
	{
		return 3;	
	}
	
	if((GSM_ph.temp_ph4[0] != '\0') && (strstr(GSM_ph.temp_ph4,_mm+In) != NULL))
	{
		return 4;	
	}

	if((GSM_ph.temp_ph5[0] != '\0') && (strstr(GSM_ph.temp_ph5,_mm+In) != NULL))
	{
		return 5;	
	}
	
	if((GSM_ph.temp_ph6[0] != '\0') && (strstr(GSM_ph.temp_ph6,_mm+In) != NULL))
	{
		return 6;	
	}
	
	if((GSM_ph.temp_ph7[0] != '\0') && (strstr(GSM_ph.temp_ph7,_mm+In) != NULL))
	{
		return 7;	
	}
	
	if((GSM_ph.temp_ph8[0] != '\0') && (strstr(GSM_ph.temp_ph8,_mm+In) != NULL))
	{
		return 8;	
	}
	
	return 0;
}	

/*sstr:�����ַ��� rcount:���յ��ַ����� rtime:���ճ�ʱʱ�� atcount:ִ�д��� rstrtype:ATָ���� return:true�ɹ�,falseʧ��*/
bool GSM_AT_Send(char *sstr,u16 rcount,u8 rtime,u8 atcount,u32 rstrtype)
{  
	u8 i;
	char *p1;
	
	for(i=0;i<atcount;i++)
	{
		clr_gsm_buf();gsm_rx_req = rcount;

		MCU_F.b.Chk_AT_Cnt_Flag = true; GSM_Wakeup();
		
		send_str_to_gsm("AT"); send_str_to_gsm(sstr); send_str_to_gsm("\r\n");	
		
		xEventGroupWaitBits(EventGroupHandler_OTH,AT_Cnt_OK_Flag, pdFALSE, pdTRUE,(rtime * 1000));
					
		switch(rstrtype)
		{	
			case 0x0002: 	// ���Ͷ���															
				p1=strstr(gsm_rx_buf,">");         					 
				break;
			
			case 0x0003:	// ����GPRS			
#if MODULE_UG96
				p1=strstr(gsm_rx_buf,"+QIRD:");
				
				if(p1 != NULL)
				{
					if(*(p1+6) == '0')					
					{
						return false;
					}
				
					vTaskDelay(10);
				}	
#elif MODULE_SIM7500
				p1=strstr(gsm_rx_buf,"+CIPRXGET");

				if(p1 != NULL)
				{
					vTaskDelay(10);
				}
#endif
				break;
						
			case 0x0004:	// ���Ͷ���															
				p1=strstr(gsm_rx_buf,"+CMGS"); 
				break;	
#if MODULE_SIM7500			
			case 0x0005:	// 4G����
				p1=strstr(gsm_rx_buf,"+CNSMOD");
				if(p1 != NULL)
				{
					if(!strstr(gsm_rx_buf,"OK"))
					{
						vTaskDelay(2);
					}	
				}
				break;
		
			case 0x0006:	// ������Ƶ
				if(strstr(gsm_rx_buf,"ERROR") != NULL)
				{	
					return false;	
				}
				
				vTaskDelay(2);
				
				p1 = strstr(gsm_rx_buf,"wav play");	
				break;
#if GSM_Positon				
			case 0x0007:	// ��վ��λ
				p1=strstr(gsm_rx_buf,"+CLBS:");
				if(p1 != NULL)
				{
					char *p2;

					vTaskDelay(2);
					
					p1 = strstr(p1,",");
					
					p1++;
					
					p2 = strstr(p1,",");
					
					*p2 = '\0';p2++;
										
					strtok(p2+1,",");

					if(*p1 == '-')	{gpsx.nshemi = 'S';p1++;}
					else			{gpsx.nshemi = 'N';		}
					
					gpsx.latitude = atof(p1) * 10000000;
					
					if(*p2 == '-')	{gpsx.ewhemi = 'W';p2++;}
					else			{gpsx.ewhemi = 'E';		}	
					
					gpsx.longitude = atof(p2) * 10000000;
					
					gpsx.AV_indicate = 'V';					
				}
	
				break;
#endif				
			case 0x0008:	// ��ѯ����״̬
				p1=strstr(gsm_rx_buf,"+CIPOPEN: 0,0"); 															
				break;
#endif
			
			case 0x0010:	// ��ѯ���ͽ��															
#if MODULE_UG96
				p1=strstr(gsm_rx_buf,"+QHTTPGET:"); 
#elif MODULE_SIM7500
				p1=strstr(gsm_rx_buf,"+HTTPACTION");
#endif
				break;

			case 0x0011:	// ��ѯHTTPHEAD
				p1=strstr(gsm_rx_buf,"+HTTPHEAD");
				break;
			
			case 0x0012:	// ��ѯHTTPREADFILE
				p1=strstr(gsm_rx_buf,"+HTTPREADFILE");
				break;
			
			case 0x0020:	// CREG															
				p1=strstr(gsm_rx_buf,"+CREG:"); 
				if(p1 != NULL)
				{
					if(strstr(gsm_rx_buf,"OK") == NULL)
					{
						vTaskDelay(10);
					}
				}
				break;
			
			case 0x0040:	// ��ѯAP���ý��
#if MODULE_SIM7500
				p1=strstr(gsm_rx_buf+17,"MDM"); 
#endif	
				break;

			case 0x0080:	// ��ѯ����
				p1=strstr(gsm_rx_buf,"OK");									// �����Ƿ�OK,���ж��Ƿ�ΪREAD
			
				if((p1 != NULL)&&(gsm_rx_sta<=6))							// ֻ�ҵ�OK
				{
					gsmerr=0;return false;
				}
				else
				{
					vTaskDelay(rtime * 1000);
					p1=strstr(gsm_rx_buf,"READ");	 
				}					
				break;
				
			case 0x0100:	// ATA
				
#if MODULE_UG96
		
				p1=strstr(gsm_rx_buf,"OK");
			
				if(p1 == NULL)
				{
					vTaskDelay(10);
					
					if(strstr(gsm_rx_buf,"ERROR")) 
					{
						gsmerr=0;return false;
					}
				}
				
#elif MODULE_SIM7500
			
				p1=strstr(gsm_rx_buf,"VOIC");
				
				vTaskDelay(10);
			
				if(p1 == NULL)
				{					
					if(strstr(gsm_rx_buf,"ERROR") || strstr(gsm_rx_buf,"NO CARRIER")) 
					{
						gsmerr=0;return false;
					}
				}				

#endif			
				break;

			case 0x0200:	// ��ȡCOPS
				p1=strstr(gsm_rx_buf,"+COPS:");
				if(p1 != NULL)
				{
					if(strstr(gsm_rx_buf,"OK") == NULL)
					{
						vTaskDelay(10);
					}
				}		
				break;
				
			case 0x0400:   // HTTP����
				p1=strstr(gsm_rx_buf,"CONNECT");	      
				break;	

			case 0x0800:	// ok or ERROR
				p1=strstr(gsm_rx_buf,"OK");
				if(p1==NULL)
				{
					vTaskDelay(10);
					p1=strstr(gsm_rx_buf,"ERR");
				}
				break;
				
			case 0x1000:	// CLCC
				p1=strstr(gsm_rx_buf,"+CLCC:");
				break;
			
			case 0x2000:	// 

				break;
			
			case 0x4000:	// ��ѯ����״̬														
#if MODULE_UG96
				p1=strstr(gsm_rx_buf,"OK");
			
				if(p1 != NULL)
				{
					gsmerr=0;return false;
				}
				else
				{
					vTaskDelay(10);
					
					if(strstr(gsm_rx_buf,"ERROR") != NULL)
					{
						gsmerr=0;return false;
					}
					else
					{
						u8 *p2,posx;
						
						p2 = (u8*)strstr(gsm_rx_buf,"QISTATE");	
						
						posx = NMEA_Comma_Pos(p2,5);
						
						if(posx != 0xFF)
						{
							if(*(p2+posx) == '3')
							{
								return true;
							}
						}
					}		
				}
#elif MODULE_SIM7500				
				p1=strstr(gsm_rx_buf,"+CIPOPEN: 0");
			
				if(p1 != NULL)
				{
					vTaskDelay(15);
					p1=strstr(gsm_rx_buf,",-1");
				}			
#endif
				break;
				
			case 0x8000:	// ��ȡCSQ
				p1=strstr(gsm_rx_buf,"+CSQ:");
				if(p1 != NULL)
				{
					if(strstr(gsm_rx_buf,"OK") == NULL)
					{
						vTaskDelay(10);
					}
				}		
				break;
				
			case 0x8001:	// �¶�
#if MODULE_UG96
				p1=strstr(gsm_rx_buf,"+QTEMP");
#elif MODULE_SIM7500
				p1=strstr(gsm_rx_buf,"+CPMUT");
#endif			
				if(p1 != NULL)
				{
					if(strstr(gsm_rx_buf,"OK") == NULL)
					{
						vTaskDelay(10);
					}
				}
				break;
			
			case 0x8002:	// ����
				p1=strstr(gsm_rx_buf,"+CBC:");
			
				if(p1 != NULL)
				{
					if(strstr(gsm_rx_buf,"OK") == NULL)
					{
						vTaskDelay(10);
					}
				}	
				break;
				
			default:																// ��ѯ����Ƿ�OK
				p1=strstr(gsm_rx_buf,"OK");	       
				break;
		}

		if(p1!=NULL)																// ��ȡOK 
		{						
			gsmerr=0;return true;
		}
		else																		// ��ȡNG
		{
			if(gsm_rx_sta==0)														// ���ģ���Ƿ���������
			{
#if MODULE_SIM7500
				if(MCU_F.b.Uart1_to_Luart1) {gsmerr = 0;	}
#endif		
				if((gsmerr++)>5){Task_SPI_Flash(Flash_MCU_Reset,0,0,0);	} 
			}
			else
			{
				if((strstr(gsm_rx_buf,"AT+") != NULL) || (strstr(gsm_rx_buf,"AT\r") != NULL)){vTaskDelay(10);send_str_to_gsm("ATE0\r\n");}
			}
			
			vTaskDelay(10+5*i);
		}
	}
	
	return false;
}

/* ���ò����� */ 
void ATE0_Test(void)
{	
	if(!__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))								// �����������
	{
		vTaskDelay(200);GSMON_H();vTaskDelay(500);GSMON_L();vTaskDelay(2000);// ����MCU_F
	}

#if MODULE_SIM7500
	if(GSM_AT_Send("E0",6,5,5,0x0001) == false)								// ��9600������,�������ʧ��
#else	
	if(GSM_AT_Send("E0",6,2,5,0x0001) == false)								// ��9600������,�������ʧ��
#endif
	{
		bool reset = false;
		
		vTaskDelay(200);GSMON_H();vTaskDelay(500);GSMON_L();vTaskDelay(2000);// ����MCU_F
									
		MX_LPUART1_UART_Init(9600);										

#if MODULE_SIM7500
		if(GSM_AT_Send("+IPREX=115200",21,5,2,0x0001) == false)
#else		
		if(GSM_AT_Send("+IPR=115200",6,5,2,0x0001) == false)		
#endif			
		{
			if(++GSM_P.ATE0_Fail_Cnt <= 3)
			{
				reset = true;
			}
			else
			{
				GSM_P.ATE0_Fail_Cnt=0;
			}
			
			MCU_F.b.No_Simcard_Flag = true;
		}
		else
		{
			GSM_P.ATE0_Fail_Cnt=0;
			MX_LPUART1_UART_Init(115200);
#if MODULE_SIM7500
			GSM_AT_Send("E0",6,5,2,0x0001);						// ��9600������,�������ʧ��
#else	
			GSM_AT_Send("E0",6,2,2,0x0001);						// ��9600������,�������ʧ��
			GSM_AT_Send("&W",6,2,2,0x0001);	
#endif
		}
		
		Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);
		
		if(reset == true)
		{
			Task_SPI_Flash(Flash_MCU_Reset,0,0,0);;	
		}
	}
}

/* ��ѯSIM״̬	*/
bool CPIN_P(void)
{
	for(u8 i=0;i<3;i++)
	{
		GSM_AT_Send("+CPIN?",9,2,1,0x0001);
		
		vTaskDelay(10);
			
		if(strstr(gsm_rx_buf,"READY")) 		{return true;	}
		
		vTaskDelay(3000);
	}
	
	return false;
}

/* ģ���ʼ������*/
void GSM_init(void)
{
	char*p1;
	u8 i;	
	
	GSM_AT_Send("+CMEE=0",6,2,10,0x0001);
	
	if(MCU_F.b.Imei_Err_Flag)
	{
		GSM_AT_Send("+CGSN",25,2,2,0x0001);									// ��ȡIMEI��					 	
	
		for(i=0;i<10;i++) 
		{
			if(isdigit(gsm_rx_buf[i]))
			{
				gsm_rx_buf[i+15]=0;break;
			}
		}

		snprintf(GSM_P.IMEI,16,"%s",gsm_rx_buf+i);							// ��IMEI�Ŵ���IMEI������

		if(Isdigit(GSM_P.IMEI,15) == false)									// ���IMEI���Ƿ�Ϊ���֣��粻�����¶�ȡ
		{
			Task_SPI_Flash(Flash_MCU_Reset,0,0,0);;	
		}
		else
		{
			Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);		
		}			
	}
				
	if(CPIN_P() == false)													// ���SIM����װ�Ƿ�OK,�޿�ֱ�ӽ���ϵͳ
	{		
		GSM_AT_Send("+CFUN=0",33,5,1,0x0001);
			
		GSM_AT_Send("+CFUN=1",6,5,1,0x0001);
		
		if(CPIN_P() == false)
		{
			MCU_F.b.No_Simcard_Flag = true;return;
		}
		
		GSM_AT_Send("+CMEE=0",6,2,10,0x0001);
	}
	
#if MODULE_UG96
		
	GSM_AT_Send("+CREG=2",6,2,10,0x0001);									// �ر�SIM�ϱ���Ϣ	
	
	for(i=0;i<100;i++)														// ��ѯע����
	{
		GSM_AT_Send("+CREG?",30,2,1,0x0020);				
		p1=strstr(gsm_rx_buf,"+CREG");	
		if((*(p1+9)!=0x31)&&(*(p1+9)!=0x35))	
		{
			vTaskDelay(5000);
		}
		else 
		{
			break;
		}
		if(i>12) 
		{
			GSM_AT_Send("+CFUN=1,1",6,2,10,0x0001);							// ����MCU_Fģ��
			Task_SPI_Flash(Flash_MCU_Reset,0,0,0);							// ע��30��δ�ɹ�,��λ
		}
	}
	
	Task_SPI_Oled(Oled_Preogress,"GSM Init Now","",20,0);						// LED��ʾ����
	
	GSM_AT_Send("+CREG=0",6,3,5,0x0001);									// �ر��ϱ�
	
	GSM_AT_Send("+QSCLK=1",6,3,2,0x0001);									// ������ʱ��
	
	GSM_AT_Send("+CFUN=1",6,3,2,0x0001);									// 

	GSM_AT_Send("+QDAC=4",6,3,5,0x0001);									// ����PCM
	
	GSM_AT_Send("+QCFG=\"nwscanmode\",0,1",6,4,2,0x0001);					// �����Զ�����ģʽ
			
	vTaskDelay(1000);
	
	GSM_AT_Send("+CLIP=1",6,2,2,0x0001);
	
	GSM_AT_Send("+CMGF=1",6,2,2,0x0001);									// ѡ����Ÿ�ʽ--�ı�ģʽ
	
	if(GSM_AT_Send("+CSDH=0",6,2,2,0x0001) == false){Task_SPI_Flash(Flash_MCU_Reset,0,0,0);}		// �ı�ģʽ����ʾ���Ӳ���
	
    GSM_AT_Send("+CNMI=2,1,0,0,0",6,2,2,0x0001);
	
	GSM_AT_Send("+CSCS=\"IRA\"",6,2,2,0x0001);
	
	GSM_AT_Send("+CSMP=17,255,0,0",6,2,5,0x0001);

#if !DEBUG	
	GSM_AT_Send("+CGCLASS=\"B\"",6,60,2,0x0001);
#endif
			
	snprintf(dtbuf,12,"+CLVL=%d",GSM_P.spk_volume*10);		
	GSM_AT_Send(dtbuf,6,2,2,0x0001); 	
	
	GSM_AT_Send("+QCFG=\"uart/power\",1,2",6,2,2,0x0001);
	
	GSM_AT_Send("+QCFG=\"uart/power\",0,0",6,2,2,0x0001);
	
	GSM_AT_Send("+QCFG=\"urc/delay\",1",6,2,2,0x0001);
	
	GSM_AT_Send("+QAUDMOD=2",6,2,2,0x0001);
	
	GSM_AT_Send("+QAUDGAIN=2,250,60",6,2,2,0x0001);
		
	GSM_AT_Send("+QSIDET=-200",6,2,2,0x0001);
	
	GSM_AT_Send("+QAUDCFG=\"handfree/eec\"",180,2,2,0x0001);
	
	GSM_AT_Send("+CMEE=0",6,2,10,0x0001);									// �ر��ϱ�ERR��
	
	GSM_AT_Send("+IFC=0,0",6,2,2,0x0001);
	
	GSM_AT_Send("+CRSL=90",6,2,2,0x0001);									// ��������

#if !DEBUG
	GSM_AT_Send("+QSTK=1",6,2,2,0x0001);									// ����������Ӫ��
	
	GSM_AT_Send("+QSTKRSP=33,0",6,2,2,0x0800);								// �ر�������Ӫ���ϱ���Ϣ
		
	if(GSM_AT_Send("+COPS=0,2",6,150,1,0x0001) == false)					// ���ע��ʧ��,��ʱ3S
	{
		vTaskDelay(30000);
	}
	
	if(GSM_AT_Send("+CGREG?",21,150,1,0x0001) == false)						// ���ע��ʧ��,��ʱ3S
	{
		vTaskDelay(30000);
	}
#endif
	
#elif MODULE_SIM7500
	
	GSM_AT_Send("+CSCLK=1",6,2,2,0x0001);									// ����ͨ��DTR���ſ���MCU_F˯�߼�����
			
	GSM_AT_Send("+CREG=1",6,2,10,0x0001);									// �ر�SIM�ϱ���Ϣ	
	
	for(i=0;i<200;i++)														// ��ѯע����
	{
		GSM_AT_Send("+CREG?",20,2,1,0x0001);				
		p1=strstr(gsm_rx_buf,"+CREG");	
		if((*(p1+9)!=0x31)&&(*(p1+9)!=0x35))	
		{
			vTaskDelay(1000);
		}
		else 
		{
			break;
		}
			
		if(i == 20) 
		{
			snprintf(dtbuf,50,"+CGDCONT=1,\"IP\",\"%s\"",GSM_P.apn);
			GSM_AT_Send(dtbuf,6,5,1,0x0001);
			GSM_AT_Send("+CFUN=0",33,5,1,0x0001);
			GSM_AT_Send("+CFUN=1",6,5,1,0x0001);
		}
		
		if(i >= 100)
		{
			MCU_F.b.No_Simcard_Flag = true;return;
		}
	}
	
	GSM_AT_Send("+CREG=0",6,2,5,0x0001);
	
	if(GSM_AT_Send("+CSDH=0",6,2,2,0x0001) == false){Task_SPI_Flash(Flash_MCU_Reset,0,0,0);}	// �ı�ģʽ����ʾ���Ӳ���
	
	GSM_AT_Send("+CLIP=1",6,2,2,0x0001);
	
	GSM_AT_Send("+CMGF=1",6,2,2,0x0001);									// ѡ����Ÿ�ʽ--�ı�ģʽ
	
	GSM_AT_Send("+CNMI=2,1,0,0,0",6,2,2,0x0001);
	
	GSM_AT_Send("+CSMP=17,255,0,0",6,5,5,0x0001);

	GSM_AT_Send("+CSCS=\"IRA\"",6,2,2,0x0001);
	
	GSM_AT_Send("+CGCLASS=?",25,2,2,0x0001);
	
	GSM_AT_Send("+CSUB",58,2,2,0x0040);
	
	CSDVC_P();
			
	GSM_AT_Send("+CLVL=5",6,2,2,0x0001); 									// ͨ������

	GSM_AT_Send("+COUTGAIN=8",6,10,2,0x0001);

	GSM_AT_Send("+CIPRXGET=1",6,2,2,0x0001);

#if GSM_Positon
	GSM_AT_Send("+CLBSCFG=1,3,\"lbs-simcom.com:3002\"",6,2,2,0x0001);
#endif

#if !DEBUG
	if(GSM_AT_Send("+COPS=3,2",6,150,1,0x0001) == false)					// ���ע��ʧ��,��ʱ3S
	{
		vTaskDelay(30000);
	}
#endif
	
#endif
}	


/*��ȡ�������źš���վ����Ӫ����Ϣ,flag1==1ֻ��ȡ�ź�, flag1==1��ȡCOPS��flag1==2��ȡȫ��,���λΪ1����ִ��*/
void Gsm_Get_Parameters(u8 flag1)
{
	char *p1;
	unsigned char i,j;
	unsigned int  tmp=0;
	
#if !DEBUG
	char *p2,*str;	
#endif	
		
	if(!(flag1&0x80))
	{
		if((cnt_1s.Track_SOC_CSQ_Cnt >= 60) || (gpsx.soc == 0) || (gpsx.sig_strength == 0) || (gpsx.sig_strength >= 32))
		{
			cnt_1s.Track_SOC_CSQ_Cnt = 0;
		}
		else
		{
			return;	
		}
	}
		
	flag1 = flag1 & 0x7F;

	for(i=0;i<3;i++)
	{
		if(GSM_AT_Send("+CBC",22,2,1,0x8002) == true)
		{
#if MODULE_UG96
			p1 = strstr(gsm_rx_buf,",");p1++;
			p1 = strstr(p1,",");
			if(p1 != NULL)
			{
				p1++;strtok(p1,"\r");
				tmp = atoi(p1);
				if(tmp < 3450){tmp = 3450;}
				if(tmp > 4050){tmp = 4050;}
				j =(unsigned char)((tmp-3450) / 6);	
			}
#else		
			p1 = strstr(gsm_rx_buf,"+CBC:");			
			strtok(p1,"V");
			tmp = (unsigned int)(atof(p1+6) * 1000);
			if(tmp < 3450){tmp = 3450;}
			if(tmp > 4050){tmp = 4050;}
			j =(unsigned char)((tmp-3450) / 6);		
#endif		
			
			if(j > 100)	{j = 100;	}
			
			if(MCU_F.b.First_Read_CBC_Flag == false)						// ����ǿ�����һ�λ�ȡ����,�����Ϊ0
			{
				MCU_F.b.First_Read_CBC_Flag = true;
				
				Bat_Val[0] = Bat_Val[1] = Bat_Val[2] =  gpsx.soc = j;
			}
			else
			{
				Bat_Val[Bat_Cnt]=j;											// ��¼����
				
				if(Bat_Val[0] >= Bat_Val[1])
				{
					if(Bat_Val[1] >= Bat_Val[2])	
					{
						j = Bat_Val[1];
					}
					else
					{
						if(Bat_Val[0] >= Bat_Val[2])
						{
							j = Bat_Val[2];
						}
						else
						{
							j = Bat_Val[0]; 
						}
					}
				}
				else
				{
					if(Bat_Val[1] <= Bat_Val[2])	
					{
						j = Bat_Val[1];
					}
					else
					{
						if(Bat_Val[0] <= Bat_Val[2])
						{
							j = Bat_Val[2];
						}
						else
						{
							j = Bat_Val[0]; 
						}
					}
				}
				
				if(++Bat_Cnt >= 3)											// ��ȡ�����ε�������ƽ��ֵ
				{
					Bat_Cnt=0;
				}
				
				if((MCU_F.b.Charging_Flag == false) && (j < gpsx.soc))
				{
					gpsx.soc = j;
				}
				else if((MCU_F.b.Charging_Flag == true) && (j > gpsx.soc))
				{
					gpsx.soc = j;
				}
			}
					
			if((gpsx.soc <= 2) && (MCU_F.b.Charging_Flag == false))											
			{
				Task_AT_GSM(Gsm_Pwr_off,0,0,0,0,0,0);
			}
										
			if(GSMA.b.ALM_LowBat_Flag)														// ����ѿ����͵��⹦��
			{	
				if(gpsx.soc<(GSM_P.set_soc&0x7F))											// δ���͹��͵籨��
				{
					if(MCU_F.b.Charging_Flag == false){SOUND_BI(BIBI);}
					
					if(!(GSM_P.set_soc&0x80))												// ��7λ��Ϊ����ֵ,�Ѵ�͵籨����ֵ
					{							
						GSM_P.set_soc|=0x80;												
						Task_SPI_Flash(Flash_Write,gpsadd,(u8*)&gpsx,gpsx_All_Size);		
						Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);	
						snprintf(socalm,22,"LOW BAT!%d%%",gpsx.soc);	
						Task_AT_GSM(Gprs_Send_Msg,0,0,ALM_D,OLDGPS,MSG_GPRS,1);	
						Task_AT_GSM(Gsm_Send_SMS,GSM_ph.temp_ph1,socalm,0,NOGPS,MSG_NO,0);						
					}
				}
				else																		// ����ѷ����͵籨��
				{
					if(gpsx.soc>((GSM_P.set_soc&0x7F)+20))									
					{
						GSM_P.set_soc&=0x7f;												
						Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);	
					}
				}				
			}
			
#if Bat_Full_ON	
			
			if((GSMB.b.ALM_Bat_Full_On_Flag)&&(GSMB.b.ALM_Bat_Full_Send_Flag))				// ����ѿ��������籨�����ѷ��ͱ�����Ϣ
			{
				if(gpsx.soc < 60)															
				{
					GSMB.b.ALM_Bat_Full_Send_Flag=false;
					Task_SPI_Flash(Flash_Write,1,&GSMB.w,1);												
				}
			}	
			
#endif
			
			break;
		}
	}
	
	for(i=0;i<3;i++)														// ��ȡCSQ
	{
		if(GSM_AT_Send("+CSQ",20,2,1,0x8000) == true)
		{
			p1 = strstr(gsm_rx_buf,"+CSQ:");strtok(p1,",");
			
			j = (unsigned char)(atoi(p1+6));
									
			if(j<32){gpsx.sig_strength = j;break; }	
		}	
	}

#if MODULE_UG96
	
	for(i=0;i<3;i++)														// ��ȡTEMP
	{
		if(GSM_AT_Send("+QTEMP",20,2,1,0x8001) == true)
		{
			p1 = strstr(gsm_rx_buf,"MP: ");
		
			if(*(p1+4) == '-')	{temperature = -1;p1=p1+5;	}
			else				{temperature = 1; p1=p1+4;	}
			
			j = (unsigned char)(atoi(p1));
						
			if(j > 80)			{j = 80;}
							
			temperature = temperature * j;
			
			break;
		}	
	}
	
#elif MODULE_SIM7500
	
	for(i=0;i<3;i++)														// ��ȡTEMP
	{
		if(GSM_AT_Send("+CPMUTEMP",25,2,1,0x8001) == true)
		{
			p1 = strstr(gsm_rx_buf,"MP: ");
		
			if(*(p1+4) == '-')	{temperature = -1;p1=p1+5;	}
			else				{temperature = 1; p1=p1+4;	}
			
			j = (unsigned char)(atoi(p1));
						
			if(j > 80)			{j = 80;}
							
			temperature = temperature * j;
			
			break;
		}	
	}
#endif
	
#if !DEBUG	
	if(flag1 >= 1)
	{
		for(i=0;i<3;i++)
		{
			if(GSM_AT_Send("+COPS?",30,2,1,0x0200) == true)
			{
				p1=strstr(gsm_rx_buf,"\"");
				p2=strstr(p1+1,"\"");
				if((p1 != NULL) && (p2 != NULL) && (isdigit(*(p1+1)))) 
				{	
#if MODULE_UG96
					switch(*(p2+2))
					{
						case '0':	
						case '3': network = 2;break;
						case '2':	
						case '4':
						case '5':
						case '6': network = 3;break;
					}
#endif										
					*p2=0;tmp=atoi(p1+1);
					
					if((tmp > 0) && (tmp < 9999999))	
					{
						gpsx.operators=tmp;break;
					}
#if MODULE_UG96
					else if((MCU_F.b.No_Simcard_Flag == false) && (CPIN_P() == false))	// ����,��������
					{
						Task_AT_GSM_Front(Gsm_cfun_reset,0,0,0,0,0,0);
					}
#endif
				}
				else
				{
#if MODULE_UG96
					GSM_AT_Send("+COPS=0,2",6,150,1,0x0001);
#else
					GSM_AT_Send("+COPS=3,2",6,150,1,0x0001);
#endif					
				}
			}
#if MODULE_SIM7500
			else if((MCU_F.b.No_Simcard_Flag == false) && (CPIN_P() == false))	// ����,��������
			{
				Task_AT_GSM_Front(Gsm_cfun_reset,0,0,0,0,0,0);
			}	
#endif
		}
#if MODULE_SIM7500	
		for(i=0;i<3;i++)
		{
			if(GSM_AT_Send("+CNSMOD?",22,2,1,0x0005) == true)
			{
				p1=strstr(gsm_rx_buf,"CNSMOD");strtok(p1,"\r");
			
				j = atoi(p1+10);
										
				if((j == 8) || (j == 16))						{network = 4;	}
				else if((j == 1) || (j == 2) || (j == 13))		{network = 2;	}
				else if((j > 0) && (j <16))						{network = 3;	}	
				else 											{network = 0;	}
				break;
			}
		}
#endif
	}
		
	if(flag1 >= 2)
	{		
		GSM_AT_Send("+CREG=2",6,2,3,0x0001);
		
		for(i=0;i<3;i++)
		{
			if(GSM_AT_Send("+CREG?",30,2,1,0x0020) == true)
			{
#if MODULE_SIM7500
				p1 = strstr(gsm_rx_buf,",");
				p1 = strstr(p1+1,",");
				p2 = strstr(p1+1,",");
				
				if((p1 != NULL) && (p2 != NULL))
				{
					p1++;*p2 = 0;p2++;strtok(p2,"\r");
									
					tmp=(u16)strtol(p1,&str,16);	
					if(tmp < 0xffff)			{gpsx.gsmbase1=(u16)tmp;}	
		   	
					tmp=strtol(p2,&str,16);			
					if((tmp > 0) && (tmp < 0xffffffff))			
					{
						gpsx.gsmbase2=tmp;
						
						if(MCU_F.b.No_Simcard_Flag)
						{
							Task_AT_GSM_Front(Gsm_cfun_reset,0,0,0,0,0,0);
						}
						
						break;
					}
				}
#else
				p1=strstr(gsm_rx_buf,"\"");	
				p2=strstr(p1+1,"\"");
				if((p1 != NULL) && (p2 != NULL)) 
				{
					*p2=0;		
					snprintf(tempv,9,"%s",p1+1);
					tmp=(u16)strtol(tempv,&str,16);	
					if(tmp < 0xffff)			{gpsx.gsmbase1=(u16)tmp;}
					p1=p2+3;
					p2=strstr(p1,"\"");
					*p2=0;
					snprintf(tempv,9,"%s",p1);    	
					tmp=strtol(tempv,&str,16);			
					if(tmp < 0xffffffff)		{gpsx.gsmbase2=tmp;break;}
				}		
				
#endif
			}
		}
		
		GSM_AT_Send("+CREG=0",6,2,2,0x0001);
	}
	
#endif
	
	GSM_Sleep();
}


#if CALLIN_ENABLE || CALLOUT_ENABLE

/* ͨ������,��������¼�	*/
void Gsm_Calling_Clr_Flag(void)
{
	CALL.w = 0;	
	Ring_Clip_Count=0;															
	cnt_1s.Ring_Time=0;	
	xEventGroupClearBits(EventGroupHandler_MCU,Event_Key_All);	
}

/* ����ͨ��ģʽ			*/
void Calling_Set_Mode(bool SOS_Hangup,bool Turn_Vol,bool Lis_To_Spk)
{
	CALL.b.SOS_HangUp_En_Flag = SOS_Hangup;
	CALL.b.Turn_Vol_En_Flag   = Turn_Vol;
	CALL.b.Lis_To_Spk_En_Flag = Lis_To_Spk;
}

#if MODULE_SIM7500
// ��������
void CECM_P(void)
{
	GSM_AT_Send("+CECM=1",6,2,2,0x0001);
}

void CSDVC_P(void)
{
	GSM_AT_Send("+CSDVC=3",6,15,3,0x0001);
}

#endif

/*�ҵ绰*/
void ATH_P(void)
{
#if MODULE_UG96
	GSM_AT_Send("H",6,2,2,0x0800);
#elif MODULE_SIM7500
	GSM_AT_Send("+CHUP",6,2,2,0x0800);
#endif
}

/*��ѯ�绰�״̬*/
u8 CPAS_P(void)
{	
	char* p1; 
	GSM_AT_Send("+CPAS",18,1,1,0x0001);
	p1=strstr(gsm_rx_buf,"+CPAS");
	switch(*(p1+7)-0x30)
	{
		case 0:return 0;//ready
		case 2:return 2;//unknown
		case 3:return 3;//ring
		case 4:return 4;//call in progress
		default:return 1;
	}
}

/* ��ѯ�绰״̬		*/
char CLCC(void)
{
	char *p1;
#if MODULE_SIM7500
	if(GSM_AT_Send("+CLCC",6,5,1,0x0001) == true)
#else	
	if(GSM_AT_Send("+CLCC",6,1,1,0x0001) == true)									// +CLCC: 1,0,2,0,0,"+8618948252262",145,""	��ѯ�Ƿ��лظ�CLCC,�绰����������Ϊ20���ַ�	
#endif
	{	
		return 0;																	// ����ʧ��
	}
	else
	{
		vTaskDelay(10);
		p1 = strstr(gsm_rx_buf,"+CLCC:");					
		if(p1 != NULL)																// ���Բ�ѯ��ͨ��״̬
		{
			return(*(p1+11));
		}
		else
		{
			return 1;
		}
	}
}

/* ͨ���д���		*/
void GSM_Calling_Handle(void)
{
#if MODULE_SIM7500
	char CPAS_Status='0';
#else
	char CPAS_Status=4;															// �绰�״̬
#endif
	
	EventBits_t Event_Key;
	
	while(1)
	{
		Event_Key = xEventGroupWaitBits(EventGroupHandler_MCU,Event_Key_All, pdFALSE,pdFALSE,1000);
		
		xEventGroupClearBits(EventGroupHandler_MCU,Event_Key_All);
		
		if(Event_Key & (Key_Call1_S_Flag | Key_Call2_S_Flag))						// ����/���Ϊͨ��ģʽ������һ���������,���Ե�������
		{				
			if(GSMB.b.Callout_LisMode_Flag)										
			{
				if(CALL.b.Lis_To_Spk_En_Flag)										// �������Ӿ���ģʽ�л���ͨ��ģʽ
				{
					CALL.b.Lis_To_Spk_En_Flag = false;
					CALL.b.Turn_Vol_En_Flag = true;									// ���Ե�������
					Speak_Amp_On();															
					spk_mic(GSM_P.spk_volume,MIC_Spk);								// ����������,��MIC����
				}
			} 	
		}
		
		if(Event_Key & Key_Call1_S_Flag)											// ����/���Ϊͨ��ģʽ������һ���������,���Ե�������
		{							
			if(CALL.b.Turn_Vol_En_Flag)												// ��������������
			{
				Spk_Vol_Turn(1);
			}	 	
		}
		else if(Event_Key & Key_Call2_S_Flag)										// ����/���Ϊͨ��ģʽ������һ���������,���Ե�С����
		{				
			if(CALL.b.Turn_Vol_En_Flag)												// ��������������
			{
				Spk_Vol_Turn(0);
			}	
		}
		
		if(Event_Key & Key_SOS_S_Flag)													// SOS�����Ҷ����е绰
		{	
			if(CALL.b.SOS_HangUp_En_Flag )												// ������������һ�
			{
				ATH_P();																// �Ҷϵ绰,�˳�ͨ������
				CPAS_Status = 0;														// ǿ�������˳�
			}			
		}
		else																			// û�а���������ѯͨ��״̬
		{
#if MODULE_SIM7500
			CPAS_Status=CLCC();
#else
			CPAS_Status=CPAS_P();														// ��ѯͨ��״̬
#endif
		}
				
		if(CPAS_Status==0)																// �ѹҶϵ绰
		{		
			GSMD.b.CMDF_SOS_Flag = false;
			GSMD.b.CMDF_BMAL_Flag = false;
			GSMD.b.CMDF_BMAH_Flag = false;
			Task_SPI_Flash(Flash_Write,8,(u8*)&GSMD,1);
			
			Gsm_Calling_Clr_Flag();
			
			Speak_Amp_Off();	

			break;
		}
	}
}

#endif


#if CALLIN_ENABLE

#if MODULE_SIM7500

// Num ��Ƶ�ļ���
void play_audio(bool Loc)
{
	xEventGroupClearBits(EventGroupHandler_OTH,Audio_Finish_Flag);
	
	if(Loc)
	{
		if(GSM_AT_Send("+CCMXPLAYWAV=\"/system/ring/Call.wav\",2",9,1,1,0x0006) == false)
		{	
			GSM_AT_Send("+CCMXPLAYWAV=\"E:/Call.wav\",2",9,1,1,0x0006);
		}
		
		CALL.b.Play_Audio_Flag = true;
	
		xEventGroupWaitBits(EventGroupHandler_OTH,Audio_Finish_Flag, pdFALSE, pdTRUE,2000);
	
		CALL.b.Play_Audio_Flag = false;
	}
	else
	{	
		if(GSM_AT_Send("+CCMXPLAYWAV=\"/system/ring/dtmf.wav\",1",9,1,1,0x0006))
		{
			CALL.b.Play_Audio_Flag = true;
		
			xEventGroupWaitBits(EventGroupHandler_OTH,Audio_Finish_Flag, pdFALSE, pdTRUE,6000);
		
			CALL.b.Play_Audio_Flag = false;
		}
		else 
		{
			if(GSM_AT_Send("+CCMXPLAYWAV=\"E:/dtmf.wav\",1",9,1,1,0x0006))
			{
				CALL.b.Play_Audio_Flag = true;
			
				xEventGroupWaitBits(EventGroupHandler_OTH,Audio_Finish_Flag, pdFALSE, pdTRUE,6000);
			
				CALL.b.Play_Audio_Flag = false;
			}
		}	
	}
}

// �Ƿ�ֹͣ������������
void stop_audio(void)
{
	GSM_AT_Send("+CCMXSTOPWAV",20,1,1,0x0001);
	
	vTaskDelay(10);
}

#endif

/* ��������		*/
void Gsm_Callin_ATA(void)
{
#if MODULE_SIM7500
	
	if(GSMB.b.Callin_SpkMode_Flag)
	{
		stop_audio();
	}
	
	GSM_AT_Send("A",27,5,1,0x0100);
	
	vTaskDelay(200);
	
	CECM_P();
	
#else
	
	GSM_AT_Send("A",6,2,2,0x0100);	
	
#endif
																							
	Gsm_Calling_Clr_Flag();		

	CALL.b.Calling_Flag = true;				
	
	if(GSMB.b.Callin_SpkMode_Flag)															// ����ͨ��ģʽ														
	{
		Calling_Set_Mode(1,1,0);
	}
	else																					// �������ģʽ
	{
		Calling_Set_Mode(0,0,0);
	}	
}

/* ��������		*/
bool GSM_Callin_Handle(void)
{
	if(CALL.b.Calling_Flag) {return false;}													// ����ͨ���� 						

	CALL.b.Calling_Flag = true;					
		
	if((((GSMA.b.Any_CallIn_Flag))|| (Compare_phone_number(Ring_Phone_Number) != 0)) && (!GSMD.b.CMDF_SOS_Flag) && (!GSMD.b.CMDF_BMAL_Flag) && (!GSMD.b.CMDF_BMAH_Flag))			
	{
		EventBits_t GSM_CallIn;
				
		if(GSMB.b.Callin_SpkMode_Flag)															// ͨ��ģʽ
		{	
			Speak_Amp_On();															
			
			spk_mic(GSM_P.spk_volume,MIC_Spk);
		}
		else																	
		{						
			spk_mic(0,MIC_Lis);	
		}
	
#if MODULE_SIM7500
		CSDVC_P();
#endif
		
		while(1)																
		{
#if MODULE_SIM7500
			if(CALL.b.Play_Ring_Flag)
			{
				CALL.b.Play_Ring_Flag = false;
				
				if(GSMB.b.Callin_SpkMode_Flag)
				{
					play_audio(1);
				}
			}
#endif
			
			GSM_CallIn = xEventGroupWaitBits(EventGroupHandler_MCU, Key_SOS_S_Flag|Key_Call1_L_Flag|Key_Call2_L_Flag|Key_Power_S_Flag, pdFALSE,pdFALSE,1000);
				
			xEventGroupClearBits(EventGroupHandler_MCU,Event_Key_All);
			
			if(GSM_CallIn & Key_Power_S_Flag)													// ��CALL1,PWR��һ��
			{								
				if(GSMA.b.CallIn_ByMe_Flag && GSMB.b.Callin_SpkMode_Flag)						// �������������������,��Ϊͨ��ģʽ
				{	
					Gsm_Callin_ATA();return true;
				}
			}
			
			if((!GSMA.b.CallIn_ByMe_Flag) || (!GSMB.b.Callin_SpkMode_Flag))						// ��������������������ģʽ
			{
#if DEBUG 
				if(Ring_Clip_Count >= 1)				
#else				
				if(Ring_Clip_Count >= 2)														// ���峬������,���Խ���
#endif
				{
					Gsm_Callin_ATA();return true;
				}
			}	

			if((cnt_1s.Ring_Time > 6) || ((GSM_CallIn & Key_SOS_S_Flag) && GSMA.b.CallIn_ByMe_Flag && GSMB.b.Callin_SpkMode_Flag))// ��������һ�.������Է��������Ĵ�ǰ�һ�
			{
#if MODULE_SIM7500
				if(GSMB.b.Callin_SpkMode_Flag)
				{
					stop_audio();
				}
#endif				
				if(GSM_CallIn & Key_SOS_S_Flag)													// SOS���������һ�
				{	
					ATH_P();																	// �Ҷϵ绰,�˳�ͨ������	
				}
				else																			// �Է��һ�
				{
					if((Ring_Phone_Number[0]!=0) && (Ring_Clip_Count < 4))								
					{
						strcpy(SMS_Phone_Number_Backup,Ring_Phone_Number);		
						SOS_BMA_F_GPS_Start(1);													// ������ȡGPS����,���ظ����ֻ�����	
					}
				}
					
				Gsm_Calling_Clr_Flag();	
				
				Speak_Amp_Off();												
				
				return false;
			}	
		}
	}																		
	else
	{
		ATH_P();																		
		Gsm_Calling_Clr_Flag();														
	}
	
	return false;
}	
#endif


#if CALLOUT_ENABLE

/* SOS,BMA,CALLIN,Fָ��,������ȡGPS��Ч��Ϣ		*/
void SOS_BMA_F_GPS_Start(unsigned char CMD)
{
	GPS_Power_On();																		// �����GPS��Դ
	
	if(GPS_F.b.CMDF_Cnt_Flag == false)												// δ��������ȡGPS��Ч����Ҫ��
	{
		cnt_1s.CMD_F_Count = 0;															// ��ʱ��0��ʼ
		GPS_F.b.CMDF_Cnt_Flag = true;
	}
		
	switch(CMD)
	{
		case 1: GPS_F.b.CMDF_Sendto_SMS_Flag = true; break;								// ��Ҫ�ظ����ֻ��Ķ�λ��Ϣ
			
		case 2: GPS_F.b.CMDF_Sendto_Gprs_Flag = true;break;								// ��Ҫ�ظ���GPRS�Ķ�λ��Ϣ
			
		case 3: GPS_F.b.CMDF_Sendto_SOS_BMA_Flag = true;
				GSMD.b.CMDF_SOS_Flag = true; 
				Task_SPI_Flash(Flash_Write,8,(u8*)&GSMD,1); break;						// ��Ҫ����SOS������λ��Ϣ
		
		case 4: GPS_F.b.CMDF_Sendto_SOS_BMA_Flag = true;
				GSMD.b.CMDF_BMAL_Flag = true;
				Task_SPI_Flash(Flash_Write,8,(u8*)&GSMD,1);break;						// ��Ҫ����BMA���䱨����λ��Ϣ
		
		case 5: GPS_F.b.CMDF_Sendto_SOS_BMA_Flag = true;
				GSMD.b.CMDF_BMAH_Flag = true;
				Task_SPI_Flash(Flash_Write,8,(u8*)&GSMD,1);break;						// ��Ҫ����BMA��ײ������λ��Ϣ
				
		case 6: GPS_F.b.CMDF_Powron_5min_Flag = true;break;								// ����5���Ӷ�ȡ��λ��Ϣ
		
		case 7: GPS_F.b.CMDF_Ten_Track_Flag = true;	break;								// ÿʮ���Ӷ�ȡһ��GPS
	}	
}

/* SOS/BMA����GPS������Ϣ		*/
void SOS_BMA_Send_AlmMsg_And_Callout(unsigned char GPS_Type)
{	
	cnt_1s.BiBi_Cnt = 0;
						
	if(GSMD.b.CMDF_SOS_Flag)
	{
		send_alarm(0x01,"HELP ME!",ALM_A,GPS_Type);								// ����SOS������Ϣ
	}
	else if(GSMD.b.CMDF_BMAL_Flag)
	{
		send_alarm(0x01,"Mdown!",ALM_M,GPS_Type);								// ���͵��䱨����Ϣ
	}
	else if(GSMD.b.CMDF_BMAH_Flag)
	{
		send_alarm(0x01,"Sudden Stop or Shock!",ALM_N,GPS_Type);				// ������ײ������Ϣ
	}
	
	if(GSMA.b.SOS_CALL_ENABLE_Flag && (!CALL.b.Calling_Flag))					// �貦��绰����
	{				
		if(GSMB.b.Callout_LisMode_Flag)											// ����ģʽ
		{
			Calling_Set_Mode(0,1,1);
		}
		else																	// ͨ��ģʽ
		{
			Calling_Set_Mode(1,1,0);
		}
				
		Task_AT_GSM(Gsm_SOS_BMA_Call_out,0,0,0,0,0,0);
	}
	else																		
	{		
		GSMD.b.CMDF_SOS_Flag = false;
		GSMD.b.CMDF_BMAL_Flag = false;
		GSMD.b.CMDF_BMAH_Flag = false;
		Task_SPI_Flash(Flash_Write,8,(u8*)&GSMD,1);
	}
}

/*sos����绰*/
/* ����:�Ƿ���������һ�; ����ֵ: 0: ����ʧ��; 1:����ɹ�; 2:�����һ����� */
bool SOS_BMA_Handle(void)
{
	bool hangup = false;
	unsigned char count_call_again=0,res=0,SOS_BMA=2;
		
	if(CALL.b.SOS_HangUp_En_Flag)
	{
		hangup = true;
	}
		
	if(GSMD.b.CMDF_SOS_Flag)
	{
		SOS_BMA = 1;
	}

	if((!GSMD.b.CMDF_SOS_Flag) && (!GSMD.b.CMDF_BMAL_Flag) && (!GSMD.b.CMDF_BMAH_Flag)) {return false;}
	
	while(count_call_again < Sos_cycle_num)													// ���δ����ɹ�,�������ದ��������ܳ�������ֵ
	{		
		res=Callout_Phone(GSM_ph.temp_ph1,1,GSMB.b.Callout_LisMode_Flag,hangup,SOS_BMA);	// �����һ������
		if((res==0)&&(count_call_again++ > Sos_cycle_num)){break;}							// �粦��ʧ���Ҳ��򳬳�����,���˳�
		else if((res==1)||(res==2)){break;	}												// �Ѿ��ɹ���ͨ�绰�������һ�
			
		if(Sos_call_num >= 2)																// ���������ڶ�����
		{	
			res=Callout_Phone(GSM_ph.temp_ph2,1,GSMB.b.Callout_LisMode_Flag,hangup,SOS_BMA);// ����ڶ�������
			if((res==0)&&(count_call_again++ > Sos_cycle_num)){break;}						// �粦��ʧ���Ҳ��򳬳�����,���˳�
			else if((res==1)||(res==2)){break;	}											// �Ѿ��ɹ���ͨ�绰�������һ�
		}
		
		if(Sos_call_num >= 3)																// ����������������
		{
			res=Callout_Phone(GSM_ph.temp_ph3,1,GSMB.b.Callout_LisMode_Flag,hangup,SOS_BMA);// �������������
			if((res==0)&&(count_call_again++ > Sos_cycle_num)){break;}						// �粦��ʧ���Ҳ��򳬳�����,���˳�
			else if((res==1)||(res==2)){break;	}											// �Ѿ��ɹ���ͨ�绰�������һ�
		}
		
		if(Sos_call_num >= 4)																// �����������ĺ���
		{
			res=Callout_Phone(GSM_ph.temp_ph4,1,GSMB.b.Callout_LisMode_Flag,hangup,SOS_BMA);// ������ĸ�����
			if((res==0)&&(count_call_again++ > Sos_cycle_num)){break;}						// �粦��ʧ���Ҳ��򳬳�����,���˳�
			else if((res==1)||(res==2)){break;	}											// �Ѿ��ɹ���ͨ�绰�������һ�
		}
		if(Sos_call_num >= 5)																// ���������������
		{
			res=Callout_Phone(GSM_ph.temp_ph5,1,GSMB.b.Callout_LisMode_Flag,hangup,SOS_BMA);// ������������
			if((res==0)&&(count_call_again++ > Sos_cycle_num)){break;}						// �粦��ʧ���Ҳ��򳬳�����,���˳�
			else if((res==1)||(res==2)){break;	}											// �Ѿ��ɹ���ͨ�绰�������һ�
		}
		if(Sos_call_num >= 6)																// ����������������
		{
			res=Callout_Phone(GSM_ph.temp_ph6,1,GSMB.b.Callout_LisMode_Flag,hangup,SOS_BMA);// �������������
			if((res==0)&&(count_call_again++ > Sos_cycle_num)){break;}						// �粦��ʧ���Ҳ��򳬳�����,���˳�
			else if((res==1)||(res==2)){break;	}											// �Ѿ��ɹ���ͨ�绰�������һ�
		}
		if(Sos_call_num >= 7)																// �����������ߺ���
		{
			res=Callout_Phone(GSM_ph.temp_ph7,1,GSMB.b.Callout_LisMode_Flag,hangup,SOS_BMA);// ������߸�����
			if((res==0)&&(count_call_again++ > Sos_cycle_num)){break;}						// �粦��ʧ���Ҳ��򳬳�����,���˳�
			else if((res==1)||(res==2)){break;	}											// �Ѿ��ɹ���ͨ�绰�������һ�
		}
		if(Sos_call_num >= 8)																// ���������ڰ˺���
		{
			res=Callout_Phone(GSM_ph.temp_ph8,1,GSMB.b.Callout_LisMode_Flag,hangup,SOS_BMA);// ����ڰ˸�����
			if((res==0)&&(count_call_again++ > Sos_cycle_num)){break;}						// �粦��ʧ���Ҳ��򳬳�����,���˳�
			else if((res==1)||(res==2)){break;	}											// �Ѿ��ɹ���ͨ�绰�������һ�
		}
	}	
	
	if(res == 1)
	{
		return true;
	}
	else
	{
		Gsm_Calling_Clr_Flag();
		GSMD.b.CMDF_SOS_Flag = false;
		GSMD.b.CMDF_BMAL_Flag = false;
		GSMD.b.CMDF_BMAH_Flag = false;
		Task_SPI_Flash(Flash_Write,8,(u8*)&GSMD,1);	
		return false;
	}
}

/* AT+CLCC��ѯ�绰״̬ 0:�Է�δ����;1:�Է��ѽ���;2:�����Ҷϵ绰*/
/* ����: ����绰��ͨ�����Ǽ���ģʽ, �Ƿ���������Ҷ�			*/
unsigned char CLCC_P(bool Callout_LisMode,bool hangup,unsigned char SOS_BMA)					
{
	char clcc;
#if MODULE_SIM7500
	bool dtmf_start = true;
	bool spk_mic_start = true;
#endif
	EventBits_t Event_CLCC_Key;
  
	while(1)
	{
		Event_CLCC_Key = xEventGroupWaitBits(EventGroupHandler_MCU, Event_Key_All, pdFALSE,pdFALSE,1000);
		
		xEventGroupClearBits(EventGroupHandler_MCU,Event_Key_All);	
		
#if volume_control

#if MODULE_SIM7500
		
		if((Event_CLCC_Key & (Key_Call1_S_Flag | Key_Call1_S_Flag)) && (clcc == '3'))		// ����+
		{				
			if(!Callout_LisMode)															// �Ǽ���ģʽ�¿��Ը�������		
			{
				Spk_Vol_Turn(1);
			}			
		}
		else if((Event_CLCC_Key & (Key_Call2_S_Flag | Key_Call2_L_Flag)) && (clcc == '3'))	// ����-
		{				
			if(!Callout_LisMode)															// �Ǽ���ģʽ�¿��Ը�������	
			{
				Spk_Vol_Turn(0);
			}			
		}
#else	
		
		if(Event_CLCC_Key & Key_Call1_S_Flag)												// ����+
		{				
			if(!Callout_LisMode)															// �Ǽ���ģʽ�¿��Ը�������		
			{
				Spk_Vol_Turn(1);
			}			
		}
		else if(Event_CLCC_Key & Key_Call2_S_Flag)											// ����-
		{				
			if(!Callout_LisMode)															// �Ǽ���ģʽ�¿��Ը�������	
			{
				Spk_Vol_Turn(0);
			}			
		}
		
#endif
		
#endif 	
		
		if(Event_CLCC_Key & Key_SOS_S_Flag)													// �ڲ����绰���̳���,��SOS����
		{
			if(hangup==true)																// �������SOS�Ҷϵ绰
			{
				ATH_P();																	// �Ҷϵ绰,�˳�ͨ������
				return 2;
			}
		}
		else
		{
			clcc = CLCC();
						
			if(clcc)																		// ���Բ�ѯ��ͨ��״̬
			{		
				if(clcc == '0')																// �ѽ�ͨ	
				{					
#if MODULE_SIM7500
					if(SOS_BMA && GSMC.b.DTMF_Det_Flag)										// DTMF
					{
						if(dtmf_start == true)												// �״�
						{
							CECM_P();
							dtmf_start = false;cnt_1s.Ring_Time = 0;CALL.b.Dtmf_Flag = true;
							xEventGroupClearBits(EventGroupHandler_OTH,DTMF_OK_Flag);
						}
						
						if((cnt_1s.Ring_Time % 10) == 0)
						{
							play_audio(0);
						}
									
						if(xEventGroupGetBits(EventGroupHandler_OTH) & DTMF_OK_Flag)
						{
							CALL.b.Dtmf_Flag = false;
							stop_audio();
							return 1;
						}
						else if(cnt_1s.Ring_Time >= 20)
						{
							CALL.b.Dtmf_Flag = false;							
							ATH_P();
							stop_audio();
							return 0;
						}	
					}
					else
					{
						CECM_P();
#endif
						return 1;
#if MODULE_SIM7500
					}
#endif

				}	
#if MODULE_SIM7500
				else if((clcc == '3') && spk_mic_start)
				{
					spk_mic_start = false;
					
					CSDVC_P();
					
					if(Callout_LisMode == true)												// ���Ϊ����ģʽ
					{
						spk_mic(0,MIC_Lis);													// ����������,��MIC����		
					}
					else																	// ���Ϊͨ��ģʽ
					{
						spk_mic(GSM_P.spk_volume,MIC_Spk);									// ����������,��MIC����
					}
				}	
#endif				
				else if((clcc == '1') || (clcc == '4') || (clcc == '5'))					// ����Է�δ����/ͨ����/�ȴ������˳�
				{
					return 0;																// ����ʧ��
				}			
			}
			else																			// ���ѹһ�
			{	
				return 0;																	// ����ʧ��
			}
		}
	}
}

/*����绰����ֵ 0:�Է�δ����/�绰�������/�պ�;1:�Է��ѽ���;2:�����Ҷϵ绰					*/
/*����: �绰����,����ز�����,���ͨ�������ģʽ,�Ƿ���������Ҷ�							*/
unsigned char Callout_Phone(char *PhoneNum,unsigned char Call_Time,bool Call_LisMode,bool hangup,unsigned char SOS_BMA)
{ 
	unsigned char i;
	unsigned char Res_Tmp=0;
	char ATD_PhoneNum_TMP[32]={0};	
	
	if(CALL.b.Calling_Flag) return 2;									// �������ͨ��,��ֱ���˳�
	
	if((*PhoneNum<0x30 && *PhoneNum!='+')||(*(PhoneNum)>0x39))	{return 0;	}	// ���ѡ��ĺ���δ����(�жϵ�һλ�͵ڶ�λ)����ֱ���˳�	
	if(*(PhoneNum+1)<0x30 || *(PhoneNum+1)>0x39)				{return 0;	}	// ����������,��ֱ���˳�

	CALL.b.Calling_Flag = true;											// Ԥ�Ƚ���ͨ����־	
		
	if(Call_LisMode == true)											// ���Ϊ����ģʽ
	{	
#if MODULE_UG96
		spk_mic(0,MIC_Lis);												// ����������,��MIC����	
#endif		
	}
	else																// ���Ϊͨ��ģʽ
	{		
		Speak_Amp_On();	
#if MODULE_UG96
		spk_mic(GSM_P.spk_volume,MIC_Spk);								// ����������,��MIC����
#endif
	}
				
	for(i=0;i<Call_Time;i++)											// �������ʧ��,�ظ�����ͬһ�绰����
	{
		sprintf(ATD_PhoneNum_TMP,"D%s;",PhoneNum);						// ִ�в���绰ָ��ATD+����+�ֺ�+\r\n	
		if(GSM_AT_Send(ATD_PhoneNum_TMP,6,3,1,0x0001))					// ����绰�ɹ�
		{
			Res_Tmp=CLCC_P(Call_LisMode,hangup,SOS_BMA);				// ��ȡ����绰״̬
			if(Res_Tmp) break;											// ����Է��ѽ���(1), ���������Ҷ�(2)
		}
	}
	
	if(Res_Tmp != 1)													// �Է�δ�����������Ҷ�,���ͨ����־,���ô���绰����
	{
		CALL.b.Calling_Flag = false;
		Speak_Amp_Off();								
		ATH_P();														// ������
	}
	
	return(Res_Tmp);													// ���ش���绰���
}

/*�������ȡ���ͷ����*/
void spk_mic(u8 spk,u8 mic)
{
#if MODULE_SIM7500
	char buf1[24];
#else
	char buf1[12];
#endif
	
	if(spk > 10) {spk = 10;}
		
#if  MODULE_UG96																// ����ģ�����ֱ����������
	snprintf(buf1,12,"+CLVL=%d",spk*10);								// ��������
#elif MODULE_SIM7500
	snprintf(buf1,24,"+CWIIC=0x34,0x6C,0x%02X,1",spk * 6 + 0x83);		// ��������	
#endif
	GSM_AT_Send(buf1,6,1,2,0x0001);

#if MODULE_SIM7500
	if(mic > 8) {mic=8;}
	snprintf(buf1,12,"+CMICGAIN=%d",mic);								// MIC����
	GSM_AT_Send(buf1,6,1,2,0x0001);
#elif MODULE_UG96
	if(mic > 15) {mic=15;}
	snprintf(buf1,12,"+QMIC=2,%d",mic);									// MIC����
	GSM_AT_Send(buf1,6,1,2,0x0001);
#endif
}

/* �����������ӻ��С	*/
void Spk_Vol_Turn(bool add_dec)
{
	if(add_dec == true)						// ��������
	{
		GSM_P.spk_volume++;

		if(GSM_P.spk_volume>10)	{GSM_P.spk_volume=10;	}
	}
	else									// ��С����
	{
		GSM_P.spk_volume--;
		if(GSM_P.spk_volume==255){GSM_P.spk_volume=0;	}
	}
	
	spk_mic(GSM_P.spk_volume,MIC_Spk);		// ������������	
}


#endif


#if SMS_ENABLE

/* �Ƚ�ָ��,����Ҫ��֤��Ȩ����*/
bool Compare_command(char* command)
{	
	if((command[0] == 'A') && (command[2] == ','))	{return true;}
	
	return false;
}


/*ɾ������*/
void cmgd_p(bool UnRead_All)
{		
#if MODULE_UG96
	GSM_AT_Send("+CMGD=1,1",6,5,5,0x0001);
#elif MODULE_SIM7500
	if(UnRead_All == true)
	{
		GSM_AT_Send("+CMGD=,4",6,5,5,0x0001);
	}
	else
	{
		GSM_AT_Send("+CMGD=,1",6,5,5,0x0001);
	}
#endif
}	

/*��ȡ���ŵ����ݣ��ֻ�����ż���,p_123456Ϊָ�������ָ�����
//+CMGR: "REC UNREAD","1252013510000786","","15/06/17,11:37:55+32" Kaisen: 525252A4,13145906426*/
//+CMGR: "REC UNREAD","13923474423",,"17/06/02,20:50:27+32"
//123456B
char * Read_sms_content(void)
{	
	char *pn,*p_123456;
	
	pn=strstr(gsm_rx_buf,"READ");													// ��ȡ�Ƿ�Ϊȡ����
	
	if(pn != NULL)																	// δ��ȡ����
	{
#if RESET_FUN	
		if(strstr(gsm_rx_buf,"RESET!")!=NULL) 
		{				
			if((strstr(gsm_rx_buf,"RESET!1")) != NULL)		
			{
				cmgd_p(false);
				Task_SPI_Flash(Flash_MCU_Reset,0,0,0);								// ֻ��λ
			}
			else 			
			{
				cmgd_p(false);
				Task_SPI_Flash(Flash_Reset_ALL,0,0,0);								// �������в���
			}
		}		
#endif
		
		p_123456 = strstr(pn,"\n");p_123456++;
			
		pn=strstr(pn,","); 															// Ѱ�ҵ绰����ǰ','
		pn=pn+2;pn=strtok(pn,"\"");															
		snprintf(SMS_Phone_Number,24,"%s",pn);										// ���Ƶ绰����
				
		if(SMS_Phone_Number[0])														// ����绰����ǿ����������
		{
			if(strncmp(p_123456,GSM_P.cmd_key,6) == 0)	{return p_123456+6;	}
			else if(!GSMC.b.SMS_Check_psw_Flag)			{return p_123456;	}
		}
	}

	return NULL;
}	

/* �����¶���	*/
void New_SMS_Handle(void)
{  
	char buf1[9];
	unsigned char i=0;
		
	GSM_Wakeup();
	
	clr_gsm_buf();
	
	SMS.b.Chk_CMGL_Flag = true;											// �����жϻ�ȡδ�����ű�־������
	send_str_to_gsm("AT+CMGL=\"REC UNREAD\"\r\n");	 					// ���Ͷ�ȡ����δ������ָ��
	xEventGroupWaitBits(EventGroupHandler_OTH,CMGL_OK_Flag, pdFALSE, pdFALSE,10000);
	SMS.b.Chk_CMGL_Flag = false;
	
	if(SMS.b.New_SMS_Flag)
	{
#if MODULE_SIM7500
		for(i=0;i<=SMS_NUM;i++)												// ��ѯ����
#else
		for(i=1;i<=SMS_NUM;i++)												// ��ѯ����
#endif
		{		
			snprintf(buf1,9,"+CMGR=%d",i); 									// ��ȡ����ʾ�������� "AT+CMGR=1\r\n"����ȡ��һ������		

			if(GSM_AT_Send(buf1,6,1,1,0x0080))								// ���ж��Ƿ�ΪOK,���ж��Ƿ�ΪUNREAD��READ	
			{
				char *sms;
				sms=Read_sms_content(); 									// ȡ����������	
				if(sms != NULL)
				{
#if COMPARE_PASWD
					if((Compare_command(sms) == true) || (Compare_phone_number(SMS_Phone_Number) != 0))
#else
					if(1)	
#endif
					{
						strtok(sms,"\r");strtok(sms,"#");
						analysis_sms_content(sms, RPLY_TO_SMS);				// �����������ݲ�����Ӧ�Ĵ���
					}			
				}
			}
		}
		
		if(SMS_NUM > 30){cmgd_p(true);		}
		else			{cmgd_p(false);		}
		
		SMS_NUM = 0;
		SMS.b.New_SMS_Flag = false;
		cnt_1s.Chk_SMS_Ten_Cnt = 0;														// ɾ�������Ѷ�����
	}
}	

/*���͵绰���룬 ˵����ph_numΪ�跢�͵��ֻ��ţ������շ��ֻ����룬ʾ����AT+CMGS="13145906426"\r\n*/
bool send_phone_number(char*ph_num)
{
	char pbuf1[30];
	unsigned char i=0;
	
	for(i=0;i<2;i++)
	{
		snprintf(pbuf1,30,"+CMGS=\"%s\"",ph_num);// �����ֻ�����
		if(GSM_AT_Send(pbuf1,4,5,1,0x0002) == false)	
		{
			uartTxByte(&hlpuart1,0x1b);			// �����ַ�
			send_str_to_gsm("\r\n");
			vTaskDelay(1000);
		}
		else									// ����ȡ���ɹ�,����
		{
			return true;
		}
	}

	return false;								// ������
}	

/*���Ͷ��ŵ��ֻ������裺�ȷ��ͽ��շ��ֻ��ţ��ٷ������ݣ������ 0x1A,0d,0a����*/
void Send_Msg_To_Phone(char*PhoneNum,char*str1,u8 GPS_Type,u8 MSG_Type)
{	
	unsigned char i=0;
	EventBits_t Send_Result;
	
	if((PhoneNum[0] == 0) || (strlen(PhoneNum) <= 4)) return;		// ����պ�,ֱ���˳�
		
/*******************************************************************************************	
***********************************��ȡMCU_F\GPS����******************************************
********************************************************************************************/	
	
	if((GPS_Type!=HISGPS)&&(GPS_Type!=NOGPS))		// ��ʷ���ݼ���GPS����,�����ȡMCU_F�ź�ǿ��/��վ��Ϣ������
	{	
		Gsm_Get_Parameters(2);
	}
	
	if(GPS_Type)									// ��ȡGPS����,������Ӧ��ʽ��ӡ
	{
		Read_GPS_data(GPS_Type,MSG_Type);
		
		if((GPS_Type == CMDFGPS) || (GPS_Type == NEWGPS) || (GPS_Type == ALMGPS))
		{
			dtbuf[0]='A';
		}
		else if(GPS_Type == OLDGPS)
		{
			dtbuf[0]='V';
		}
	}
		
/*******************************************************************************************	
******************************************��������******************************************
********************************************************************************************/

	for(i=0;i<3;i++)
	{
		if(send_phone_number(PhoneNum)==0) return;			// ���ͺ������,�򷵻�	
	
		clr_gsm_buf();
		
		if(GSM_OTA.Device[0] != '\0'){send_str_to_gsm(GSM_OTA.Device);send_str_to_gsm("\n");}
			
		send_str_to_gsm(str1); 								// ����ָ���
		
		if((GPS_Type!=NOGPS)&&(MSG_Type!=MSG_NO))
		{
			if(dtbuf[0] == 'V')								// ���������ЧGPS����
			{
				send_str_to_gsm("Before!");
			}
			
			send_str_to_gsm("\n");
			
			send_str_to_gsm(dtbuf+2);						// GPS/MCU_F����
		}
		
		uartTxByte(&hlpuart1,0x1a);							// ���ͽ���ַ�
		send_str_to_gsm("\r\n");

		SMS.b.Chk_CMGS_Flag = true;
		Send_Result=xEventGroupWaitBits(EventGroupHandler_OTH,CMGS_Send_OK_Flag|CMGS_Send_NG_Flag, pdFALSE, pdFALSE,30000);
		SMS.b.Chk_CMGS_Flag = false;
	
		vTaskDelay(10);	
		
		if(Send_Result & CMGS_Send_OK_Flag)					// ���ͳɹ�
		{
			break;
		}
		else												// ����ʧ��
		{
			if(i<2)											// ǰ���η���ʧ��,ȡ���ط�,�����β�ȡ��
			{
				uartTxByte(&hlpuart1,0x1b);						
				send_str_to_gsm("\r\n");	
				vTaskDelay(3000);
			}
		}
	}
	
	vTaskDelay(100);	
}	

#endif


#if GPRS_ENABLE

/* ������ȡSEND OK				*/
void GPRS_Start_ChkSendok(void)
{
	clr_gsm_buf();	
	GPRS.b.Chk_Notif_Flag = true;
}

/* GPRSȡ�����ݷ���				*/
void GPRS_Cancel_Send(void)
{
	GPRS_Start_ChkSendok();				
	uartTxByte(&hlpuart1,0x1B); send_str_to_gsm("\r\n");vTaskDelay(1000);	
}

/* ����GPRS���ݺ�����ѯ��� */
bool GPRS_Check_SendOK(void) 
{
	EventBits_t Event_Gprs_Send_Result;	
	
#if MODULE_UG96
	Event_Gprs_Send_Result = xEventGroupWaitBits(EventGroupHandler_OTH,Gprs_Send_OK_Flag|Gprs_Send_NG_Flag, pdFALSE, pdFALSE,3000);
#elif MODULE_SIM7500
	Event_Gprs_Send_Result = xEventGroupWaitBits(EventGroupHandler_OTH,Gprs_Send_OK_Flag|Gprs_Send_NG_Flag, pdFALSE, pdFALSE,10000);vTaskDelay(1);
#endif
	
	xEventGroupClearBits(EventGroupHandler_OTH,Gprs_Send_OK_Flag|Gprs_Send_NG_Flag);
	
	if(Event_Gprs_Send_Result & Gprs_Send_OK_Flag)
	{
		GPRS.b.Chk_Notif_Flag = false;return true;
	}
	else
	{
		GPRS.b.Chk_Notif_Flag = false;return false;
	}
}

/*������Ϣ��������			*/
/* ����˵�� ����ָ��/��ȡGPS����/������Ͳ���ȥ�Ƿ���Ҫ����*/
void Send_Msg_To_Platform(u8 GPRS_CMD,u8 GPS_Type,u8 MSG_Type,u8 Save)
{	
	u8 i=0;											// ���ʹ���
	
	/* ����Ϊ�ж��Ƿ���Ҫ��ȡGPS�����ݼ�MCU_F��վ,��������Ϣ*/	
	
	if((!GSMA.b.GPRS_flag) || MCU_F.b.No_Simcard_Flag) return;
	
	switch(GPRS_CMD)
	{
		case CMD_A:  _ptcmdx=",CMD-A,"; break;		// ָ��A
		case CMD_A1: _ptcmdx=",CMD-A1,";break;		// ָ��A1,���ø��ĵ绰����1
		case CMD_A2: _ptcmdx=",CMD-A2,";break;		// ָ��A2,���ø��ĵ绰����2
		case CMD_A3: _ptcmdx=",CMD-A3,";break;		// ָ��A3,���ø��ĵ绰����3
		case CMD_A4: _ptcmdx=",CMD-A4,";break;		// ָ��A4,���ø��ĵ绰����4
		case CMD_A5: _ptcmdx=",CMD-A5,";break;		// ָ��A5,���ø��ĵ绰����5
		case CMD_A6: _ptcmdx=",CMD-A6,";break;		// ָ��A6,���ø��ĵ绰����6
		case CMD_A7: _ptcmdx=",CMD-A7,";break;		// ָ��A7,���ø��ĵ绰����7
		case CMD_A8: _ptcmdx=",CMD-A8,";break;		// ָ��A8,���ø��ĵ绰����8
		case CMD_B:  _ptcmdx= HS_VER;  	break;		// ָ��B, ��ȡ�汾��
		case CMD_C:  _ptcmdx=",CMD-C,"; break;		// ָ��C, ����TCP/IP
		case CMD_D:	 _ptcmdx=",CMD-D,"; break;		// ָ��D, ���ÿ���GPRS
		case CMD_E:	 _ptcmdx=",CMD-E,"; break;		// ָ��E, ���ùر�GPRS
		case CMD_F:  _ptcmdx=",CMD-F,"; break;		// ָ��F, ��ȡ���ζ�λ
		case CMD_F1: _ptcmdx=",CMD-F1,";break;		// ָ��F1,��ȡ��վ��λ
		case CMD_G:  _ptcmdx=",CMD-G,"; break;		// ָ��G, 
		case CMD_H:  _ptcmdx=",CMD-H,"; break;		// ָ��H, ���ø�������
		case CMD_I:  _ptcmdx=",CMD-I,"; break;		// ָ��I, ����GEO
		case CMD_J:	 _ptcmdx=",CMD-J,"; break;		// ָ��J, �����ٶȱ���
		case CMD_K:	 _ptcmdx=",CMD-K,"; break;		// ָ��K, ָ��ػ�
		case CMD_K0: _ptcmdx=",CMD-K0,";break;		// ָ��K, ���ÿ������ٱ���
		case CMD_K1: _ptcmdx=",CMD-K1,";break;		// ָ��K, ���ÿ���ʧ�ر���
		case CMD_K2: _ptcmdx=",CMD-K2,";break;		// ָ��K, ���ùرռ��ٱ���
		case CMD_K3: _ptcmdx=",CMD-K3,";break;		// ָ��K, ���ùر�ʧ�ر���
		case CMD_L:  _ptcmdx=",CMD-L,"; break;		// ָ��L, ����ʱ��
		case CMD_L1: _ptcmdx=",CMD-L1,";break;		// ָ��L1,��������ʱ��������/ʱ��
		case CMD_L2: _ptcmdx=",CMD-L2,";break;		// ָ��L2,����
		case CMD_L3: _ptcmdx=",CMD-L3,";break;		// ָ��L3,����OLED��ʾ�Ƕ�
		case CMD_L4: _ptcmdx=",CMD-L4,";break;		// ָ��L4,�������»�ȡGPSʱ��
		case CMD_L5: _ptcmdx=",CMD-L5,";break;		// ָ��L5,��������
		case CMD_M:	 _ptcmdx=",CMD-M,"; break;		// ָ��M, ����������λ
		case CMD_M2: _ptcmdx=",CMD-M2,";break;		// ָ��M2,��������
		case CMD_M3: _ptcmdx=",CMD-M3,";break;		// ָ��M3,����GPRS����������λʱ����
		case CMD_M4: _ptcmdx=",CMD-M4,";break;		// ָ��M4,����GPS���ģʽ
		case CMD_N:	 _ptcmdx=",CMD-N,"; break;		// ָ��N, ���õ͵籨��
		case CMD_O:	 _ptcmdx=",CMD-O,"; break;		// ָ��O, ����APN
		case CMD_P:	 _ptcmdx=",CMD-P,"; break;		// ָ��P, ����λ�Ʊ���
		case CMD_Q1: _ptcmdx=",CMD-Q1,";break;		// ָ��Q1,����SOS�ǲ���Ҫ����绰
		case CMD_Q2: _ptcmdx=",CMD-Q2,";break;		// ָ��Q2,�����Ƿ���Ҫ��֤����Ϊ��Ȩ����
		case CMD_Q3: _ptcmdx=",CMD-Q3,";break;		// ָ��Q3,��������Ϊͨ��ģʽʱ,�Ƿ��������������Ҷ�����
		case CMD_Q4: _ptcmdx=",CMD-Q4,";break;		// ָ��Q4,���ô���绰�Ƿ�������������ʶ��
		case CMD_Q5: _ptcmdx=",CMD-Q5,";break;		// ָ��Q5,���ô�������Ƿ���Ҫ�ȶ�����
		case CMD_R:	 _ptcmdx=",CMD-R,"; break;		// ָ��R, ��������
		case CMD_S:	 _ptcmdx=",CMD-S,"; break;		// ָ��S
		case CMD_T:	 _ptcmdx=",CMD-T,"; break;		// ָ��T, ��������������λ����
		case CMD_U:  _ptcmdx=",CMD-U,"; break;		// ָ��U, OTA����
		case CMD_U0: _ptcmdx=",CMD-U0,";break;		// ָ��U0,���ô���绰Ϊͨ�� 
		case CMD_U1: _ptcmdx=",CMD-U1,";break;		// ָ��U1,���ô���绰Ϊ����
		case CMD_U2: _ptcmdx=",CMD-U2,";break;		// ָ��U2,���ô���绰Ϊ����
		case CMD_U3: _ptcmdx=",CMD-U3,";break;		// ָ��U3,���ô���绰Ϊͨ��
		case CMD_U4: _ptcmdx=",CMD-U4,";break;		// ָ��U4,����������GPS��Դ
		case CMD_U5: _ptcmdx=",CMD-U5,";break;		// ָ��U5,���ý�ֹ����GPS��Դ
		case CMD_U6: _ptcmdx=",CMD-U6,";break;		// ָ��U6,����GPRSʡ����ģʽ
		case CMD_U7: _ptcmdx=",CMD-U7,";break;		// ָ��U7,����GPRS������ʱ���ϴ�
		case CMD_U8: _ptcmdx=",CMD-U8,";break;		// ָ��U8,����GPS��ǿģʽ,���ضϵ�Դ
		case CMD_U9: _ptcmdx=",CMD-U9,";break;		// ָ��U9,����GPSʡ��ģʽ,��ضϵ�Դ
		case CMD_V:	 _ptcmdx=",CMD-V,"; break;		// ָ��V
		case CMD_V3: _ptcmdx=",CMD-V3,";break;		// ָ��V3,���ô����Ӧ�绰
		case CMD_W:	 _ptcmdx=",CMD-W,"; break;		// ָ��W
		case CMD_X:	 _ptcmdx=",CMD-X";	break;		// ָ��X, ������ʱ������������
		case CMD_Y:	 _ptcmdx=",CMD-Y,"; break;		// ָ��Y
		case CMD_Z1: _ptcmdx=",CMD-Z1,";break;		// ָ��Z1
		case CMD_Z2: _ptcmdx=",CMD-Z2,";break;		// ָ��Z2
		
		case ALM_A:	 _ptcmdx=",ALM-A,"; break;		// ����A, ��������SOS����
		case ALM_B1: _ptcmdx=",ALM-B1,";break;		// ����B1,��������GEO1����	
		case ALM_B2: _ptcmdx=",ALM-B2,";break;		// ����B2,��������GEO2����	
		case ALM_B3: _ptcmdx=",ALM-B3,";break;		// ����B3,��������GEO3����	
		case ALM_B4: _ptcmdx=",ALM-B4,";break;		// ����B4,��������GEO4����	
		case ALM_B5: _ptcmdx=",ALM-B5,";break;		// ����B5,��������GEO5����	
		case ALM_C:	 _ptcmdx=",ALM-C,"; break;		// ����C, �������ͳ��ٱ���
		case ALM_D:	 _ptcmdx=",ALM-D,";	break;		// ����D, �������͵͵籨��
		case ALM_E:	 _ptcmdx=",ALM-E";	break;		// ����E, ��������GPS��ʱ������Ч���ݱ���
		case ALM_F:	 _ptcmdx=",ALM-F,"; break;		// ����F, �������ͳ����籨��	
		case ALM_G:	 _ptcmdx=",ALM-G,"; break;		// ����G
		case ALM_H:	 _ptcmdx=",ALM-H,"; break;		// ����H, GPSλ�Ʊ���		
		case ALM_I:	 _ptcmdx=",ALM-I,"; break;		// ����I	
		case ALM_J:	 _ptcmdx=",ALM-J,"; break;		// ����J
		case ALM_K:	 _ptcmdx=",ALM-K,"; break;		// ����K
		case ALM_L:	 _ptcmdx=",ALM-L,"; break;		// ����L		
		case ALM_M:	 _ptcmdx=",ALM-M,"; break;		// ����M, BMA���䱨��
		case ALM_N:	 _ptcmdx=",ALM-N,"; break;		// ����N, BMA��ײ���� 
		case ALM_O:	 _ptcmdx=",ALM-O,"; break;		// ����O
		case ALM_P:	 _ptcmdx=",ALM-P,"; break;		// ����P
		case ALM_Q:	 _ptcmdx=",ALM-Q,"; break;		// ����Q
		case ALM_R:	 _ptcmdx=",ALM-R,"; break;		// ����R
		case ALM_S:	 _ptcmdx=",ALM-S,"; break;		// ����S
		case ALM_T:	 _ptcmdx=",ALM-T,"; break;		// ����T
		case ALM_U:	 _ptcmdx=",ALM-U,"; break;		// ����U, OTA�����ɹ�
		case ALM_V:	 _ptcmdx=",ALM-V,"; break;		// ����V
		case ALM_W:  _ptcmdx=",ALM-V"; 	break;		// ����W
		case ALM_X:  _ptcmdx=",ALM-X"; 	break;		// ����X
		case ALM_Y:	 _ptcmdx=",ALM-Y,"; break;		// ����Y
		case ALM_Z:	 _ptcmdx=",ALM-Z,"; break;		// ����Z
		
		case CMD_ERR:_ptcmdx=",ERROR!";	break;		// ָ�����
		
		default:     return;						// ָ�����
	}

	if((GPS_Type!=HISGPS)&&(GPS_Type!=NOGPS))		// ��ʷ���ݼ���GPS����,�����ȡMCU_F�ź�ǿ��/��վ��Ϣ������
	{	
		Gsm_Get_Parameters(2);
	}
	
	if(GPS_Type)									// ��ȡGPS����,������Ӧ��ʽ��ӡ
	{
		Read_GPS_data(GPS_Type,MSG_Type);
		
		if((GPS_Type == CMDFGPS) || (GPS_Type == NEWGPS) || (GPS_Type == ALMGPS))
		{
			dtbuf[0]='A';
		}
		else if(GPS_Type == OLDGPS)
		{
			dtbuf[0]='V';
		}
	}
		
	if((GPRS_CMD == CMD_T) && Save)															// ����Ƿ�������λ����,����Ҫ����,������ʷ���ݻ�δ������,���ȱ������ݲ�����,�ȴ�������ʷ���ݷ�����ɺ������һ����
	{
		if(GSM_P.send_history < GSM_P.store_history)
		{
			i=200;
		}
	}
		
	/********************************************************************************************/	
	/***********************************�������ͣ���⡮>��****************************************/	
	/********************************************************************************************/
	// ʾ����#867273021518134,CMD-T,A,DATE:160113,TIME:140709,LAT:22.7613506N,LOT:114.3610382E,Speed:000.3,X-0-1-100-26,000,46000-262D-FF86#	
			
	if(GPRS.b.Connected_OK_Flag && (i==0))													// �����ǰ��������,�Ҳ�������������
	{									
		for(;i<2;i++)																		// ѭ��2�Σ����������˳�
		{	
			if(CALL.b.Ring_Flag || CALL.b.Calling_Flag)										// ������������Ҫ����ˢ��,�����˳�,������
			{
				i=200;break;																// ������		
			}
			
#if MODULE_UG96
			if(GSM_AT_Send("+QISEND=1",4,2,1,0x0002))										// ��������
#elif MODULE_SIM7500
			if(GSM_AT_Send("+CIPSEND=0,",3,10,1,0x0002))									// ��������
#endif
			{
	/********************************************************************************************/	
	/******************************************��������******************************************/	
	/********************************************************************************************/
				
				uartTxByte(&hlpuart1,'#');													
				send_str_to_gsm(GSM_P.IMEI);												
				send_str_to_gsm(_ptcmdx);														
				if(GPS_Type)
				{
					send_str_to_gsm(dtbuf);
					uartTxByte(&hlpuart1,',');send_str_to_gsm(wifi_mac[0]);
					uartTxByte(&hlpuart1,',');send_str_to_gsm(wifi_mac[1]);
					uartTxByte(&hlpuart1,',');send_str_to_gsm(wifi_mac[2]);
				}
				
				uartTxByte(&hlpuart1,'#');													
				
				GPRS_Start_ChkSendok();
				uartTxByte(&hlpuart1,0x1A);send_str_to_gsm("\r\n");							

				if(GPRS_Check_SendOK())														// ������ͳɹ�
				{	
					cnt_1s.HeartBeat_Cnt = 0;break;
				}
				else
				{
					GPRS_Cancel_Send();
					
					if((i>=1) || (GPRS.b.Connected_OK_Flag == false))						// ������Ͳ���ȥ,�Ҵ����ﵽ2��
					{
						GPRS.b.Connected_OK_Flag = false;cnt_1s.GPRS_Reconnect_Cnt++;Task_AT_GSM(Gprs_ReConnect_Now,0,0,0,0,0,0);break;
					}
				}
			}
			else																			// ������ӳ����쳣
			{
				GPRS_Cancel_Send();
				
				if((i>=1) || (GPRS.b.Connected_OK_Flag == false))																	
				{
					GPRS.b.Connected_OK_Flag = false;cnt_1s.GPRS_Reconnect_Cnt++;Task_AT_GSM(Gprs_ReConnect_Now,0,0,0,0,0,0);break;
				}
			}
		}		
	}
		
	if(((GPRS.b.Connected_OK_Flag == false) || (i==200)) && Save) 							// �ѻ�ȡ�������/�����缱�˳�����/�ж���CMD-T��ʷ����δ������,��Ҫ�ٷ�CMD-T��Ϣ,������Ҫ��������
	{
		if(((GPRS_CMD!=CMD_T) && (GPRS_CMD!=CMD_X) && (GPRS_CMD!=ALM_E)) || ((GPRS_CMD==CMD_T) && (gpsx.AV_indicate=='A')))
		{
			memcpy(&gpsx_his_write,&gpsx,gpsx_All_Size);
			gpsx_his_write.cmd_code = GPRS_CMD;											
			Task_SPI_Flash(Flash_Write_History,0,0,0);	
		}
	}
	
	if((GPRS.b.Connected_OK_Flag) && (!CALL.b.Ring_Flag) && (!CALL.b.Calling_Flag))
	{
		if((GPRS.b.GPRS_send_history_Flag == true) && (GPS_Type == HISGPS))
		{
			if(++GSM_P.send_history >= GSM_P.store_history)															
			{
				GSM_P.send_history=0;GSM_P.store_history=0;GPRS.b.GPRS_send_history_Flag = false;	
				Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);	
			}
			else
			{
				Task_SPI_Flash(Flash_Read_History,0,0,0);
			}
		}
		else if(GPRS.b.GPRS_send_history_Flag == false)
		{
			if(GSM_P.send_history < GSM_P.store_history)									
			{
				Task_SPI_Flash(Flash_Read_History,0,0,0);	
			}
		}
	}
	else
	{
		if(GPRS.b.GPRS_send_history_Flag == true)
		{
			GPRS.b.GPRS_send_history_Flag = false;
			Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);	
		}
	}
}	

/*GPRS����*/
#if MODULE_UG96
bool QIACT_P(void)
{
	return(GSM_AT_Send("+QIACT=1",6,150,1,0x0001));
}
#elif MODULE_SIM7500
bool NETOPEN_P(void)
{
	return(GSM_AT_Send("+NETOPEN",21,150,1,0x0001));
}
#endif

void UTC(void)
{
	char *p1;
	type_gsm GSM_UTC;
	
	xQueueReceive(Queue_GSM,(u8*)&GSM_UTC,3000);
	
	GPRS.b.Chk_Ciprxget_Flag = false;
	
#if MODULE_UG96
	GSM_AT_Send("+QIRD=1,100",67,1,1,0x0001);
#elif MODULE_SIM7500
	GSM_AT_Send("+CIPRXGET=2,0,100",67,1,1,0x0001);
#endif	

#if MODULE_UG96
	p1=strstr(gsm_rx_buf,"+QIRD:");
#elif MODULE_SIM7500
	p1=strstr(gsm_rx_buf,"+CIPRXGET");
#endif

	if(p1 != NULL)
	{
		gpsx.posslnum = 1;						// ����GPS���յ�����
		
		p1=strstr(gsm_rx_buf,"-");
		
		if(p1 != NULL)
		{
			snprintf(GPS_RMC_Buf,80,"RMC,%c%c%c%c%c%c,,,,,,,,%c%c%c%c%c%c,",*(p1+7),*(p1+8),*(p1+10),*(p1+11),*(p1+13),*(p1+14),*(p1+4),*(p1+5),*(p1+1),*(p1+2),*(p1-2),*(p1-1));			
			NMEA_GPRMC_Analysis();
		}
	}	
}

/*��ѯ��ǰGPRS����״̬, ����: 1: ��Ҫ��ʱ�ȴ�*/
bool GPRS_Check(void)
{											
#if MODULE_UG96
	return(GSM_AT_Send("+QISTATE=1,1",6,2,1,0x4000));
#elif MODULE_SIM7500
	return(GSM_AT_Send("+CIPOPEN?",40,2,1,0x4000));
#endif
}

/*�ر�GPRS����*/
void GPRS_Disconnect(void)
{
	if(GPRS.b.GPRS_send_history_Flag == true)
	{
		GPRS.b.GPRS_send_history_Flag = false;Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);	
	}
		
#if MODULE_UG96
	GSM_AT_Send("+QICLOSE=1",6,20,1,0x0800); 
	GSM_AT_Send("+QIDEACT=1",6,20,1,0x0800); 
#elif MODULE_SIM7500
	GSM_AT_Send("+CIPCLOSE=0",22,1,1,0x0800); 
	GSM_AT_Send("+NETCLOSE",22,1,1,0x0800); 
#endif
}	

/*��ʼ����GPRS�������������*/
bool GPRS_APN_Connect(void)
{
	u8 i;
	bool res;
	
	for(i=0;i<3;i++)
	{
#if MODULE_UG96
		snprintf(dtbuf,90,"+QICSGP=1,1,\"%s\",\"%s\",\"%s\",1",GSM_P.apn,GSM_P.APN_username,GSM_P.APN_password);
#elif MODULE_SIM7500
		snprintf(dtbuf,90,"+CGDCONT=1,\"IP\",\"%s\"",GSM_P.apn);
		GSM_AT_Send(dtbuf,6,5,1,0x0001);
		snprintf(dtbuf,90,"+CSOCKAUTH=1,3,\"%s\",\"%s\"",GSM_P.APN_username,GSM_P.APN_password);
#endif
		res=GSM_AT_Send(dtbuf,6,5,1,0x0001); 
		if(res == false) 
		{	
			vTaskDelay(10);
			
			if(find_str("ERR")) 
			{
				return 1;		
			}
		}
		else
		{
			i=3;
		}
	}
	
	if(res == false){return 0;	}
	else			{return 1;	}
}	

/*����GPRS����*/
bool GPRS_TCPIP_Connect(unsigned char modeflag)
{
#if !MODULE_SIM7500
	EventBits_t Result;
#endif
	
#if MODULE_UG96
	if(QIACT_P() == 0) 		{return 0;				}
	if(modeflag == 2)		{vTaskDelay(500);		}
#elif MODULE_SIM7500
	if(NETOPEN_P() == false){return 0;				}	
#endif
	
	if(modeflag == 2)																						// NIST����
	{																
#if MODULE_UG96
		snprintf(dtbuf,80,"+QIOPEN=1,1,\"TCP\",\"time.nist.gov\",13,0,0");										
#elif MODULE_SIM7500
		snprintf(dtbuf,80,"+CIPOPEN=0,\"TCP\",\"time.nist.gov\",13");												
#endif
	}
	else if(modeflag == 1)																					// ��������
	{								
#if MODULE_UG96
		snprintf(dtbuf,80,"+QIOPEN=1,1,\"%s\",\"%s\",%s,0,0",GSM_P.TCP_UDP,GSM_P.domain_name,GSM_P.domain_port);				
#elif MODULE_SIM7500
		snprintf(dtbuf,80,"+CIPOPEN=0,\"%s\",\"%s\",%s",GSM_P.TCP_UDP,GSM_P.domain_name,GSM_P.domain_port);									
#endif
	}
	else																									// IP����
	{						
#if MODULE_UG96
		snprintf(dtbuf,80,"+QIOPEN=1,1,\"%s\",\"%s\",%s,0,0",GSM_P.TCP_UDP,GSM_P.ip_addr,GSM_P.ip_port);					
#elif MODULE_SIM7500
		snprintf(dtbuf,80,"+CIPOPEN=0,\"%s\",\"%s\",%s",GSM_P.TCP_UDP,GSM_P.ip_addr,GSM_P.ip_port);										
#endif
	}

#if MODULE_SIM7500
	
	if(GSM_AT_Send(dtbuf,23,10,1,0x0008))		
	{
		if(modeflag != 2){GPRS.b.Connected_OK_Flag = true;}
		
		return 1;	
	}
	else						
	{
		return 0;
	}
		
#elif MODULE_UG96

	if(GSM_AT_Send(pbuf1,6,150,1,0x0001) == false)		{return 0;	}
	
	GPRS.b.Chk_Connected_Flag = true;
	Result = xEventGroupWaitBits(EventGroupHandler_OTH,Gprs_Connected_OK_Flag|Gprs_Connected_NG_Flag, pdFALSE, pdFALSE,30000);
	GPRS.b.Chk_Connected_Flag = false;
	
	xEventGroupClearBits(EventGroupHandler_OTH,Gprs_Connected_OK_Flag|Gprs_Connected_NG_Flag);
	
	if(Result & Gprs_Connected_OK_Flag)	
	{
		if(modeflag != 2){GPRS.b.Connected_OK_Flag = true;}
		
		return 1;	
	}
	else
	{
		return 0;									// ���򱨴�
	}
		
#endif
}	

/*����GPRS,����AT+CIPCSGP ��AT+CIPSTART*/
// Reset_UTC 
//           0:�Ȳ�ѯGPRS����״̬,���ʧ������������; 
//			 1:����ѯGPRS״̬,ǿ����������; 
//			 2:���ӵ�NIST��վ,��ȡUTCʱ��
bool GPRS_Connect(unsigned char Reset_UTC)
{	 
	u8 i;
	
	if((GSMA.b.GPRS_flag == false)&&(Reset_UTC != 2))									// ���δ����GPRS����,��ֱ�ӷ���			
	{
		return true;	
	}	
					
	if((Reset_UTC==0)&&(GPRS_Check()==true))											// �����ǿ������,�򲻲���GPRS״̬;����ȴ�1��,��������������˳�,������������
	{
		cnt_1s.HeartBeat_Cnt=0;
		GPRS.b.Chk_Ciprxget_Flag = true;
		GPRS.b.Connected_OK_Flag = true;return true;									
	}
	else
	{	
		GPRS.b.Connected_OK_Flag = false;
		
		if(Reset_UTC == 2)																// ��Ҫ��ȡUTCʱ��
		{
			bool GPRS_Temp_Flag = GSMA.b.GPRS_flag;
			
			GSMA.b.GPRS_flag = 1;
			
			GPRS.b.Chk_Ciprxget_Flag = true;											// Ԥ����������ƽ̨����
					
			GPRS_Disconnect();															// ��ɾ��ԭ����

			if(GPRS_APN_Connect())														// ����ģ�鰴��������
			{
				GPRS_TCPIP_Connect(2);													// ��������ɹ�
			}
			
			UTC();		
			GPRS_Disconnect();
			GSMA.b.GPRS_flag = GPRS_Temp_Flag;
			return false;
		}
		//wifi����
		else if((strlen(GSM_P.domain_name)>2)&&(strlen(GSM_P.domain_port)>2))			// ��������Ͷ˿���������
		{
			for(i=0;i<2;i++)
			{
				if(CALL.b.Ring_Flag){break;} 											// �е绰������а�������Ҫ����,�������˳�
				
				GPRS_Disconnect();														// ��ɾ��ԭ����

				if(GPRS_APN_Connect())													// ����ģ�鰴��������
				{
					if(GPRS_TCPIP_Connect(1)){i=3;break;	}							// ��������ɹ�
				}
			}		
		}																
		else if((strlen(GSM_P.ip_addr)>2)&&(strlen(GSM_P.ip_port)>2))					// ��TCP/IP����			
		{
			for(i=0;i<2;i++)
			{
				if(CALL.b.Calling_Flag){break;} 										// �е绰������а�������Ҫ����,�������˳�
				
				GPRS_Disconnect();

				if(GPRS_APN_Connect())
				{
					if(GPRS_TCPIP_Connect(0)){i=3;break;	}
				}			
			}
		}
		else																			// ���������IP��δ����,��ر�GPRS,��������
		{
			GSMA.b.GPRS_flag=false;return 0;
		}		
	}
	
	if(GPRS.b.Connected_OK_Flag)														// ��������ɹ�
	{
		if(GPRS_Check()==true)
		{	
			cnt_1s.HeartBeat_Cnt=0;
			GPRS.b.Chk_Ciprxget_Flag = true;											// ��������ƽ̨����
			Send_Msg_To_Platform(CMD_D,NOGPS,MSG_NO,No_Save);							// ����ƽ̨GPRS��������Ϣ
			
			if(SMS.b.CMDD_SMS_Flag)														// �ظ�����
			{
				SMS.b.CMDD_SMS_Flag = false;
				Send_Msg_To_Phone(SMS_Phone_Number_Backup,"Opened GPRS Ok!",NOGPS,MSG_NO);
			}
			
			return true;
		}
		else
		{
			GPRS.b.Connected_OK_Flag = false;return false;
		}
	}		
	else
	{		
		GPRS_Disconnect();
		
		if(SMS.b.CMDD_SMS_Flag)															// �ظ�����
		{
			SMS.b.CMDD_SMS_Flag = false;
			Send_Msg_To_Phone(SMS_Phone_Number_Backup,"Connect Fail,Please Reinstall APN,IP&Port!",NOGPS,MSG_NO);
		}
		
		return false;
	}
}

/*��GPRS����*/
void GPRS_Data_Handle(void)
{	
	char *p1;
	unsigned char count_error=0;
	
#if CIPRXGET_N																	
	char buf123[101];
#endif		

	cnt_1s.Chk_GPRS_Cnt = 0;
	GPRS.b.Chk_Ciprxget_Flag = false;													
	
rx_again:	

#if MODULE_UG96
	if(GSM_AT_Send("+QIRD=1,100",18,1,1,0x0003))
#elif MODULE_SIM7500
	if(GSM_AT_Send("+CIPRXGET=2,0,100",15,1,1,0x0003))
#endif	
	{
		
#if RESET_FUN
		 
		if(strstr(gsm_rx_buf,"RESET!1")!=NULL) 
		{	
			Task_SPI_Flash(Flash_MCU_Reset,0,0,0);										
		}
		else if(strstr(gsm_rx_buf,"RESET!")!=NULL) 
		{
			Task_SPI_Flash(Flash_Reset_ALL,0,0,0);									// ���ò�����λ
		}
		
#endif

		p1=strstr(gsm_rx_buf,":123456");
		
#if CIPRXGET_N							
		
		if(p1 != NULL)
		{
			snprintf(buf123,101,"%s",p1);snprintf(gsm_rx_buf,101,"%s",buf123);	
			
			p1 = strstr(gsm_rx_buf,":123456");
			
			while(1)		
			{	
				p1 = strstr(p1,":123456");
				
				if(p1 != NULL)
				{
					strtok(p1,"\r");strtok(p1,"#");
					analysis_sms_content(p1 + 7, RPLY_TO_GPRS);
					snprintf(gsm_rx_buf,101,"%s",buf123);					
					p1 = p1 + 7;
				}
				else
				{
					if(strlen(buf123)  < 100)	{break;			}
					else						{goto rx_again;	}
				}
			}
		}

#else

		if(p1!=NULL)		
		{
			analysis_sms_content(p1+7, RPLY_TO_GPRS);	
		}
		
#endif	
	}
#if MODULE_SIM7500
	else if(find_str("No data"))
	{
		;
	}
#endif
	else if(find_str("ERROR"))		
	{
		count_error+=1;
		if(count_error<2)
		{
			goto rx_again;
		}
		else
		{	
			GPRS.b.Connected_OK_Flag = false;				// �Ѿ�����
			Task_AT_GSM(Gprs_ReConnect_Now,0,0,0,0,0,0);	
			return;
		}
	}
	
	GPRS.b.Chk_Ciprxget_Flag = true;
}

#endif

/*������Ϣ����  ��Ҫ�����ָ��/������ɺ�ظ�ͨ��*/
void analysis_sms_content(char *Str,unsigned char Reply)
{ 
	if(Str!=NULL)
	{						 			
		switch(Str[0])										// �жϵ�һ���ַ�
		{
		#if (RESET_FUN==1)									// �������ûָ�����Ĭ������ 
			case '1': CMD_1_Handle(Str+1,Reply);break;		// ʾ��: SET1
			case '2': CMD_2_Handle(Str+1,Reply);break;		// ʾ��: SET2				
		#endif	
		
		#if (GPRS_FUN == 1)
			case '4': CMD_4_Handle(Str+1,Reply);break;		// ��GPRS����,ʾ��: SET4
			case '5': CMD_5_Handle(Str+1,Reply);break;		// �ر�GPRS����,ʾ��: SET5
		#endif
				
		#if (A_FUN==1)										// ������Ȩ1-8����,  		
			case 'A': CMD_A_Handle(Str+1,Reply);break;		// ʾ��: 123456A1,+8613145906426   ɾ������123456A1,D	
		#endif
			
		#if (B_FUN==1)										// ��ȡIMEI��					
			case 'B': CMD_B_Handle(Str+1,Reply);break;		// ʾ��: 123456B		 
		#endif
		
		#if GPRS_ENABLE									 
		#if (C_FUN==1)   									// �����������Ӳ��� 			    				 				
			case 'C': CMD_C_Handle(Str+1,Reply);break;		// ʾ��: 123456CCMNET,183.12.181.141:19003 	 
		#endif	
				
		#if (D_FUN==1)										// ���ſ���GPRS									
			case 'D': CMD_D_Handle(Str+1,Reply);break;		// ʾ��: 123456D	
		 #endif									 
			
		#if (E_FUN==1)										// ���Ż�GPRS�ر�GPRS  											 
			case 'E': CMD_E_Handle(Str+1,Reply);break;		// ʾ��: 123456E	
		#endif	
				
		#endif						
																 
		#if (F_FUN==1)										// ���ζ�λ 									 
			case 'F': CMD_F_Handle(Str+1,Reply);break;		// ʾ��: 123456F		
		#endif	

		#if (G_FUN==1)										// ��ȡ�û����õĲ��� 		         
			case 'G': CMD_G_Handle(Str+1,Reply);break;		// ʾ��: 123456G 	
		#endif	
									
		#if (H_FUN==1)										// ������Ȩ���� 			 											
			case 'H': CMD_H_Handle(Str+1,Reply);break;		// ʾ��: 123456H999999					           		
		#endif
								 
		#if (I_FUN==1)										// ���þ��ε���Χ������  	
			case 'I': CMD_I_Handle(Str+1,Reply);break;		// ʾ��123456I1,1,1,51113525N009125670E50241115N011011173E
		#endif
									 
		#if (J_FUN==1)										// ���ó��ٱ���								 
			case 'J': CMD_J_Handle(Str+1,Reply);break;		// ʾ��: 123456J1,080
		#endif
								
		#if (K_FUN==1)										// �رյ�Դ,����BMA250	
			case 'K': CMD_K_Handle(Str+1,Reply);break;		// ʾ��: 123456KH,050	123456KL,050	123456K				 
		#endif 										
							 
		#if (L_FUN==1)										// ���ñ���ʱ��,����LED��ʾʱ��					 
			case 'L': CMD_L_Handle(Str+1,Reply);break;		// ʾ��: 123456L+08 		123456L��170101��120000			 
		#endif

		#if (M_FUN==1)										// ���ó�����λ����,����ʱ��															
			case 'M': CMD_M_Handle(Str+1,Reply);break;		// ʾ��: 123456M1,030S	123456M2��120		
		#endif
				
		#if (N_FUN==1)										// ���õ͵籨��  									
			case 'N': CMD_N_Handle(Str+1,Reply);break;		// ʾ��: 123456N1,40		
		#endif	

		#if (O_FUN==1)										// �����û���������											
			case 'O': CMD_O_Handle(Str+1,Reply);break;		// ʾ��: 123456Ointernet,internet123			
		#endif	

		#if (P_FUN==1)										// ����Բ�ε���Χ�� 									
			case 'P': CMD_P_Handle(Str+1,Reply);break;		// ʾ��: 123456P1��020
		#endif	
								
		#if (Q_FUN==1)										// ����SOS�Ƿ��绰				
			case 'Q': CMD_Q_Handle(Str+1,Reply);break;		// ʾ��: 123456Q1	123456Q0	
		#endif										
							 
		#if (R_FUN==1)										// �����������˿�					
			case 'R': CMD_R_Handle(Str+1,Reply);break;		// ʾ��: 123456Rhttp:160162yx73.imwork.net;1290								
		#endif	

		#if (S_FUN==1)											
			case 'S': CMD_S_Handle(Str+1,Reply);break;		// ���ÿ�����رմ��ڵ���							
		#endif	

		#if (T_FUN==1)
			case 'T':							break;				 				 
		#endif		 

		#if (U_FUN==1)										// ���ü�����ͨ����GPS����ģʽ��									
			case 'U': CMD_U_Handle(Str+1,Reply);break;		// ʾ��: 123456U0	123456UV 	123456UV01 
		#endif	
						 
		#if (V_FUN==1)										// ��ָ�����绰,����ڰ˸�����,V31�������,V30ͨ�����  	 					 
			case 'V': CMD_V_Handle(Str+1,Reply);break;		// ʾ��: 123456V30,9963	123456V31,9963 	
		#endif	      								 

		#if (W_FUN==1)																		
			case 'W': CMD_W_Handle(Str+1,Reply);break;		//	TIM-�����õ�WIFI-msg�������ṹ��
		#endif	
	 
		#if (X_FUN==1)										 
			case 'X':						 	break;
		#endif	
				
		#if (Y_FUN==1)										 
			case 'Y':						 	break;
		#endif	
		
		#if (Z_FUN==1)										 
			case 'Z': CMD_Z_Handle(Str+1,Reply);break;
		#endif	
		
			default:break;						
		 }						       
	}
}


/*������Ϣ�����ڡ����š������� */
/*PC_CMD--�������ڵ�����, SMS_CMD ������������, GPRS_CMD���͵�GPRS����, BackAndGPSType����ͨ��+GPS��ȡ��,SMS_MSG_Type���ŷ��͸�ʽ,GPRS_MSG_Type GPRS���͸�ʽ, Save���������Ƿ���Ҫ����*/
void send_info(char *PC_CMD,char *SMS_CMD,u8 GPRS_CMD,u8 BackAndGPSType,u8 SMS_MSG_Type,bool Save)
{
	switch(BackAndGPSType&0x70)
	{
		case RPLY_TO_PC: //��������
			printf("%s",PC_CMD);
			break; 

#if SMS_ENABLE
		case RPLY_TO_SMS: //�յ�����ָ���	
			Send_Msg_To_Phone(SMS_Phone_Number,SMS_CMD,BackAndGPSType&0x0F,SMS_MSG_Type);  
			break;	
#endif

#if GPRS_ENABLE
		case RPLY_TO_GPRS://�յ�GPRSָ���
			Send_Msg_To_Platform(GPRS_CMD,BackAndGPSType&0x0F,MSG_GPRS,Save);
			break;
#endif

		default:break;	 					
	}			  	
}

/* ����������λ����ģʽ	*/
void Set_Track_Time(void)
{	
	switch(GSM_P.Track_Set_Unit)
	{		 
		case 'S': cnt_1s.Track_Time_Set=1;   break;		 
		case 'M': cnt_1s.Track_Time_Set=60;  break;
		case 'H': cnt_1s.Track_Time_Set=3600;break;
		default:break;							 
	}	
	
	cnt_1s.Track_Time_Set = GSM_P.Track_Set_Time * cnt_1s.Track_Time_Set;	// ��ȡʵ������ֵ
	
	cnt_1s.Track_Time_Cnt=0;SMS.b.Track_SMS_Flag = false;GPRS.b.Track_GPRS_Flag = false;
		
	GPS_F.b.Track_For_sms_Flag = false; GPS_F.b.Track_For_gprs_flag = false;
	
	cnt_1s.Track_Save_Cnt=0;GPRS.b.GPRS_Save_now_Flag = false;
	
	cnt_1s.Track_wifi_Cnt = 0;
				
	if((cnt_1s.Track_Time_Set > 0) && GSMA.b.Track_Flag)					// �����ÿ���������λʱ��
	{		
		if(cnt_1s.Track_Time_Set <= 600)									// ���������λʱ��С��10����,ÿ��GPS��Դ����ʱ��Ϊ5��							
		{
			Track_Time_Unit = 5;
		}
		else																// ���������λʱ�����10����,ÿ�ΰ�15������	
		{
			Track_Time_Unit = 15;
		}
						
		if(GSMA.b.GPRS_flag)												// ����ѿ���GPRS����
		{							
			GPRS.b.Track_GPRS_Flag = true;		
		}	
		else
		{					
			SMS.b.Track_SMS_Flag = true;

			if(cnt_1s.Track_Time_Set < 20)
			{
				cnt_1s.Track_Time_Set = 20;
			}
		}
	}
	else
	{
		GSMA.b.Track_Flag = 0;cnt_1s.Track_Time_Set = 0;
	}
	
	if(GSMA.b.Track_Flag && (cnt_1s.Track_Time_Set <= 20))	{GPS_Power_On();	}
	else													{GPS_Power_Off();	}
}

/* �������ûָ���������	*/
void CMD_1_Handle(char *Str1,unsigned char Reply1)
{
	if(Reply1 == RPLY_TO_PC)
	{
		printf(" OK!\r\n");
		Task_SPI_Flash(Flash_Reset_ALL,0,0,0);
	}
}

/* ��������				*/
void CMD_2_Handle(char *Str2,unsigned char Reply2)
{
	if(Reply2 == RPLY_TO_PC)
	{
		printf(" OK!\r\n");
		Task_SPI_Flash(Flash_MCU_Reset,0,0,0);
	}
}

/* ����GPRS����			*/
void CMD_4_Handle(char *Str4,unsigned char Relay4)
{
	if(Relay4 == RPLY_TO_PC)                   							
	{	
		printf(" OK!\r\n");
		GSMA.b.GPRS_flag=1;							
		Task_SPI_Flash(Flash_Write,0,(u8*)&GSMA.w,1);
		Task_AT_GSM(Gprs_Disconnect,0,0,0,0,0,0);
		Task_AT_GSM(Gprs_ReConnect_Now,0,0,0,0,0,0);
		Set_Track_Time();
	}	
}

/* �ر�GPRS����			*/
void CMD_5_Handle(char *Str5,unsigned char Relay5)
{
	if(Relay5 == RPLY_TO_PC)								 
	{
		printf(" OK!\r\n");
		GSMA.b.GPRS_flag=0;
		GPRS.b.Connected_OK_Flag = false;
		Task_SPI_Flash(Flash_Write,0,(u8*)&GSMA.w,1);	
		Task_AT_GSM(Gprs_Disconnect,0,0,0,0,0,0);
		Set_Track_Time();
	}
}

/*������Ȩ���롢ɾ����Ȩ����*/
void CMD_A_Handle(char *StrA,unsigned char ReplyA)
{ 
	bool Err = true;
	
	if((StrA[0] >= '1') && (StrA[0] <= '8') && (StrA[1] == ','))	
	{
		char *Telphone_Addr,*Telphname_Addr;
		char Reply_Data[32];

		switch(StrA[0])
		{
			case '1':	Telphone_Addr=GSM_ph.temp_ph1;Telphname_Addr=GSM_phname.name_ph1;break;
			case '2':	Telphone_Addr=GSM_ph.temp_ph2;Telphname_Addr=GSM_phname.name_ph2;break;
			case '3':	Telphone_Addr=GSM_ph.temp_ph3;Telphname_Addr=GSM_phname.name_ph3;break;
			case '4':	Telphone_Addr=GSM_ph.temp_ph4;Telphname_Addr=GSM_phname.name_ph4;break;
			case '5':	Telphone_Addr=GSM_ph.temp_ph5;Telphname_Addr=GSM_phname.name_ph5;break;
			case '6':	Telphone_Addr=GSM_ph.temp_ph6;Telphname_Addr=GSM_phname.name_ph6;break;
			case '7':	Telphone_Addr=GSM_ph.temp_ph7;Telphname_Addr=GSM_phname.name_ph7;break;
			case '8':	Telphone_Addr=GSM_ph.temp_ph8;Telphname_Addr=GSM_phname.name_ph8;break;
		}	

		if(StrA[2]=='D')												
		{
			if(ReplyA == RPLY_TO_SMS)									
			{
				snprintf(Reply_Data,32,"%cTEL OK!%s;",StrA[0],Telphone_Addr);
			}
			else if(ReplyA == RPLY_TO_PC)								
			{
				snprintf(Reply_Data,32,"%cTEL OK!",StrA[0]);
			}
			
			Err = false;
			sprintf(Telphone_Addr,"%s","");sprintf(Telphname_Addr,"%s","");					
		}
		else															
		{
			char *P1;
			
			P1 = strstr(StrA+2,",");
			
			if(P1 != NULL)	{*P1 = '\0'; P1++;}
			
			if((StrA[2] == '+') || isdigit(StrA[2]))	
			{
				if(Isdigit(StrA+3,strlen(StrA+3)) == true)
				{
					Err =false;

					snprintf(Telphone_Addr,24,"%s",StrA+2);
					
					if(ReplyA == RPLY_TO_SMS)								
					{
						snprintf(Reply_Data,32,"%cTEL OK!%s;",StrA[0],Telphone_Addr);
					}
					else if(ReplyA == RPLY_TO_PC)							
					{
						snprintf(Reply_Data,32,"%cTEL OK!",StrA[0]);
					}
					
					if(P1 != NULL)
					{
						snprintf(Telphname_Addr,13,"%s",P1);
					}
					else
					{
						sprintf(Telphname_Addr,"%s","");		
					}
				}
			}
		}
		
		if(Err == false)
		{
			send_info(Reply_Data,Reply_Data,CMD_A1+(StrA[0]&0x0F)-1,ReplyA|OLDGPS,MSG_NO,Do_Save);// �Ȼظ���Ϣ
			Task_SPI_Flash(Flash_Write,phoneaddr,		(u8*)&GSM_ph,		GSM_ph_size);
			Task_SPI_Flash(Flash_Write,phonenameaddr,	(u8*)&GSM_phname,	phonenamesize);
		}
	}
	
	if(Err == true)																															// ����ظ�ָ�����
	{
		send_info(" ERROR!","error phone number!",CMD_ERR,ReplyA|OLDGPS,MSG_NO,Do_Save);
	}
}

/* ����汾��Ϣ*/
void CMD_B_Handle(char *StrB,unsigned char ReplyB)
{ 
	if(StrB[0] == '1')
	{
		send_info(company1,company1,CMD_ERR,ReplyB|NOGPS,MSG_NO,No_Save);
	}
	else
	{
		char pbuf1[52]={0};
		snprintf(pbuf1,52,"IMEI:%s,%s",GSM_P.IMEI,HS_VER);		
		send_info(pbuf1,pbuf1,CMD_B,ReplyB|NOGPS,MSG_NO,No_Save);
	}
}

/* ����GPRS APN, IP, Port ��ַ*/
void CMD_C_Handle(char *StrC,unsigned char ReplyC)			// SETCapn,x.x.x.x:yyyy
{
	bool Err=false;
	bool TCP=true;
	char *p1,*p2,*p3;
	unsigned char i,Di=0,Dp=0,Dp_Cnt=0;
		
	p1=StrC;												// ��ȡAPN
	
	p2=strchr(StrC,',');									// 
	
	if(((*(p2+1) == 'u') || (*(p2+1) == 'U')) && (*(p2+2) == ','))
	{
		TCP = false;p2=p2+2;
	}

	p3=strchr(StrC,':');									// 
	
	if((p2 == NULL) && (p3 == NULL))						// ֻ����APN
	{
		snprintf((char*)GSM_P.apn,30,"%s", p1);				// ��ౣ��20���ַ�
		GSM_P.ip_addr[0]='\0';	
		GSM_P.ip_port[0]='\0';
	}
	else													// Ҫ����APN��IP��PORT
	{	
		p2++;p3++;											// ����','֮����ַ�IP��ַ,����":"֮����ַ��˿ں�
		
		if(p1[0]==',')										// APN����Ϊ��
		{
			GSM_P.apn[0]='\0';	
		}				
		else												// ��APN����			
		{
			strtok(p1,",");
			snprintf((char*)GSM_P.apn,30,"%s", p1);			// ��ౣ��10���ַ�
		}					
		
		if(p2[0]==':')										// IP��ַΪ��	
		{
			GSM_P.ip_addr[0]='\0';	
		}					
		else												// IP��ַ������			
		{
			strtok(p2,":");
			if(strlen(p2)<16)								// IP����С��16
			{
				for(i=0;i<strlen(p2);i++)					// ���IP
				{
					if(isdigit(p2[i]))						// ��鵽����
					{
						Di++;Dp=0;							// ����ͳ��,С������0
						if(Di > 3) {Err=true;break;}		// �������ֲ�����3��
					}  
					else if(p2[i] == '.') 
					{
						Dp++;Di=0;Dp_Cnt++;					// С����ͳ��,����ͳ����0
						if(Dp > 1) {Err=true;break;}		// ����С���㳬��1��
					}			
					else									// �����ּ�С����
					{
						Err=true;break;	
					}
				}
				if(Dp_Cnt != 3)								// С��������������3
				{
					Err = true;
				}
				if(Err == false)
				{
					snprintf((char*)GSM_P.ip_addr,18,"%s",p2);// ��ౣ��15���ַ�
				}
			}
			else
			{
				Err=true;
			}	
		}					
		
		if(p3[0] != '\0')
		{
			if(strlen(p3) < 6) 								// ���˿ں��ַ�����
			{
				for(i=0;i<strlen(p3);i++)
				{
					if(!isdigit(p3[i])) {Err=true;break;}	// �����֣�����
				}
				if(atoi(p3) > 65535)	{Err=true;		}	// ���ݴ���65535������
				if(Err == false)
				{
					snprintf((char*)GSM_P.ip_port,6,"%s",p3);
				}
			}
			else
			{
				Err=true;
			}
		}
		else
		{
			GSM_P.ip_port[0]='\0';
		}
	}

	if(Err == false)
	{
		if(TCP == true)	{snprintf(GSM_P.TCP_UDP,4,"%s","TCP");}
		else			{snprintf(GSM_P.TCP_UDP,4,"%s","UDP");}
		Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);	
		send_info(" OK!","SET APN,IP&Port OK!",CMD_C,ReplyC|OLDGPS,MSG_NO,Do_Save);
	}
	else
	{
		send_info(" ERROR!","SET APN,IP&Port ERROR!",CMD_ERR,ReplyC|OLDGPS,MSG_NO,Do_Save);
	}
}

/* ����GPRS */
void CMD_D_Handle(char *StrD,unsigned char ReplyD)
{
	if((ReplyD != RPLY_TO_PC) && (StrD[0] == '\0'))                  							
	{	
		GSMA.b.GPRS_flag=1;	
		Task_SPI_Flash(Flash_Write,0,(u8*)&GSMA.w,1);	
		Task_AT_GSM(Gprs_Disconnect,0,0,0,0,0,0);
		Task_AT_GSM(Gprs_ReConnect_Now,0,0,0,0,0,0);
		Set_Track_Time();	
		if(ReplyD == RPLY_TO_SMS) 
		{
			strcpy(SMS_Phone_Number_Backup,SMS_Phone_Number);	// ���ƺ���
			SMS.b.CMDD_SMS_Flag = true;
		}
	}
	else
	{
		send_info(" ERROR!","",CMD_ERR, ReplyD|NOGPS,MSG_NO,Do_Save);									 
	}
}

/* �ر�GPRS */
void CMD_E_Handle(char *StrE,unsigned char ReplyE)
{
	if((ReplyE != RPLY_TO_PC) && (StrE[0] == '\0')) 								 
	{
		GSMA.b.GPRS_flag=0;	
		GPRS.b.Connected_OK_Flag = false;			
		Task_SPI_Flash(Flash_Write,0,(u8*)&GSMA.w,1);		//�������GPRS_flag 
		Task_AT_GSM(Gprs_Disconnect,0,0,0,0,0,0);	
		Set_Track_Time();
		if(ReplyE != RPLY_TO_GPRS)
		{
			send_info("","Closed GPRS OK!",CMD_ERR,ReplyE|NOGPS,MSG_NO,No_Save);	
		}
	}
}

/* ���ζ�λ */
void CMD_F_Handle(char *StrF,unsigned char ReplyF)
{
	if(StrF[0] == '\0')
	{
		if(ReplyF == RPLY_TO_SMS)		
		{
			strcpy(SMS_Phone_Number_Backup,SMS_Phone_Number);		// ���ƺ���
			SOS_BMA_F_GPS_Start(1);
			Set_Track_Time();
		}
		else if(ReplyF == RPLY_TO_GPRS)
		{
			SOS_BMA_F_GPS_Start(2);
			Set_Track_Time();
		}
	}
#if MODULE_SIM7500 && GSM_Positon
	else if((StrF[0] == '1') && (ReplyF != RPLY_TO_PC))
	{
		if(!GSMA.b.GPRS_flag){GPRS_APN_Connect();}
		
		GSM_AT_Send("+CSOCKSETPN=1",6,5,1,0x0001); 
		
		GSM_AT_Send("+CNETSTART",23,5,1,0x0001); 
		
		if(GSM_AT_Send("+CLBS=1",14,10,1,0x0007))
		{
			send_info("","LBS!",CMD_F1,ReplyF|CMDFGPS,MSG_GOOGLE,Do_Save);
		}
		else
		{
			send_info("","LBS!",CMD_F1,ReplyF|OLDGPS,MSG_GOOGLE,Do_Save);
		}
		
		GSM_AT_Send("+CNETSTOP",6,5,1,0x0001); 		
	}
#endif
}

void CMD_G_Handle(char *StrG,unsigned char ReplyG)
{		
	if(ReplyG == RPLY_TO_GPRS) 								// ����������
	{
		send_info("","",CMD_G,ReplyG|OLDGPS,MSG_NO,Do_Save);return;
	}
		
	if(ReplyG == RPLY_TO_PC) 								// ��������
	{
		Task_SPI_Flash(Flash_SETG,0,0,0);
	}
	else if(ReplyG == RPLY_TO_SMS)							//��������									              
	{		
		snprintf(dtbuf,160,"A%s;B%s;C%s;D%s;E%s;F%s;G%s;H%s;%s;%s;%s;%s;%s;Speed:%d;%d;%d;%d;%d;%d;BAT:%d,%d%%,%d%c,Lg:%d,Hg:%d",\
		GSM_ph.temp_ph1,GSM_ph.temp_ph2,GSM_ph.temp_ph3,GSM_ph.temp_ph4,GSM_ph.temp_ph5,GSM_ph.temp_ph6,GSM_ph.temp_ph7,GSM_ph.temp_ph8,GSM_P.apn,GSM_P.APN_username,GSM_P.APN_password,
		GSM_P.ip_addr,GSM_P.ip_port,GSM_P.set_speed,GSMA.b.ALM_OverSpeed_Flag,GSMA.b.Track_Flag,
		(gpsx.AV_indicate=='A'? 1:0),GSMA.b.ALM_Circle_GEO_Flag,GSMA.b.ALM_LowBat_Flag ,gpsx.soc,GSM_P.set_soc&0x7F,GSM_P.Track_Set_Time,GSM_P.Track_Set_Unit,GSM_P.lowg,GSM_P.higg);
		send_info("",dtbuf,CMD_ERR,ReplyG|NOGPS,MSG_NO,Do_Save);	
	}		
}

/* ���ö�������*/
void CMD_H_Handle(char *StrH,unsigned char ReplyH)
{	
	if(Isdigit(StrH,6) == false)
	{
		send_info(" ERROR!","ERROR!",CMD_ERR,ReplyH|OLDGPS,MSG_NO,Do_Save);
	}
	else
	{
		StrH[6]='\0';
		snprintf(GSM_P.cmd_key,7,"%s",StrH);
		Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);
		send_info(" OK!","Change Password OK!",CMD_H,ReplyH|OLDGPS,MSG_NO,Do_Save);	
	}									 
}	

/* ����GEO */
void CMD_I_Handle(char *StrI,unsigned char ReplyI)
{
	bool err_data_flag=0;															// ���ݸ�ʽ�����־

	char *p1;
	char pbuf1[4]={0};	

	u8 row,GEO_OnOff;	
	u8 GEO_InOut_Flag=0;		
	u8 hi=0;	
	
	p1=StrI;
	
	row = *p1-0x30; 																// ���	
	if((row>5)||(row<1))		{err_data_flag = 1;	} 								// ��Ų�����5 
										
	p1+=2;
	if(*p1!='0' && *p1!='1' )	{err_data_flag = 1;	}
	else						{GEO_OnOff = *p1;	}								// GEO ����

	p1+=2; 																			// ָ���������
	if(*p1 ==0x30)				{GEO_InOut_Flag = 0;} 								// ���뱨��
	else if (*p1==0x31)			{GEO_InOut_Flag = 1;} 								// ��ȥ���� 
	else						{err_data_flag =1;	}

	if(GEO_OnOff == '1')															// ��������
	{ 
		p1+=2; 																		// ָ��γ�����ݵ�һλ
		for(hi=0;hi<38;hi++)
		{
			if(*(p1+hi)<0x30||*(p1+hi)>0x39)
			{
				if((hi!=8)&&(hi!=18)&&(hi!=27)&&(hi!=37))
				{err_data_flag=1; break;}	 
				if(*(p1+hi)!='W'&&*(p1+hi)!='E'&&*(p1+hi)!='N'&&*(p1+hi)!='S')
				{err_data_flag=1; break;}
			}													 
		}

		GEO_c[row-1].lat1=dms_to_d(p1,1);											// �Ѷȷ����ʽ��GPS����ת�ɶȸ�ʽ									 
		if(*(p1+8)=='S')  GEO_c[row-1].lat1=-GEO_c[row-1].lat1; 

		GEO_c[row-1].lot1=dms_to_d(p1+9,0);			
		if(*(p1+18)=='W') GEO_c[row-1].lot1=-GEO_c[row-1].lot1; 

		GEO_c[row-1].lat2=dms_to_d(p1+19,1);															 
		if(*(p1+27)=='S') GEO_c[row-1].lat2=-GEO_c[row-1].lat2;   

		GEO_c[row-1].lot2=dms_to_d(p1+28,0);														 
		if(*(p1+37)=='W') GEO_c[row-1].lot2=-GEO_c[row-1].lot2; 

	}
	
	if(!err_data_flag)
	{           										
		if(GEO_OnOff == 0x31)
		{ 						
			Task_SPI_Flash(Flash_Write,geoaddr,(u8*)&GEO_c[0],GEO_c_size);
			Task_SPI_Flash(Flash_Write,((row-1)*40+setgeoaddr),(u8*)p1,setgeo_size);
			GEOA.w|=1<<(row-1);														// ����GEO����
			GEOD.w|=1<<(row-1);														// ��Ч	 
			if(GEO_InOut_Flag)														// �߳�����
			{
				GEOB.w|=1<<(row-1);													// ����GEO��ȥ��־
				GEOC.w&=~(1<<(row-1));												// ����GEO�����־	
			}
			else																	// ���뱨��
			{
				GEOB.w&=~(1<<(row-1));												// ����GEO��ȥ��־
				GEOC.w|=1<<(row-1);													// ����GEO�����־
			}
		}									
		else
		{	
			Task_SPI_Flash(Flash_Write,(row-1)*40+setgeoaddr,(u8*)&pbuf1,setgeo_size);
			GEOA.w&=~(1<<(row-1));													// ����GEO���� 
			GEOD.w&=~(1<<(row-1));													// ��Ч		
			GEOB.w&=~(1<<(row-1));													// ����GEO��ȥ��־
			GEOC.w&=~(1<<(row-1));													// ����GEO�����־											
		}
		pbuf1[0]=GEOA.w;pbuf1[1]=GEOB.w;pbuf1[2]=GEOC.w;pbuf1[3]=GEOD.w;	
		Task_SPI_Flash(Flash_Write,3,(u8*)&pbuf1,4);
		
		GPS_F.b.Send_Geo1_ALM_flag = GEOA.b.GEO1_flag;								
		GPS_F.b.Send_Geo2_ALM_flag = GEOA.b.GEO2_flag;
		GPS_F.b.Send_Geo3_ALM_flag = GEOA.b.GEO3_flag;
		GPS_F.b.Send_Geo4_ALM_flag = GEOA.b.GEO4_flag;
		GPS_F.b.Send_Geo5_ALM_flag = GEOA.b.GEO5_flag;
		
		if(GEO_OnOff==0x31)
		{
			send_info(" OK!","GeoFence set OK",CMD_I,ReplyI|OLDGPS,MSG_NO,Do_Save);
		}
		else
		{
			send_info(" OK!","Geo fence deleted",CMD_I,ReplyI|OLDGPS,MSG_NO,Do_Save);
		}
	} 
	else
	{
		send_info(" ERROR!","Geofence setup failed",CMD_ERR,ReplyI|OLDGPS,MSG_NO,Do_Save);		
	}							
}

/* ���ó��ٱ��� */
void CMD_J_Handle(char *StrJ,unsigned char ReplyJ)
{
	bool Err=true;

	if(((StrJ[0] == '0' || StrJ[0] == '1'))&&(StrJ[1] == ','))
	{
		if(Isdigit(StrJ+2,strlen(StrJ+2)) == true)	
		{
			unsigned int i;
			i = Char_To_Dig(StrJ+2,strlen(StrJ+2));
			if((i >= 30) && (i <= 255)) 
			{
				GSM_P.set_speed=(u8)i;
				
				if(StrJ[0] == '1')	{GSMA.b.ALM_OverSpeed_Flag=1;	}				
				else				{GSMA.b.ALM_OverSpeed_Flag=0;	}	
				
				Err = false;
				GPS_F.b.Send_overspd_ALM_flag = false;
				Task_SPI_Flash(Flash_Write,0,(u8*)&GSMA.w,1);
				Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);
				send_info(" OK!","SET SPD OK!",CMD_J,ReplyJ|OLDGPS,MSG_NO,Do_Save);
			}
		}
	}

	if(Err == true)
	{
		send_info(" ERROR!","ERROR!",CMD_ERR,ReplyJ|OLDGPS,MSG_NO,Do_Save);								
	}	
}

/* ����BMA250���ػ� */
void CMD_K_Handle(char *StrK,unsigned char ReplyK)
{
	bool Err = true;

	if(StrK[0] == 'H')															// ����BMA��ײ��ֵKHxxx	
	{
		if(Isdigit(StrK+1,strlen(StrK+1)) == true)	
		{
			GSM_P.higg=Char_To_Dig(StrK+1,strlen(StrK+1));
			if(GSM_P.higg > 255){GSM_P.higg=255;}
			Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);
			if(GSM_P.higg == 0)													// �ر�HIG	
			{
				send_info(" OK!","Disabled High-g OK!",CMD_K2,ReplyK|OLDGPS,MSG_NO,Do_Save);
			}		
			else																// ����HIG				
			{
				send_info(" OK!","SET High-g Threshold OK!",CMD_K0,ReplyK|OLDGPS,MSG_NO,Do_Save);	
			}
			
			Err = false;
			
			BMA250_init();
		}
	}
	else if(StrK[0] == 'L')														// ����BMA����ֵKLxxx
	{
		if(Isdigit(StrK+1,strlen(StrK+1)) == true)
		{
			GSM_P.lowg=Char_To_Dig(StrK+1,strlen(StrK+1));
			if(GSM_P.lowg>255) {GSM_P.lowg=255;}
			Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);
			if(GSM_P.lowg == 0)													// �ر�LOW
			{
				send_info(" OK!","Disabled Low-g OK!",CMD_K3,ReplyK|OLDGPS,MSG_NO,Do_Save);								
			}		
			else																// ����LOW				
			{
				send_info(" OK!","SET Low-g Threshold OK!",CMD_K1,ReplyK|OLDGPS,MSG_NO,Do_Save);								
			}
			
			Err = false;
			
			BMA250_init();
		}
	}
	else if(StrK[0] =='\0')														// ����ػ�
	{
		Err = false;
		send_info(" OK!","Power Off OK!",CMD_K,ReplyK|OLDGPS,MSG_NO,Do_Save);
		vTaskDelay(1000);										
		Task_AT_GSM(Gsm_Pwr_off,0,0,0,0,0,0);	
	}
	
	if(Err == true)
	{
		send_info(" ERROR!","ERROR!",CMD_ERR,ReplyK|OLDGPS,MSG_NO,Do_Save);
	}
}


/* ����ʱ��, ����RTCʱ��, ����*/
void CMD_L_Handle(char *StrL,unsigned char ReplyL)
{
	bool Err=true;
	unsigned char i=0;
	
	if(((StrL[0] == '+') || (StrL[0] == '-')) && isdigit(StrL[1]) && isdigit(StrL[2])) 	// ��ʽΪ +08	��	-08								
	{
		i = (StrL[1] & 0x0F) * 10 + (StrL[2] & 0x0F);
		
		if(i < 24)										// ʱ��δ����24Сʱ
		{
			if(StrL[3]==':')							// ����
			{				
				if(isdigit(StrL[4]) && isdigit(StrL[5]))// ��ʽ������
				{
					unsigned char Min=0;
					
					Min = (StrL[4] & 0x0F) * 10 + (StrL[5] & 0x0F);
					
					switch(Min)							// ����ʱ����
					{
						case 0: GSM_P.TimeZone = 0x00;Err=false;break;
						case 15:GSM_P.TimeZone = 0x20;Err=false;break;
						case 30:GSM_P.TimeZone = 0x40;Err=false;break;
						case 45:GSM_P.TimeZone = 0x60;Err=false;break;
					}
				}
			}
			else
			{
				Err = false;
				GSM_P.TimeZone  = 0;
			}
			
			if(Err == false)
			{
				GSM_P.TimeZone |= i;				
				if(StrL[0] == '+') 	{GSM_P.TimeZone |= 0x80;}
				else				{GSM_P.TimeZone &= 0x7F;}
				GSMB.b.RTC_Update_Flag=false;			
				Task_SPI_Flash(Flash_Write,0,(u8*)&GSMA.w,1);
				Task_SPI_Flash(Flash_Write,1,(u8*)&GSMB.w,1);
				Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);
				send_info(" OK!","SET TIME ZONE OK!",CMD_L,ReplyL|OLDGPS,MSG_NO,Do_Save);
			}
		}	
	}
	else if((StrL[0] == '1')&&(StrL[1] == ',')&&(StrL[8] == ','))					// ��ʽΪ: 1,170101,120101		��������ʱ��		
	{		
		if((Isdigit(StrL+2,6) == true) && (Isdigit(StrL+9,6) == true))
		{
			unsigned short int year_tmp;
			unsigned char month_tmp,day_tmp,hour_tmp,min_tmp,sec_tmp;
			
			year_tmp  = (StrL[2]  & 0x0F)*10 + (StrL[3]  & 0x0F);
			month_tmp = (StrL[4]  & 0x0F)*10 + (StrL[5]  & 0x0F);
			day_tmp   = (StrL[6]  & 0x0F)*10 + (StrL[7]  & 0x0F);
			hour_tmp  = (StrL[9]  & 0x0F)*10 + (StrL[10]  & 0x0F);
			min_tmp   = (StrL[11] & 0x0F)*10 + (StrL[12] & 0x0F);
			sec_tmp   = (StrL[13] & 0x0F)*10 + (StrL[14] & 0x0F);
			
			if((year_tmp >=18) && (year_tmp <= 38) && (month_tmp <= 12) && (day_tmp <= 31) && (hour_tmp <= 23) && (min_tmp <= 59) && (sec_tmp <= 59))
			{
				Err = false;
				
				if(day_tmp > 30)					// ��Ϊ31��
				{
					if((month_tmp == 2) || (month_tmp == 4) || (month_tmp == 6) || (month_tmp == 9) || (month_tmp == 11))
					{
						Err = true;
					}
				}
				else if(day_tmp > 29)			 	// ��Ϊ30��
				{
					if(month_tmp == 2) 					{Err = true;}
				}
				else if(day_tmp > 28)				// ��Ϊ29��,�����ƽ���򱨴�
				{
					if((month_tmp == 2)&&(year_tmp%4))  {Err = true;}
				}
				
				if(Err == false)
				{
					RTC_Write_Year_Time(year_tmp, month_tmp,day_tmp, hour_tmp, min_tmp,sec_tmp);	
					GSMB.b.RTC_Update_Flag=true;													
					Task_SPI_Flash(Flash_Write,1,(u8*)&GSMB.w,1);
					send_info(" OK!","SET CLOCK OK!",CMD_L1,ReplyL|OLDGPS,MSG_NO,Do_Save);
				}
			}				
		}
	}
	else if(StrL[0] == '4')																		// ��ʽΪ4,����RTC�Զ�����һ��
	{
		Err = false;
		GSMB.b.RTC_Update_Flag=false;
		send_info(" OK!","SET to Get GPS New Time OK!",CMD_L4,ReplyL|OLDGPS,MSG_NO,Do_Save);
	}
#if RTC_Alarm
	else if((StrL[0] == '5') && (StrL[1] == ','))												// ��ʽΪ5,xx:xx
	{
		if(StrL[2] == '\0')
		{
			Err = false;
			GSMC.b.RTC_Alarm_Flag = false;
			HAL_RTC_DeactivateAlarm(&hrtc,RTC_ALARM_A);
		}
		else if(StrL[4] == ':')
		{
			if((Isdigit(StrL+2,2) == true) && (Isdigit(StrL+5,2) == true))
			{
				unsigned char Hour,Min;
				
				Hour = Char_To_Dig(StrL+2,2);
				
				Min = Char_To_Dig(StrL+5,2);
				
				if((Hour < 24) && (Min < 60))
				{	
					Err = false;		
					
					GSM_P.RTC_Alarm_Hour = Hour;
					GSM_P.RTC_Alarm_Min  = Min;
					
					GSMC.b.RTC_Alarm_Flag = true;
					RTC_Write_Alarm(GSM_P.RTC_Alarm_Hour,GSM_P.RTC_Alarm_Min);
				}
			}
		}
		
		if(Err == false)
		{
			Task_SPI_Oled(Oled_Main,0,0,0,0);	
			Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);
			send_info(" OK!","SET Alarm OK!",CMD_L5,ReplyL|OLDGPS,MSG_NO,Do_Save);
		}
	}
#endif

	if(Err == true)
	{
		send_info(" ERROR!","ERROR!",CMD_ERR,ReplyL|OLDGPS,MSG_NO,Do_Save);
	}
}

/* ����������λ������ʱ��*/
void CMD_M_Handle(char *StrM,unsigned char ReplyM)
{
	bool Err = true;
	unsigned int i;
	
	if((StrM[0] == '4') && (StrM[1] == ','))					// ����GPS���ģʽ ��ʽM4,1 M4,0
	{
		if((StrM[2] == '0') || (StrM[2] == '1'))
		{
			Err = false;
			GSMC.b.GPS_Tran_Mode_Flag = StrM[2] & 0x01;
			GPS_Mode_Setting();	
			Task_SPI_Flash(Flash_Write,2,(u8*)&GSMC.w,1);		
			send_info(" OK!","SET GPS Mode OK!",CMD_M4,ReplyM|OLDGPS,MSG_NO,Do_Save);
		}
	}		
	else if((StrM[0] == '2') && (StrM[1] == ','))				// �����������ʱ�� ��ʽ: M2,120, ���÷�Χ30 - 600
	{			
		if(Isdigit(StrM+2,strlen(StrM+2))) 	
		{			
			i = Char_To_Dig(StrM+2,strlen(StrM+2));
			
			if((i >= 60)&&(i <= 600))  	
			{
				Err = false;
				GSM_P.heartbeatset = i;	
				Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);	
				Set_Track_Time();				
				send_info(" OK!","SET Heartbeat Time OK!",CMD_M2,ReplyM|OLDGPS,MSG_NO,Do_Save); 
			}
		}
	}
	else if((StrM[0] == '0')||(StrM[0] == '1'))
	{
		if((StrM[1] == ',') && ((StrM[5] =='S') || (StrM[5] =='M') || (StrM[5] =='H')))
		{
			if(Isdigit(StrM+2,3) == true)
			{				
				i = Char_To_Dig(StrM+2,3);
		
				if((i > 0) && (i < 255))
				{
					Err = false;
					GSM_P.Track_Set_Time = i;
					GSM_P.Track_Set_Unit = StrM[5];
					GSMA.b.Track_Flag    = StrM[0] & 0x01;			
					Set_Track_Time();	
					Task_SPI_Flash(Flash_Write,0,(u8*)&GSMA.w,1);					
					Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);
					
					if(StrM[0] == '1')
					{
						send_info(" OK!","SET AUTO TRACK OK!",CMD_M,ReplyM|OLDGPS,MSG_NO,Do_Save);
					}
					else
					{
						send_info(" OK!","Cancel AUTO TRACK OK!",CMD_M,ReplyM|OLDGPS,MSG_NO,Do_Save);
					}
				}				
			}
		}
	}
                          											
	if(Err == true)
	{
		send_info(" ERROR!","ERROR!",CMD_ERR,ReplyM|OLDGPS,MSG_NO,Do_Save);	
	}		
}

/* ���õ͵籨�� */
void CMD_N_Handle(char *StrN,unsigned char ReplyN)		// ��ʽ N1,40		��	N0,40
{
	bool Err = true;

	if(((StrN[0] == '0') || (StrN[0] == '1'))&& (StrN[1] == ','))								// �͵籨������
	{
		if(Isdigit(StrN+2,2)) 
		{
			unsigned char i;
			
			i = Char_To_Dig(StrN+2,2);
			
			if(i <= 45)			
			{
				Err = false;
				GSM_P.set_soc=i;
				
				if(StrN[0] == '1')
				{													
					GSMA.b.ALM_LowBat_Flag=1;														 
					send_info(" OK!","OPEN LOW BAT OK!",CMD_N,ReplyN|OLDGPS,MSG_NO,Do_Save);																					
				}
				else 
				{
					GSMA.b.ALM_LowBat_Flag=0;													 
					send_info(" OK!","CLOSE LOW BAT OK!",CMD_N,ReplyN|OLDGPS,MSG_NO,Do_Save);									
				}
				Task_SPI_Flash(Flash_Write,0,(u8*)&GSMA.w,1);
				Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);
			}
		}
	}
	else if((StrN[0] == '2') && (StrN[1] == ',') && ((StrN[2] == '1') || (StrN[2] == '0')))		// �����籨������
	{
		Err = false;
		GSMB.b.ALM_Bat_Full_On_Flag = StrN[2] & 0x01;
		Task_SPI_Flash(Flash_Write,1,(u8*)&GSMB.w,1);
		
		if(StrN[2] == '1')
		{
			send_info(" OK!","OPEN FULL BAT OK!",CMD_N,ReplyN|OLDGPS,MSG_NO,Do_Save);	
		}
		else if(StrN[2] == '0')
		{
			send_info(" OK!","CLOSE FULL BAT OK!",CMD_N,ReplyN|OLDGPS,MSG_NO,Do_Save);	
		}
	}
		
	if(Err == true)
	{
		send_info(" ERROR!","ERROR!",CMD_ERR,ReplyN|OLDGPS,MSG_NO,Do_Save);										
	}
}

/* ����APN */
void CMD_O_Handle(char *StrO,unsigned char ReplyO)		// SETO, ��  SETOuser,password
{
	char *p1;

	p1 = strchr(StrO,',');		// ����','֮����ַ�
		
	if(p1 != NULL) 
	{
		*p1 = '\0';p1++;
		snprintf(GSM_P.APN_username,23,"%s",StrO);
		snprintf(GSM_P.APN_password,12,"%s",p1);
		Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);
		send_info(" OK!","SET APN Name & Password Ok!",CMD_O,ReplyO|OLDGPS,MSG_NO,Do_Save);
	}
	else
	{
		send_info(" ERROR!","ERROR!",CMD_ERR,ReplyO|OLDGPS,MSG_NO,Do_Save);
	}			
}

/* ����Բ��GEO */
void CMD_P_Handle(char *StrP,unsigned char ReplyP)		// P1,xxx
{
	bool Err = true;
	
	if(StrP[0] == '0') 
	{
		GSMA.b.ALM_Circle_GEO_Flag=0;	
		Task_SPI_Flash(Flash_Write,0,(u8*)&GSMA.w,1);
		send_info(" OK!","Cancel GEO Alarm ok!",CMD_P,ReplyP|OLDGPS,MSG_NO,Do_Save);
	}
	else if((StrP[0] == '1') && (StrP[1] == ','))
	{				
		if(Isdigit(StrP+2,strlen(StrP+2)) == true)
		{
			unsigned int GEO_tmp = 0;
			
			GEO_tmp = Char_To_Dig(StrP+2,strlen(StrP+2));
			
			if((GEO_tmp > 0) && (GEO_tmp <= 250))
			{	
				Err = false;
				
				GSMA.b.ALM_Circle_GEO_Flag=1;
				cnt_1s.Chk_GEO6_Cnt = 0;
				GPS_F.b.Send_Geo6_ALM_flag = false;		
				GSM_P.geo_radius = GEO_tmp;				
						
				GSM_P.center_lat=(float)(gpsx.latitude)/10000000;
				if(gpsx.nshemi=='S') 
				GSM_P.center_lat=-GSM_P.center_lat;		

				GSM_P.center_lot=(float)(gpsx.longitude)/10000000;
				if(gpsx.ewhemi=='W') 
				GSM_P.center_lot=-GSM_P.center_lot;	
							
				Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);
				Task_SPI_Flash(Flash_Write,0,(u8*)&GSMA.w,1);
				
				send_info(" OK!","Set GEO Alarm ok!",CMD_P,ReplyP|NEWGPS,MSG_NO,Do_Save);
			}
		}	
	}
		
	if(Err == true)
	{
		send_info(" ERROR!","ERROR!",CMD_ERR,ReplyP|OLDGPS,MSG_NO,Do_Save);		
	}
}

/* ����SOS�Ƿ��绰 */
void CMD_Q_Handle(char *StrQ,unsigned char ReplyQ)
{	
	if(((StrQ[0] >= '1') && (StrQ[0] <= '5')) && (StrQ[1] == ',') && ((StrQ[2] == '0') || (StrQ[2] == '1')))
	{
		switch(StrQ[0])
		{
			case '1':						// 1,1		��1,0		����SOS�Ƿ����绰
				if(StrQ[2] == '1')	{GSMA.b.SOS_CALL_ENABLE_Flag = 1;send_info(" OK!","Open SOS Call Out OK!",CMD_Q1,ReplyQ|OLDGPS,MSG_NO,Do_Save);		}
				else				{GSMA.b.SOS_CALL_ENABLE_Flag = 0;send_info(" OK!","Close SOS Call Out OK!",CMD_Q1,ReplyQ|OLDGPS,MSG_NO,Do_Save);	}
				break;
				
			case '2':						// 2,1		��2,0		�����Ƿ���Ҫ��֤��Ȩ����
				if(StrQ[2] == '1')	{GSMA.b.Any_CallIn_Flag = 1;send_info(" OK!","Set answer any number!",CMD_Q2,ReplyQ|OLDGPS,MSG_NO,Do_Save);			}
				else				{GSMA.b.Any_CallIn_Flag = 0;send_info(" OK!","Set answer authorized number!",CMD_Q2,ReplyQ|OLDGPS,MSG_NO,Do_Save);	}
				break;
				
			case '3':						// 3,1		��3,0		�����Ƿ��������������Ҷ�����
				if(StrQ[2] == '1')	{GSMA.b.CallIn_ByMe_Flag = 1;send_info(" OK!","Set answer by user OK!",CMD_Q3,ReplyQ|OLDGPS,MSG_NO,Do_Save);		}
				else 				{GSMA.b.CallIn_ByMe_Flag = 0;send_info(" OK!","Set answer automatically!",CMD_Q3,ReplyQ|OLDGPS,MSG_NO,Do_Save);		}
				break;
#if MODULE_SIM7500
			case '4':
				if(StrQ[2] == '1')	{GSMC.b.DTMF_Det_Flag = 1;send_info(" OK!","Set voice mail box det OK!",CMD_Q4,ReplyQ|OLDGPS,MSG_NO,Do_Save);		}
				else 				{GSMC.b.DTMF_Det_Flag = 0;send_info(" OK!","Set Voice mail box no det!",CMD_Q4,ReplyQ|OLDGPS,MSG_NO,Do_Save);		}		
				break;
#endif	
			case '5':
				if(StrQ[2] == '1')	{GSMC.b.SMS_Check_psw_Flag = 1;send_info(" OK!","Set Check psw on OK!",CMD_Q5,ReplyQ|OLDGPS,MSG_NO,Do_Save);		}
				else 				{GSMC.b.SMS_Check_psw_Flag = 0;send_info(" OK!","Set Check psw off OK!",CMD_Q5,ReplyQ|OLDGPS,MSG_NO,Do_Save);		}				
				break;	
			default :break;
		}
			
		if(StrQ[0] < '4')
		{
			Task_SPI_Flash(Flash_Write,0,(u8*)&GSMA.w,1);	
		}
		else
		{
			Task_SPI_Flash(Flash_Write,2,(u8*)&GSMC.w,1);	
		}
	}
	else
	{
		send_info(" ERROR!","ERROR!",CMD_ERR,ReplyQ|OLDGPS,MSG_NO,Do_Save); 
	}
}

/*��������*/
void CMD_R_Handle(char *StrR,unsigned char ReplyR)
{
	bool Err = true;
		
	if(StrR[0]==';')											// ɾ������ָ��
	{
		Err = false;
		GSM_P.domain_name[0]=0;
		GSM_P.domain_port[0]=0;									
	}
	else														// ��������ָ��
	{
		char *p1,*p2;	
		
		p1=StrR;
		p2=strchr(StrR,';');									// ;
	
		if(p2 != NULL)											// ��;����
		{
			*p2 = 0; p2++;										// ָ��˿ں�
			
			if((memcmp(p1,"http://",7)==0)&&(strlen(p1)>7))
			{
				snprintf(GSM_P.domain_name,24,"%s",p1+7);		// ��ౣ��24���ַ�,������http://
				
				if(p2[0] == '\0')								// �޶˿ں�
				{
					Err = false;
					GSM_P.domain_port[0]=0;	
				}
				else
				{
					if((strlen(p2) < 6) && Isdigit(p2,strlen(p2)))				// �˿ں��ַ�����С��6
					{						
						if(atoi(p2) < 65535)
						{
							Err = false;
							snprintf(GSM_P.domain_port,6,"%s",p2);	// ����˿ں�
						}
					}												
				}
			}			
		}
	}
		
	if(Err==false)
	{
		Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);
		send_info(" OK!","SET HTTP Ok!",CMD_R,ReplyR|OLDGPS,MSG_NO,Do_Save);	
	}
	else									
	{
		send_info(" ERROR!","ERROR!",CMD_ERR,ReplyR|OLDGPS,MSG_NO,Do_Save);	
	}
}

/*	���ÿ�����رմ��ڵ���	*/
void CMD_S_Handle(char *StrS,unsigned char ReplyS)
{
	;
}

/*����ģʽ*/
void CMD_U_Handle(char *StrU,unsigned char ReplyU)
{
	bool Err = true;
	
	#if (OTA_F==1)													// OTA���� ʾ����123456UV���ް汾�ţ�\123456UV01�V01Ϊ�汾��	 
	if(StrU[0] == 'V')
	{																 
		if(StrU[1] == '\0')		
		{
			Err = false;
		}
		else									
		{			
			if((strlen(StrU) <= 5) && (Isdigit(StrU+1,strlen(StrU)-1)))
			{
				Err = false;
			}		
		}
		
		if(Err == false)
		{
			if(ReplyU == RPLY_TO_PC)								// ����������������
			{
				GSM_OTA.OTA_state=0x02;								// ׼����������
			}
			else if(ReplyU == RPLY_TO_SMS)							// �����ɹ���ظ�����	
			{
				GSM_OTA.OTA_state=0x04;
				
				GSM_OTA.OTA_ph = Compare_phone_number(SMS_Phone_Number) - 1;
			}
			else													// �����ɹ���ظ�GPRS
			{
				GSM_OTA.OTA_state=0x08;								// ׼��OTA����
			}
			
			GSM_OTA.OTA_errcount=0;									// ����ʧ�ܴ���							
			GSM_OTA.mcu_to_spi_bk=0;								// ��Ҫ���±���
			snprintf(GSM_OTA.program_ver,5,"%s",StrU+1);
			Task_SPI_Flash(Flash_Write,otaaddr,(u8*)&GSM_OTA,GSM_OTA_size);
			send_info(" OK!","OTA Start!",CMD_U,ReplyU|OLDGPS,MSG_NO,Do_Save);	
			xEventGroupSetBits(EventGroupHandler_MCU,MCU_OTA_Flag);
		}
	}
	#endif
	
	if((StrU[0] >= '0') && (StrU[0] <= '9'))
	{
		Err = false;
		
		switch(StrU[0])
		{
			#if CALLIN_ENABLE
			case '0'://����绰��ͨ��
				GSMB.b.Callin_SpkMode_Flag=1;
				send_info(" OK!","Talking ok!",CMD_U0,ReplyU|OLDGPS,MSG_NO,Do_Save);
				break;

			case '1'://����绰�Ǽ���
				GSMB.b.Callin_SpkMode_Flag=0;
				send_info(" OK!","Listen in OK!",CMD_U1,ReplyU|OLDGPS,MSG_NO,Do_Save);
				break;
			#endif
			
			#if CALLOUT_ENABLE
			case '2'://����绰�Ǿ���
				GSMB.b.Callout_LisMode_Flag=1;
				send_info(" OK!","Calling in silence from SOS OK!",CMD_U2,ReplyU|OLDGPS,MSG_NO,Do_Save);
				break;

			case '3'://����绰��ͨ��
				GSMB.b.Callout_LisMode_Flag=0;
				send_info(" OK!","Calling with voice from SOS OK!",CMD_U3,ReplyU|OLDGPS,MSG_NO,Do_Save);
				break;
			#endif

			case '4'://��GPS
				GSMB.b.GPS_PowerOffMode_Flag=0;
				send_info(" OK!","Turn on GPS OK!",CMD_U4,ReplyU|OLDGPS,MSG_NO,Do_Save);
				break;
									
			case '5'://�ر�GPS
				GSMB.b.GPS_PowerOffMode_Flag=1;
				GPS_Force_Off();
				send_info(" OK!","Turn off GPS OK!",CMD_U5,ReplyU|OLDGPS,MSG_NO,Do_Save);
				break;

			case '6'://GPRS��ʡ����ģʽ
				GSMB.b.GPRS_SaveMode_Flag=1;
				Set_Track_Time();		// ����������λ
				send_info(" OK!","GPRS SLOW OK!",CMD_U6,ReplyU|OLDGPS,MSG_NO,Do_Save);
				break;

			case '7'://���趨ʱ�����ϴ�
				GSMB.b.GPRS_SaveMode_Flag=0;
				Set_Track_Time();		// ����������λ
				send_info(" OK!","GPRS NORMAL OK!",CMD_U7,ReplyU|OLDGPS,MSG_NO,Do_Save);
				break;

			case '8'://GPS�����ڳ�ǿģʽ
				GSMB.b.GPS_PowerOffMode_Flag=0;
				GSMB.b.GPS_StrongMode_Flag=1;
				GPS_Power_On();													// ����������Դ
				if(ECHO_F.b.GPS_MCU_Flag){NVIC_EnableIRQ(USART2_IRQn);	}
				send_info(" OK!","GPS STRONG OK!",CMD_U8,ReplyU|OLDGPS,MSG_NO,Do_Save);
				break;

			case '9'://GPS������ʡ��ģʽ
				GSMB.b.GPS_StrongMode_Flag=0;
				GPS_Power_Off();	
				send_info(" OK!","GPS SAVING OK!",CMD_U9,ReplyU|OLDGPS,MSG_NO,Do_Save);
				break;	 

			default :break;

		}
		
		Task_SPI_Flash(Flash_Write,1,(u8*)&GSMB,1);
	}
	
	if(Err == true)	
	{
		send_info(" ERROR!","SET ERROR!",CMD_ERR,ReplyU|OLDGPS,MSG_NO,Do_Save);
	}				
}

/* ָ�����绰 */
void CMD_V_Handle(char *StrV,unsigned char ReplyV)
{
	bool Err = true;
	
	if((StrV[0]=='3')&&((StrV[1]=='0')||(StrV[1]=='1'))&&(StrV[2]==','))	// V30,+xx
	{	
		if(isdigit(StrV[3]) || (StrV[3] == '+'))
		{
			if(Isdigit(StrV+4,strlen(StrV+4)))
			{
				char Buf[50]={0};
				
				if(StrV[1] == '0')	
				{
					Calling_Set_Mode(1,1,0);
					snprintf(Buf,50,"Calling %s with voice OK!",GSM_ph.temp_ph8);		
				}
				else				
				{
					snprintf(Buf,50,"Calling %s in mute OK!",GSM_ph.temp_ph8);			
				}
				
				Err = false;
				snprintf(Ring_Phone_Number,20,"%s",StrV+3);
				send_info(" OK!",Buf,CMD_V3,ReplyV|OLDGPS,MSG_NO,No_Save);// ע��ȽϷ����ַ�.
				Task_AT_GSM(Gsm_Call_Out,Ring_Phone_Number,0,0,0,0,0);
			}
		}
	}								   
	else if(Isdigit(StrV,strlen(StrV)) && (strlen(StrV) <= 3))
	{
		char Vol[13];
		
		Err = false;
		GSM_P.spk_volume = Char_To_Dig(StrV,strlen(StrV));
		if(GSM_P.spk_volume > 100) {GSM_P.spk_volume = 100;}
		GSM_P.spk_volume = GSM_P.spk_volume / 10;
		Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);
		snprintf(Vol,13,"Volume:%d%%!",GSM_P.spk_volume*10);
		send_info(" OK!",Vol,CMD_V,ReplyV|OLDGPS,MSG_NO,No_Save);// ע��ȽϷ����ַ�.
	}
	
	if(Err == true)
	{
		send_info(" ERROR!","ERROR!",CMD_ERR,ReplyV|OLDGPS,MSG_NO,No_Save);
	}			
}




/* 	TIM-����WIFI:ssid,key */
void CMD_W_Handle(char *StrW,unsigned char ReplyW)
{
	char * str_wifi;
	//ָ���ʽ��SETW,ssid,key
	if(StrW[0] == 'W')
	{
		str_wifi = StrW+2;
		strcpy(WIFI.wifi_ssid,strtok(str_wifi, ","));
		strcpy(WIFI.wifi_key,strtok(NULL, ","));
	}
	
}

/* ����LOG */
void CMD_Z_Handle(char *StrZ,unsigned char ReplyZ)
{
	if((StrZ[0] <= '2') && (StrZ[1] == ','))
	{
		snprintf(GSM_OTA.Device,15,"%s",StrZ+2);send_info(" OK!","Device name OK!",CMD_Z2,ReplyZ|OLDGPS,MSG_NO,Do_Save);	// 2,xxxx,�������25���ַ�
				
		Task_SPI_Flash(Flash_Write,otaaddr,(u8*)&GSM_OTA,GSM_OTA_size);
	}
	else
	{
		send_info(" ERROR!","ERROR!",CMD_ERR,ReplyZ|OLDGPS,MSG_NO,No_Save);
	}	
}

