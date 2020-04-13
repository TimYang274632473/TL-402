#include <includes.h>

//需保存到SPIFLASH的参数
GSM_A_def GSMA;
GSM_B_def GSMB;
GSM_C_def GSMC;
GSM_D_def GSMD;

GSM_para 		GSM_P;
GSM_phone 		GSM_ph;
GSM_phonename	GSM_phname;

//其他临时标志 
GSM_sms_def  	SMS;
GSM_call_def	CALL;
GSM_gprs_def 	GPRS;

char *_ptcmdx;
char socalm[12];
char gsm_rx_buf[gsm_rx_len];  

#if MODULE_SIM7500
unsigned short int gsm_rx_sta=0;						// USART1接收数据存入数组的位置
#else
unsigned char gsm_rx_sta=0;								// USART1接收数据存入数组的位置
#endif
unsigned char gsm_rx_req=0;								// USART1要求接收到字符个数

#define company1 "TOPLOVO INDUSTRIAL CO., LTD\nCHINA SHENZHEN HENGYUETONG ELECTRONICS CO., LTD\nTL403,Copyright !"
		
#if CALLIN_ENABLE
unsigned char Ring_Clip_Count=0;						// 来电响铃次数
#endif
	
u8  gsmerr=0;

unsigned char network=0;								// 2G网络, 3G网络, 4G网络

signed char temperature=0;

unsigned char SMS_NUM=0;								// 用于保存接收到的短信存储位置

char SMS_Phone_Number[24];
char SMS_Phone_Number_Backup[24],Ring_Phone_Number[20];	// 存储来电号码

	
/* 判断字符是否为数字		*/
bool Isdigit(char *Str,unsigned char Num)
{
	unsigned char i;
	
	for(i=0;i<Num;i++)
	{
		if(!isdigit(Str[i])) return false;
	}
	
	return true;
}

/* 判断是否为十六进制字符	*/
bool Isxdigit(char *Str,unsigned char Num)
{
	unsigned char i;
	
	for(i=0;i<Num;i++)
	{
		if(!isxdigit(Str[i])) return false;
	}
	
	return true;
}

/* 将字符串按十进制转换		*/
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

/* 将字符串按十六进制转换		*/
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

/*清除gsm_rx_buf， gsm_rx_sta归零*/
void clr_gsm_buf(void)	
{	
	gsm_rx_sta = 0; 
	
	for(u8 i=0;i<gsm_rx_len;i++){gsm_rx_buf[i]=0;}

	xEventGroupClearBits(EventGroupHandler_OTH,Event_Gsm_All);
}

/*在gsm_rx_buf中查找字符串,找到返回1，未找到返回0*/
u8 find_str(char *a)								 
{ 
	return strstr(gsm_rx_buf,a)==NULL? 0:1;
}

/*发送单个字节*/
void uartTxByte (UART_HandleTypeDef *huart,uint8_t ucData)
{
#if MODULE_SIM7500
	if(MCU_F.b.Uart1_to_Luart1) {return;}
#endif
	
	if(ECHO_F.b.MCU_GSM_Flag == ON)						// 如果需要将AT指令回显到PC端
	{
		HAL_UART_Transmit(&huart1,&ucData,1,1000);
	}
	
	HAL_UART_Transmit(huart,&ucData,1,1000);
}

/*发送字符串*/
void send_str_to_gsm (char *pStr)
{
#if MODULE_SIM7500
	if(MCU_F.b.Uart1_to_Luart1) {return;}
#endif	
	
	if(ECHO_F.b.MCU_GSM_Flag == ON)						// 如果需要将AT指令回显到PC端
	{
		HAL_UART_Transmit(&huart1,(uint8_t*)pStr,strlen((char*)pStr),1000);
	}
	
	HAL_UART_Transmit(&hlpuart1,(uint8_t*)pStr,strlen((char*)pStr),1000);
}

/*比较来电号码，一致返回1，否则返回0*/
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

/*sstr:发送字符串 rcount:接收到字符个数 rtime:接收超时时间 atcount:执行次数 rstrtype:AT指令编号 return:true成功,false失败*/
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
			case 0x0002: 	// 发送短信															
				p1=strstr(gsm_rx_buf,">");         					 
				break;
			
			case 0x0003:	// 发送GPRS			
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
						
			case 0x0004:	// 发送短信															
				p1=strstr(gsm_rx_buf,"+CMGS"); 
				break;	
#if MODULE_SIM7500			
			case 0x0005:	// 4G网络
				p1=strstr(gsm_rx_buf,"+CNSMOD");
				if(p1 != NULL)
				{
					if(!strstr(gsm_rx_buf,"OK"))
					{
						vTaskDelay(2);
					}	
				}
				break;
		
			case 0x0006:	// 播放音频
				if(strstr(gsm_rx_buf,"ERROR") != NULL)
				{	
					return false;	
				}
				
				vTaskDelay(2);
				
				p1 = strstr(gsm_rx_buf,"wav play");	
				break;
#if GSM_Positon				
			case 0x0007:	// 基站定位
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
			case 0x0008:	// 查询联网状态
				p1=strstr(gsm_rx_buf,"+CIPOPEN: 0,0"); 															
				break;
#endif
			
			case 0x0010:	// 查询发送结果															
#if MODULE_UG96
				p1=strstr(gsm_rx_buf,"+QHTTPGET:"); 
#elif MODULE_SIM7500
				p1=strstr(gsm_rx_buf,"+HTTPACTION");
#endif
				break;

			case 0x0011:	// 查询HTTPHEAD
				p1=strstr(gsm_rx_buf,"+HTTPHEAD");
				break;
			
			case 0x0012:	// 查询HTTPREADFILE
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
			
			case 0x0040:	// 查询AP配置结果
#if MODULE_SIM7500
				p1=strstr(gsm_rx_buf+17,"MDM"); 
#endif	
				break;

			case 0x0080:	// 查询短信
				p1=strstr(gsm_rx_buf,"OK");									// 先找是否OK,再判断是否为READ
			
				if((p1 != NULL)&&(gsm_rx_sta<=6))							// 只找到OK
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

			case 0x0200:	// 获取COPS
				p1=strstr(gsm_rx_buf,"+COPS:");
				if(p1 != NULL)
				{
					if(strstr(gsm_rx_buf,"OK") == NULL)
					{
						vTaskDelay(10);
					}
				}		
				break;
				
			case 0x0400:   // HTTP连网
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
			
			case 0x4000:	// 查询连网状态														
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
				
			case 0x8000:	// 获取CSQ
				p1=strstr(gsm_rx_buf,"+CSQ:");
				if(p1 != NULL)
				{
					if(strstr(gsm_rx_buf,"OK") == NULL)
					{
						vTaskDelay(10);
					}
				}		
				break;
				
			case 0x8001:	// 温度
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
			
			case 0x8002:	// 电量
				p1=strstr(gsm_rx_buf,"+CBC:");
			
				if(p1 != NULL)
				{
					if(strstr(gsm_rx_buf,"OK") == NULL)
					{
						vTaskDelay(10);
					}
				}	
				break;
				
			default:																// 查询结果是否OK
				p1=strstr(gsm_rx_buf,"OK");	       
				break;
		}

		if(p1!=NULL)																// 获取OK 
		{						
			gsmerr=0;return true;
		}
		else																		// 获取NG
		{
			if(gsm_rx_sta==0)														// 检测模块是否正常工作
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

/* 配置波特率 */ 
void ATE0_Test(void)
{	
	if(!__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))								// 如果是冷启动
	{
		vTaskDelay(200);GSMON_H();vTaskDelay(500);GSMON_L();vTaskDelay(2000);// 重启MCU_F
	}

#if MODULE_SIM7500
	if(GSM_AT_Send("E0",6,5,5,0x0001) == false)								// 按9600波特率,如果测试失败
#else	
	if(GSM_AT_Send("E0",6,2,5,0x0001) == false)								// 按9600波特率,如果测试失败
#endif
	{
		bool reset = false;
		
		vTaskDelay(200);GSMON_H();vTaskDelay(500);GSMON_L();vTaskDelay(2000);// 重启MCU_F
									
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
			GSM_AT_Send("E0",6,5,2,0x0001);						// 按9600波特率,如果测试失败
#else	
			GSM_AT_Send("E0",6,2,2,0x0001);						// 按9600波特率,如果测试失败
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

/* 查询SIM状态	*/
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

/* 模块初始化配置*/
void GSM_init(void)
{
	char*p1;
	u8 i;	
	
	GSM_AT_Send("+CMEE=0",6,2,10,0x0001);
	
	if(MCU_F.b.Imei_Err_Flag)
	{
		GSM_AT_Send("+CGSN",25,2,2,0x0001);									// 获取IMEI号					 	
	
		for(i=0;i<10;i++) 
		{
			if(isdigit(gsm_rx_buf[i]))
			{
				gsm_rx_buf[i+15]=0;break;
			}
		}

		snprintf(GSM_P.IMEI,16,"%s",gsm_rx_buf+i);							// 把IMEI号存入IMEI数组中

		if(Isdigit(GSM_P.IMEI,15) == false)									// 检查IMEI号是否为数字，如不是重新读取
		{
			Task_SPI_Flash(Flash_MCU_Reset,0,0,0);;	
		}
		else
		{
			Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);		
		}			
	}
				
	if(CPIN_P() == false)													// 检查SIM卡安装是否OK,无卡直接进入系统
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
		
	GSM_AT_Send("+CREG=2",6,2,10,0x0001);									// 关闭SIM上报信息	
	
	for(i=0;i<100;i++)														// 查询注册结果
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
			GSM_AT_Send("+CFUN=1,1",6,2,10,0x0001);							// 重启MCU_F模块
			Task_SPI_Flash(Flash_MCU_Reset,0,0,0);							// 注册30次未成功,复位
		}
	}
	
	Task_SPI_Oled(Oled_Preogress,"GSM Init Now","",20,0);						// LED显示进度
	
	GSM_AT_Send("+CREG=0",6,3,5,0x0001);									// 关闭上报
	
	GSM_AT_Send("+QSCLK=1",6,3,2,0x0001);									// 启用慢时钟
	
	GSM_AT_Send("+CFUN=1",6,3,2,0x0001);									// 

	GSM_AT_Send("+QDAC=4",6,3,5,0x0001);									// 设置PCM
	
	GSM_AT_Send("+QCFG=\"nwscanmode\",0,1",6,4,2,0x0001);					// 网络自动搜索模式
			
	vTaskDelay(1000);
	
	GSM_AT_Send("+CLIP=1",6,2,2,0x0001);
	
	GSM_AT_Send("+CMGF=1",6,2,2,0x0001);									// 选择短信格式--文本模式
	
	if(GSM_AT_Send("+CSDH=0",6,2,2,0x0001) == false){Task_SPI_Flash(Flash_MCU_Reset,0,0,0);}		// 文本模式不显示附加参数
	
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
	
	GSM_AT_Send("+CMEE=0",6,2,10,0x0001);									// 关闭上报ERR号
	
	GSM_AT_Send("+IFC=0,0",6,2,2,0x0001);
	
	GSM_AT_Send("+CRSL=90",6,2,2,0x0001);									// 来电响铃

#if !DEBUG
	GSM_AT_Send("+QSTK=1",6,2,2,0x0001);									// 用于虚拟运营商
	
	GSM_AT_Send("+QSTKRSP=33,0",6,2,2,0x0800);								// 关闭虚拟运营商上报信息
		
	if(GSM_AT_Send("+COPS=0,2",6,150,1,0x0001) == false)					// 如果注册失败,延时3S
	{
		vTaskDelay(30000);
	}
	
	if(GSM_AT_Send("+CGREG?",21,150,1,0x0001) == false)						// 如果注册失败,延时3S
	{
		vTaskDelay(30000);
	}
#endif
	
#elif MODULE_SIM7500
	
	GSM_AT_Send("+CSCLK=1",6,2,2,0x0001);									// 可以通过DTR引脚控制MCU_F睡眠及唤醒
			
	GSM_AT_Send("+CREG=1",6,2,10,0x0001);									// 关闭SIM上报信息	
	
	for(i=0;i<200;i++)														// 查询注册结果
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
	
	if(GSM_AT_Send("+CSDH=0",6,2,2,0x0001) == false){Task_SPI_Flash(Flash_MCU_Reset,0,0,0);}	// 文本模式不显示附加参数
	
	GSM_AT_Send("+CLIP=1",6,2,2,0x0001);
	
	GSM_AT_Send("+CMGF=1",6,2,2,0x0001);									// 选择短信格式--文本模式
	
	GSM_AT_Send("+CNMI=2,1,0,0,0",6,2,2,0x0001);
	
	GSM_AT_Send("+CSMP=17,255,0,0",6,5,5,0x0001);

	GSM_AT_Send("+CSCS=\"IRA\"",6,2,2,0x0001);
	
	GSM_AT_Send("+CGCLASS=?",25,2,2,0x0001);
	
	GSM_AT_Send("+CSUB",58,2,2,0x0040);
	
	CSDVC_P();
			
	GSM_AT_Send("+CLVL=5",6,2,2,0x0001); 									// 通话音量

	GSM_AT_Send("+COUTGAIN=8",6,10,2,0x0001);

	GSM_AT_Send("+CIPRXGET=1",6,2,2,0x0001);

#if GSM_Positon
	GSM_AT_Send("+CLBSCFG=1,3,\"lbs-simcom.com:3002\"",6,2,2,0x0001);
#endif

#if !DEBUG
	if(GSM_AT_Send("+COPS=3,2",6,150,1,0x0001) == false)					// 如果注册失败,延时3S
	{
		vTaskDelay(30000);
	}
#endif
	
#endif
}	


/*获取电量、信号、基站、运营商信息,flag1==1只读取信号, flag1==1读取COPS，flag1==2读取全部,最高位为1必须执行*/
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
			
			if(MCU_F.b.First_Read_CBC_Flag == false)						// 如果是开机第一次获取电量,或电量为0
			{
				MCU_F.b.First_Read_CBC_Flag = true;
				
				Bat_Val[0] = Bat_Val[1] = Bat_Val[2] =  gpsx.soc = j;
			}
			else
			{
				Bat_Val[Bat_Cnt]=j;											// 记录电量
				
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
				
				if(++Bat_Cnt >= 3)											// 读取到三次电量后算平均值
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
										
			if(GSMA.b.ALM_LowBat_Flag)														// 如果已开启低电监测功能
			{	
				if(gpsx.soc<(GSM_P.set_soc&0x7F))											// 未发送过低电报警
				{
					if(MCU_F.b.Charging_Flag == false){SOUND_BI(BIBI);}
					
					if(!(GSM_P.set_soc&0x80))												// 低7位作为设置值,已达低电报警阀值
					{							
						GSM_P.set_soc|=0x80;												
						Task_SPI_Flash(Flash_Write,gpsadd,(u8*)&gpsx,gpsx_All_Size);		
						Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);	
						snprintf(socalm,22,"LOW BAT!%d%%",gpsx.soc);	
						Task_AT_GSM(Gprs_Send_Msg,0,0,ALM_D,OLDGPS,MSG_GPRS,1);	
						Task_AT_GSM(Gsm_Send_SMS,GSM_ph.temp_ph1,socalm,0,NOGPS,MSG_NO,0);						
					}
				}
				else																		// 如果已发出低电报警
				{
					if(gpsx.soc>((GSM_P.set_soc&0x7F)+20))									
					{
						GSM_P.set_soc&=0x7f;												
						Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);	
					}
				}				
			}
			
#if Bat_Full_ON	
			
			if((GSMB.b.ALM_Bat_Full_On_Flag)&&(GSMB.b.ALM_Bat_Full_Send_Flag))				// 如果已开启充满电报警及已发送报警信息
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
	
	for(i=0;i<3;i++)														// 读取CSQ
	{
		if(GSM_AT_Send("+CSQ",20,2,1,0x8000) == true)
		{
			p1 = strstr(gsm_rx_buf,"+CSQ:");strtok(p1,",");
			
			j = (unsigned char)(atoi(p1+6));
									
			if(j<32){gpsx.sig_strength = j;break; }	
		}	
	}

#if MODULE_UG96
	
	for(i=0;i<3;i++)														// 读取TEMP
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
	
	for(i=0;i<3;i++)														// 读取TEMP
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
					else if((MCU_F.b.No_Simcard_Flag == false) && (CPIN_P() == false))	// 掉卡,立即重启
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
			else if((MCU_F.b.No_Simcard_Flag == false) && (CPIN_P() == false))	// 掉卡,立即重启
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

/* 通话结束,清除所有事件	*/
void Gsm_Calling_Clr_Flag(void)
{
	CALL.w = 0;	
	Ring_Clip_Count=0;															
	cnt_1s.Ring_Time=0;	
	xEventGroupClearBits(EventGroupHandler_MCU,Event_Key_All);	
}

/* 设置通话模式			*/
void Calling_Set_Mode(bool SOS_Hangup,bool Turn_Vol,bool Lis_To_Spk)
{
	CALL.b.SOS_HangUp_En_Flag = SOS_Hangup;
	CALL.b.Turn_Vol_En_Flag   = Turn_Vol;
	CALL.b.Lis_To_Spk_En_Flag = Lis_To_Spk;
}

#if MODULE_SIM7500
// 回音处理
void CECM_P(void)
{
	GSM_AT_Send("+CECM=1",6,2,2,0x0001);
}

void CSDVC_P(void)
{
	GSM_AT_Send("+CSDVC=3",6,15,3,0x0001);
}

#endif

/*挂电话*/
void ATH_P(void)
{
#if MODULE_UG96
	GSM_AT_Send("H",6,2,2,0x0800);
#elif MODULE_SIM7500
	GSM_AT_Send("+CHUP",6,2,2,0x0800);
#endif
}

/*查询电话活动状态*/
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

/* 查询电话状态		*/
char CLCC(void)
{
	char *p1;
#if MODULE_SIM7500
	if(GSM_AT_Send("+CLCC",6,5,1,0x0001) == true)
#else	
	if(GSM_AT_Send("+CLCC",6,1,1,0x0001) == true)									// +CLCC: 1,0,2,0,0,"+8618948252262",145,""	查询是否有回复CLCC,电话号码可以最多为20个字符	
#endif
	{	
		return 0;																	// 返回失败
	}
	else
	{
		vTaskDelay(10);
		p1 = strstr(gsm_rx_buf,"+CLCC:");					
		if(p1 != NULL)																// 可以查询对通话状态
		{
			return(*(p1+11));
		}
		else
		{
			return 1;
		}
	}
}

/* 通话中处理		*/
void GSM_Calling_Handle(void)
{
#if MODULE_SIM7500
	char CPAS_Status='0';
#else
	char CPAS_Status=4;															// 电话活动状态
#endif
	
	EventBits_t Event_Key;
	
	while(1)
	{
		Event_Key = xEventGroupWaitBits(EventGroupHandler_MCU,Event_Key_All, pdFALSE,pdFALSE,1000);
		
		xEventGroupClearBits(EventGroupHandler_MCU,Event_Key_All);
		
		if(Event_Key & (Key_Call1_S_Flag | Key_Call2_S_Flag))						// 打入/打出为通话模式及允许挂机的条件下,可以调大音量
		{				
			if(GSMB.b.Callout_LisMode_Flag)										
			{
				if(CALL.b.Lis_To_Spk_En_Flag)										// 如果允许从静音模式切换到通话模式
				{
					CALL.b.Lis_To_Spk_En_Flag = false;
					CALL.b.Turn_Vol_En_Flag = true;									// 可以调节音量
					Speak_Amp_On();															
					spk_mic(GSM_P.spk_volume,MIC_Spk);								// 打开喇叭声音,打开MIC声音
				}
			} 	
		}
		
		if(Event_Key & Key_Call1_S_Flag)											// 打入/打出为通话模式及允许挂机的条件下,可以调大音量
		{							
			if(CALL.b.Turn_Vol_En_Flag)												// 如果允许调节音量
			{
				Spk_Vol_Turn(1);
			}	 	
		}
		else if(Event_Key & Key_Call2_S_Flag)										// 打入/打出为通话模式及允许挂机的条件下,可以调小音量
		{				
			if(CALL.b.Turn_Vol_En_Flag)												// 如果允许调节音量
			{
				Spk_Vol_Turn(0);
			}	
		}
		
		if(Event_Key & Key_SOS_S_Flag)													// SOS按键挂断所有电话
		{	
			if(CALL.b.SOS_HangUp_En_Flag )												// 如果允许主动挂机
			{
				ATH_P();																// 挂断电话,退出通话处理
				CPAS_Status = 0;														// 强制立即退出
			}			
		}
		else																			// 没有按键操作查询通话状态
		{
#if MODULE_SIM7500
			CPAS_Status=CLCC();
#else
			CPAS_Status=CPAS_P();														// 查询通话状态
#endif
		}
				
		if(CPAS_Status==0)																// 已挂断电话
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

// Num 音频文件号
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

// 是否停止本地语音播放
void stop_audio(void)
{
	GSM_AT_Send("+CCMXSTOPWAV",20,1,1,0x0001);
	
	vTaskDelay(10);
}

#endif

/* 接听来电		*/
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
	
	if(GSMB.b.Callin_SpkMode_Flag)															// 进入通话模式														
	{
		Calling_Set_Mode(1,1,0);
	}
	else																					// 进入监听模式
	{
		Calling_Set_Mode(0,0,0);
	}	
}

/* 处理来电		*/
bool GSM_Callin_Handle(void)
{
	if(CALL.b.Calling_Flag) {return false;}													// 正在通话中 						

	CALL.b.Calling_Flag = true;					
		
	if((((GSMA.b.Any_CallIn_Flag))|| (Compare_phone_number(Ring_Phone_Number) != 0)) && (!GSMD.b.CMDF_SOS_Flag) && (!GSMD.b.CMDF_BMAL_Flag) && (!GSMD.b.CMDF_BMAH_Flag))			
	{
		EventBits_t GSM_CallIn;
				
		if(GSMB.b.Callin_SpkMode_Flag)															// 通话模式
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
			
			if(GSM_CallIn & Key_Power_S_Flag)													// 按CALL1,PWR任一键
			{								
				if(GSMA.b.CallIn_ByMe_Flag && GSMB.b.Callin_SpkMode_Flag)						// 如果设置了由自主接听,且为通话模式
				{	
					Gsm_Callin_ATA();return true;
				}
			}
			
			if((!GSMA.b.CallIn_ByMe_Flag) || (!GSMB.b.Callin_SpkMode_Flag))						// 如果不是自主接听或监听模式
			{
#if DEBUG 
				if(Ring_Clip_Count >= 1)				
#else				
				if(Ring_Clip_Count >= 2)														// 响铃超出三次,可以接听
#endif
				{
					Gsm_Callin_ATA();return true;
				}
			}	

			if((cnt_1s.Ring_Time > 6) || ((GSM_CallIn & Key_SOS_S_Flag) && GSMA.b.CallIn_ByMe_Flag && GSMB.b.Callin_SpkMode_Flag))// 如果主动挂机.或如果对方在响铃四次前挂机
			{
#if MODULE_SIM7500
				if(GSMB.b.Callin_SpkMode_Flag)
				{
					stop_audio();
				}
#endif				
				if(GSM_CallIn & Key_SOS_S_Flag)													// SOS按键主动挂机
				{	
					ATH_P();																	// 挂断电话,退出通话处理	
				}
				else																			// 对方挂机
				{
					if((Ring_Phone_Number[0]!=0) && (Ring_Clip_Count < 4))								
					{
						strcpy(SMS_Phone_Number_Backup,Ring_Phone_Number);		
						SOS_BMA_F_GPS_Start(1);													// 开启获取GPS数据,并回复到手机短信	
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

/* SOS,BMA,CALLIN,F指令,启动获取GPS有效信息		*/
void SOS_BMA_F_GPS_Start(unsigned char CMD)
{
	GPS_Power_On();																		// 尽早打开GPS电源
	
	if(GPS_F.b.CMDF_Cnt_Flag == false)												// 未开启过获取GPS有效数据要求
	{
		cnt_1s.CMD_F_Count = 0;															// 计时从0开始
		GPS_F.b.CMDF_Cnt_Flag = true;
	}
		
	switch(CMD)
	{
		case 1: GPS_F.b.CMDF_Sendto_SMS_Flag = true; break;								// 需要回复到手机的定位信息
			
		case 2: GPS_F.b.CMDF_Sendto_Gprs_Flag = true;break;								// 需要回复到GPRS的定位信息
			
		case 3: GPS_F.b.CMDF_Sendto_SOS_BMA_Flag = true;
				GSMD.b.CMDF_SOS_Flag = true; 
				Task_SPI_Flash(Flash_Write,8,(u8*)&GSMD,1); break;						// 需要发送SOS报警定位信息
		
		case 4: GPS_F.b.CMDF_Sendto_SOS_BMA_Flag = true;
				GSMD.b.CMDF_BMAL_Flag = true;
				Task_SPI_Flash(Flash_Write,8,(u8*)&GSMD,1);break;						// 需要发送BMA跌落报警定位信息
		
		case 5: GPS_F.b.CMDF_Sendto_SOS_BMA_Flag = true;
				GSMD.b.CMDF_BMAH_Flag = true;
				Task_SPI_Flash(Flash_Write,8,(u8*)&GSMD,1);break;						// 需要发送BMA碰撞报警定位信息
				
		case 6: GPS_F.b.CMDF_Powron_5min_Flag = true;break;								// 开机5分钟读取定位信息
		
		case 7: GPS_F.b.CMDF_Ten_Track_Flag = true;	break;								// 每十分钟读取一次GPS
	}	
}

/* SOS/BMA发送GPS数据信息		*/
void SOS_BMA_Send_AlmMsg_And_Callout(unsigned char GPS_Type)
{	
	cnt_1s.BiBi_Cnt = 0;
						
	if(GSMD.b.CMDF_SOS_Flag)
	{
		send_alarm(0x01,"HELP ME!",ALM_A,GPS_Type);								// 发送SOS报警信息
	}
	else if(GSMD.b.CMDF_BMAL_Flag)
	{
		send_alarm(0x01,"Mdown!",ALM_M,GPS_Type);								// 发送跌落报警信息
	}
	else if(GSMD.b.CMDF_BMAH_Flag)
	{
		send_alarm(0x01,"Sudden Stop or Shock!",ALM_N,GPS_Type);				// 发送碰撞报警信息
	}
	
	if(GSMA.b.SOS_CALL_ENABLE_Flag && (!CALL.b.Calling_Flag))					// 需拨打电话功能
	{				
		if(GSMB.b.Callout_LisMode_Flag)											// 监听模式
		{
			Calling_Set_Mode(0,1,1);
		}
		else																	// 通话模式
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

/*sos拨打电话*/
/* 参数:是否可以主动挂机; 返回值: 0: 拨打失败; 1:拨打成功; 2:主动挂机返回 */
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
	
	while(count_call_again < Sos_cycle_num)													// 如果未拨打成功,则可以最多拨打次数不能超出设置值
	{		
		res=Callout_Phone(GSM_ph.temp_ph1,1,GSMB.b.Callout_LisMode_Flag,hangup,SOS_BMA);	// 拨打第一个号码
		if((res==0)&&(count_call_again++ > Sos_cycle_num)){break;}							// 如拨打失败且拨打超出次数,则退出
		else if((res==1)||(res==2)){break;	}												// 已经成功打通电话或主动挂机
			
		if(Sos_call_num >= 2)																// 如果允许拨打第二号码
		{	
			res=Callout_Phone(GSM_ph.temp_ph2,1,GSMB.b.Callout_LisMode_Flag,hangup,SOS_BMA);// 拨打第二个号码
			if((res==0)&&(count_call_again++ > Sos_cycle_num)){break;}						// 如拨打失败且拨打超出次数,则退出
			else if((res==1)||(res==2)){break;	}											// 已经成功打通电话或主动挂机
		}
		
		if(Sos_call_num >= 3)																// 如果允许拨打第三号码
		{
			res=Callout_Phone(GSM_ph.temp_ph3,1,GSMB.b.Callout_LisMode_Flag,hangup,SOS_BMA);// 拨打第三个号码
			if((res==0)&&(count_call_again++ > Sos_cycle_num)){break;}						// 如拨打失败且拨打超出次数,则退出
			else if((res==1)||(res==2)){break;	}											// 已经成功打通电话或主动挂机
		}
		
		if(Sos_call_num >= 4)																// 如果允许拨打第四号码
		{
			res=Callout_Phone(GSM_ph.temp_ph4,1,GSMB.b.Callout_LisMode_Flag,hangup,SOS_BMA);// 拨打第四个号码
			if((res==0)&&(count_call_again++ > Sos_cycle_num)){break;}						// 如拨打失败且拨打超出次数,则退出
			else if((res==1)||(res==2)){break;	}											// 已经成功打通电话或主动挂机
		}
		if(Sos_call_num >= 5)																// 如果允许拨打第五号码
		{
			res=Callout_Phone(GSM_ph.temp_ph5,1,GSMB.b.Callout_LisMode_Flag,hangup,SOS_BMA);// 拨打第五个号码
			if((res==0)&&(count_call_again++ > Sos_cycle_num)){break;}						// 如拨打失败且拨打超出次数,则退出
			else if((res==1)||(res==2)){break;	}											// 已经成功打通电话或主动挂机
		}
		if(Sos_call_num >= 6)																// 如果允许拨打第六号码
		{
			res=Callout_Phone(GSM_ph.temp_ph6,1,GSMB.b.Callout_LisMode_Flag,hangup,SOS_BMA);// 拨打第六个号码
			if((res==0)&&(count_call_again++ > Sos_cycle_num)){break;}						// 如拨打失败且拨打超出次数,则退出
			else if((res==1)||(res==2)){break;	}											// 已经成功打通电话或主动挂机
		}
		if(Sos_call_num >= 7)																// 如果允许拨打第七号码
		{
			res=Callout_Phone(GSM_ph.temp_ph7,1,GSMB.b.Callout_LisMode_Flag,hangup,SOS_BMA);// 拨打第七个号码
			if((res==0)&&(count_call_again++ > Sos_cycle_num)){break;}						// 如拨打失败且拨打超出次数,则退出
			else if((res==1)||(res==2)){break;	}											// 已经成功打通电话或主动挂机
		}
		if(Sos_call_num >= 8)																// 如果允许拨打第八号码
		{
			res=Callout_Phone(GSM_ph.temp_ph8,1,GSMB.b.Callout_LisMode_Flag,hangup,SOS_BMA);// 拨打第八个号码
			if((res==0)&&(count_call_again++ > Sos_cycle_num)){break;}						// 如拨打失败且拨打超出次数,则退出
			else if((res==1)||(res==2)){break;	}											// 已经成功打通电话或主动挂机
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

/* AT+CLCC查询电话状态 0:对方未接听;1:对方已接听;2:主动挂断电话*/
/* 参数: 打出电话是通话还是监听模式, 是否可以主动挂断			*/
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
		
		if((Event_CLCC_Key & (Key_Call1_S_Flag | Key_Call1_S_Flag)) && (clcc == '3'))		// 音量+
		{				
			if(!Callout_LisMode)															// 非监听模式下可以更改音量		
			{
				Spk_Vol_Turn(1);
			}			
		}
		else if((Event_CLCC_Key & (Key_Call2_S_Flag | Key_Call2_L_Flag)) && (clcc == '3'))	// 音量-
		{				
			if(!Callout_LisMode)															// 非监听模式下可以更改音量	
			{
				Spk_Vol_Turn(0);
			}			
		}
#else	
		
		if(Event_CLCC_Key & Key_Call1_S_Flag)												// 音量+
		{				
			if(!Callout_LisMode)															// 非监听模式下可以更改音量		
			{
				Spk_Vol_Turn(1);
			}			
		}
		else if(Event_CLCC_Key & Key_Call2_S_Flag)											// 音量-
		{				
			if(!Callout_LisMode)															// 非监听模式下可以更改音量	
			{
				Spk_Vol_Turn(0);
			}			
		}
		
#endif
		
#endif 	
		
		if(Event_CLCC_Key & Key_SOS_S_Flag)													// 在拨出电话过程程中,有SOS按下
		{
			if(hangup==true)																// 如果允许SOS挂断电话
			{
				ATH_P();																	// 挂断电话,退出通话处理
				return 2;
			}
		}
		else
		{
			clcc = CLCC();
						
			if(clcc)																		// 可以查询对通话状态
			{		
				if(clcc == '0')																// 已接通	
				{					
#if MODULE_SIM7500
					if(SOS_BMA && GSMC.b.DTMF_Det_Flag)										// DTMF
					{
						if(dtmf_start == true)												// 首次
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
					
					if(Callout_LisMode == true)												// 如果为监听模式
					{
						spk_mic(0,MIC_Lis);													// 打开喇叭声音,打开MIC声音		
					}
					else																	// 如果为通话模式
					{
						spk_mic(GSM_P.spk_volume,MIC_Spk);									// 打开喇叭声音,打开MIC声音
					}
				}	
#endif				
				else if((clcc == '1') || (clcc == '4') || (clcc == '5'))					// 如果对方未接听/通话中/等待，则退出
				{
					return 0;																// 返回失败
				}			
			}
			else																			// 方已挂机
			{	
				return 0;																	// 返回失败
			}
		}
	}
}

/*拨打电话返回值 0:对方未接听/电话号码错误/空号;1:对方已接听;2:主动挂断电话					*/
/*参数: 电话号码,最多重拨次数,打出通话或监听模式,是否可以主动挂断							*/
unsigned char Callout_Phone(char *PhoneNum,unsigned char Call_Time,bool Call_LisMode,bool hangup,unsigned char SOS_BMA)
{ 
	unsigned char i;
	unsigned char Res_Tmp=0;
	char ATD_PhoneNum_TMP[32]={0};	
	
	if(CALL.b.Calling_Flag) return 2;									// 如果正在通话,则直接退出
	
	if((*PhoneNum<0x30 && *PhoneNum!='+')||(*(PhoneNum)>0x39))	{return 0;	}	// 如果选择的号码未设置(判断第一位和第二位)，则直接退出	
	if(*(PhoneNum+1)<0x30 || *(PhoneNum+1)>0x39)				{return 0;	}	// 如果号码错误,则直接退出

	CALL.b.Calling_Flag = true;											// 预先建立通话标志	
		
	if(Call_LisMode == true)											// 如果为监听模式
	{	
#if MODULE_UG96
		spk_mic(0,MIC_Lis);												// 打开喇叭声音,打开MIC声音	
#endif		
	}
	else																// 如果为通话模式
	{		
		Speak_Amp_On();	
#if MODULE_UG96
		spk_mic(GSM_P.spk_volume,MIC_Spk);								// 打开喇叭声音,打开MIC声音
#endif
	}
				
	for(i=0;i<Call_Time;i++)											// 如果拨打失败,重复拨打同一电话次数
	{
		sprintf(ATD_PhoneNum_TMP,"D%s;",PhoneNum);						// 执行拨打电话指令ATD+号码+分号+\r\n	
		if(GSM_AT_Send(ATD_PhoneNum_TMP,6,3,1,0x0001))					// 打出电话成功
		{
			Res_Tmp=CLCC_P(Call_LisMode,hangup,SOS_BMA);				// 获取打出电话状态
			if(Res_Tmp) break;											// 如果对方已接听(1), 或者主动挂断(2)
		}
	}
	
	if(Res_Tmp != 1)													// 对方未接听或主动挂断,清除通话标志,重置打入电话设置
	{
		CALL.b.Calling_Flag = false;
		Speak_Amp_Off();								
		ATH_P();														// 防出错
	}
	
	return(Res_Tmp);													// 返回打出电话结果
}

/*设置喇叭、咪头音量*/
void spk_mic(u8 spk,u8 mic)
{
#if MODULE_SIM7500
	char buf1[24];
#else
	char buf1[12];
#endif
	
	if(spk > 10) {spk = 10;}
		
#if  MODULE_UG96																// 其它模块可以直接配置喇叭
	snprintf(buf1,12,"+CLVL=%d",spk*10);								// 喇叭音量
#elif MODULE_SIM7500
	snprintf(buf1,24,"+CWIIC=0x34,0x6C,0x%02X,1",spk * 6 + 0x83);		// 喇叭音量	
#endif
	GSM_AT_Send(buf1,6,1,2,0x0001);

#if MODULE_SIM7500
	if(mic > 8) {mic=8;}
	snprintf(buf1,12,"+CMICGAIN=%d",mic);								// MIC音量
	GSM_AT_Send(buf1,6,1,2,0x0001);
#elif MODULE_UG96
	if(mic > 15) {mic=15;}
	snprintf(buf1,12,"+QMIC=2,%d",mic);									// MIC音量
	GSM_AT_Send(buf1,6,1,2,0x0001);
#endif
}

/* 喇叭音量增加或减小	*/
void Spk_Vol_Turn(bool add_dec)
{
	if(add_dec == true)						// 增加音量
	{
		GSM_P.spk_volume++;

		if(GSM_P.spk_volume>10)	{GSM_P.spk_volume=10;	}
	}
	else									// 减小音量
	{
		GSM_P.spk_volume--;
		if(GSM_P.spk_volume==255){GSM_P.spk_volume=0;	}
	}
	
	spk_mic(GSM_P.spk_volume,MIC_Spk);		// 调用设置音量	
}


#endif


#if SMS_ENABLE

/* 比较指令,不需要验证授权号码*/
bool Compare_command(char* command)
{	
	if((command[0] == 'A') && (command[2] == ','))	{return true;}
	
	return false;
}


/*删除短信*/
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

/*提取短信的内容，手机与飞信兼容,p_123456为指向密码的指针变量
//+CMGR: "REC UNREAD","1252013510000786","","15/06/17,11:37:55+32" Kaisen: 525252A4,13145906426*/
//+CMGR: "REC UNREAD","13923474423",,"17/06/02,20:50:27+32"
//123456B
char * Read_sms_content(void)
{	
	char *pn,*p_123456;
	
	pn=strstr(gsm_rx_buf,"READ");													// 读取是否为取短信
	
	if(pn != NULL)																	// 未读取短信
	{
#if RESET_FUN	
		if(strstr(gsm_rx_buf,"RESET!")!=NULL) 
		{				
			if((strstr(gsm_rx_buf,"RESET!1")) != NULL)		
			{
				cmgd_p(false);
				Task_SPI_Flash(Flash_MCU_Reset,0,0,0);								// 只复位
			}
			else 			
			{
				cmgd_p(false);
				Task_SPI_Flash(Flash_Reset_ALL,0,0,0);								// 重置所有参数
			}
		}		
#endif
		
		p_123456 = strstr(pn,"\n");p_123456++;
			
		pn=strstr(pn,","); 															// 寻找电话号码前','
		pn=pn+2;pn=strtok(pn,"\"");															
		snprintf(SMS_Phone_Number,24,"%s",pn);										// 复制电话号码
				
		if(SMS_Phone_Number[0])														// 如果电话号码非空且密码相符
		{
			if(strncmp(p_123456,GSM_P.cmd_key,6) == 0)	{return p_123456+6;	}
			else if(!GSMC.b.SMS_Check_psw_Flag)			{return p_123456;	}
		}
	}

	return NULL;
}	

/* 处理新短信	*/
void New_SMS_Handle(void)
{  
	char buf1[9];
	unsigned char i=0;
		
	GSM_Wakeup();
	
	clr_gsm_buf();
	
	SMS.b.Chk_CMGL_Flag = true;											// 开启中断获取未读短信标志及计数
	send_str_to_gsm("AT+CMGL=\"REC UNREAD\"\r\n");	 					// 发送读取所有未读短信指令
	xEventGroupWaitBits(EventGroupHandler_OTH,CMGL_OK_Flag, pdFALSE, pdFALSE,10000);
	SMS.b.Chk_CMGL_Flag = false;
	
	if(SMS.b.New_SMS_Flag)
	{
#if MODULE_SIM7500
		for(i=0;i<=SMS_NUM;i++)												// 查询短信
#else
		for(i=1;i<=SMS_NUM;i++)												// 查询短信
#endif
		{		
			snprintf(buf1,9,"+CMGR=%d",i); 									// 读取短信示例：发送 "AT+CMGR=1\r\n"，读取第一条短信		

			if(GSM_AT_Send(buf1,6,1,1,0x0080))								// 先判断是否为OK,再判断是否为UNREAD或READ	
			{
				char *sms;
				sms=Read_sms_content(); 									// 取出短信内容	
				if(sms != NULL)
				{
#if COMPARE_PASWD
					if((Compare_command(sms) == true) || (Compare_phone_number(SMS_Phone_Number) != 0))
#else
					if(1)	
#endif
					{
						strtok(sms,"\r");strtok(sms,"#");
						analysis_sms_content(sms, RPLY_TO_SMS);				// 分析短信内容并做相应的处理
					}			
				}
			}
		}
		
		if(SMS_NUM > 30){cmgd_p(true);		}
		else			{cmgd_p(false);		}
		
		SMS_NUM = 0;
		SMS.b.New_SMS_Flag = false;
		cnt_1s.Chk_SMS_Ten_Cnt = 0;														// 删除所有已读短信
	}
}	

/*发送电话号码， 说明：ph_num为需发送的手机号，即接收方手机号码，示例：AT+CMGS="13145906426"\r\n*/
bool send_phone_number(char*ph_num)
{
	char pbuf1[30];
	unsigned char i=0;
	
	for(i=0;i<2;i++)
	{
		snprintf(pbuf1,30,"+CMGS=\"%s\"",ph_num);// 发送手机号码
		if(GSM_AT_Send(pbuf1,4,5,1,0x0002) == false)	
		{
			uartTxByte(&hlpuart1,0x1b);			// 发送字符
			send_str_to_gsm("\r\n");
			vTaskDelay(1000);
		}
		else									// 发送取消成功,返回
		{
			return true;
		}
	}

	return false;								// 出错返回
}	

/*发送短信到手机，步骤：先发送接收方手机号，再发送内容，最后发送 0x1A,0d,0a结束*/
void Send_Msg_To_Phone(char*PhoneNum,char*str1,u8 GPS_Type,u8 MSG_Type)
{	
	unsigned char i=0;
	EventBits_t Send_Result;
	
	if((PhoneNum[0] == 0) || (strlen(PhoneNum) <= 4)) return;		// 如果空号,直接退出
		
/*******************************************************************************************	
***********************************获取MCU_F\GPS数据******************************************
********************************************************************************************/	
	
	if((GPS_Type!=HISGPS)&&(GPS_Type!=NOGPS))		// 历史数据及无GPS数据,不需读取MCU_F信号强度/基站信息，电量
	{	
		Gsm_Get_Parameters(2);
	}
	
	if(GPS_Type)									// 读取GPS数据,并按相应格式打印
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
******************************************发送数据******************************************
********************************************************************************************/

	for(i=0;i<3;i++)
	{
		if(send_phone_number(PhoneNum)==0) return;			// 发送号码出错,则返回	
	
		clr_gsm_buf();
		
		if(GSM_OTA.Device[0] != '\0'){send_str_to_gsm(GSM_OTA.Device);send_str_to_gsm("\n");}
			
		send_str_to_gsm(str1); 								// 发送指令号
		
		if((GPS_Type!=NOGPS)&&(MSG_Type!=MSG_NO))
		{
			if(dtbuf[0] == 'V')								// 如果出现无效GPS数据
			{
				send_str_to_gsm("Before!");
			}
			
			send_str_to_gsm("\n");
			
			send_str_to_gsm(dtbuf+2);						// GPS/MCU_F数据
		}
		
		uartTxByte(&hlpuart1,0x1a);							// 发送结局字符
		send_str_to_gsm("\r\n");

		SMS.b.Chk_CMGS_Flag = true;
		Send_Result=xEventGroupWaitBits(EventGroupHandler_OTH,CMGS_Send_OK_Flag|CMGS_Send_NG_Flag, pdFALSE, pdFALSE,30000);
		SMS.b.Chk_CMGS_Flag = false;
	
		vTaskDelay(10);	
		
		if(Send_Result & CMGS_Send_OK_Flag)					// 发送成功
		{
			break;
		}
		else												// 发送失败
		{
			if(i<2)											// 前两次发送失败,取消重发,第三次不取消
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

/* 启动获取SEND OK				*/
void GPRS_Start_ChkSendok(void)
{
	clr_gsm_buf();	
	GPRS.b.Chk_Notif_Flag = true;
}

/* GPRS取消数据发送				*/
void GPRS_Cancel_Send(void)
{
	GPRS_Start_ChkSendok();				
	uartTxByte(&hlpuart1,0x1B); send_str_to_gsm("\r\n");vTaskDelay(1000);	
}

/* 发送GPRS数据后开启查询结果 */
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

/*发送信息到服务器			*/
/* 参数说明 发送指令/获取GPS类型/如果发送不出去是否需要保存*/
void Send_Msg_To_Platform(u8 GPRS_CMD,u8 GPS_Type,u8 MSG_Type,u8 Save)
{	
	u8 i=0;											// 发送次数
	
	/* 以下为判断是否需要获取GPS新数据及MCU_F基站,电量等信息*/	
	
	if((!GSMA.b.GPRS_flag) || MCU_F.b.No_Simcard_Flag) return;
	
	switch(GPRS_CMD)
	{
		case CMD_A:  _ptcmdx=",CMD-A,"; break;		// 指令A
		case CMD_A1: _ptcmdx=",CMD-A1,";break;		// 指令A1,设置更改电话号码1
		case CMD_A2: _ptcmdx=",CMD-A2,";break;		// 指令A2,设置更改电话号码2
		case CMD_A3: _ptcmdx=",CMD-A3,";break;		// 指令A3,设置更改电话号码3
		case CMD_A4: _ptcmdx=",CMD-A4,";break;		// 指令A4,设置更改电话号码4
		case CMD_A5: _ptcmdx=",CMD-A5,";break;		// 指令A5,设置更改电话号码5
		case CMD_A6: _ptcmdx=",CMD-A6,";break;		// 指令A6,设置更改电话号码6
		case CMD_A7: _ptcmdx=",CMD-A7,";break;		// 指令A7,设置更改电话号码7
		case CMD_A8: _ptcmdx=",CMD-A8,";break;		// 指令A8,设置更改电话号码8
		case CMD_B:  _ptcmdx= HS_VER;  	break;		// 指令B, 获取版本号
		case CMD_C:  _ptcmdx=",CMD-C,"; break;		// 指令C, 设置TCP/IP
		case CMD_D:	 _ptcmdx=",CMD-D,"; break;		// 指令D, 设置开启GPRS
		case CMD_E:	 _ptcmdx=",CMD-E,"; break;		// 指令E, 设置关闭GPRS
		case CMD_F:  _ptcmdx=",CMD-F,"; break;		// 指令F, 获取单次定位
		case CMD_F1: _ptcmdx=",CMD-F1,";break;		// 指令F1,获取基站定位
		case CMD_G:  _ptcmdx=",CMD-G,"; break;		// 指令G, 
		case CMD_H:  _ptcmdx=",CMD-H,"; break;		// 指令H, 设置更新密码
		case CMD_I:  _ptcmdx=",CMD-I,"; break;		// 指令I, 设置GEO
		case CMD_J:	 _ptcmdx=",CMD-J,"; break;		// 指令J, 设置速度报警
		case CMD_K:	 _ptcmdx=",CMD-K,"; break;		// 指令K, 指令关机
		case CMD_K0: _ptcmdx=",CMD-K0,";break;		// 指令K, 设置开启加速报警
		case CMD_K1: _ptcmdx=",CMD-K1,";break;		// 指令K, 设置开启失重报警
		case CMD_K2: _ptcmdx=",CMD-K2,";break;		// 指令K, 设置关闭加速报警
		case CMD_K3: _ptcmdx=",CMD-K3,";break;		// 指令K, 设置关闭失重报警
		case CMD_L:  _ptcmdx=",CMD-L,"; break;		// 指令L, 设置时区
		case CMD_L1: _ptcmdx=",CMD-L1,";break;		// 指令L1,设置内置时钟年月日/时间
		case CMD_L2: _ptcmdx=",CMD-L2,";break;		// 指令L2,设置
		case CMD_L3: _ptcmdx=",CMD-L3,";break;		// 指令L3,设置OLED显示角度
		case CMD_L4: _ptcmdx=",CMD-L4,";break;		// 指令L4,设置重新获取GPS时间
		case CMD_L5: _ptcmdx=",CMD-L5,";break;		// 指令L5,设置闹铃
		case CMD_M:	 _ptcmdx=",CMD-M,"; break;		// 指令M, 设置连续定位
		case CMD_M2: _ptcmdx=",CMD-M2,";break;		// 指令M2,设置心跳
		case CMD_M3: _ptcmdx=",CMD-M3,";break;		// 指令M3,设置GPRS发送连续定位时间间隔
		case CMD_M4: _ptcmdx=",CMD-M4,";break;		// 指令M4,设置GPS输出模式
		case CMD_N:	 _ptcmdx=",CMD-N,"; break;		// 指令N, 设置低电报警
		case CMD_O:	 _ptcmdx=",CMD-O,"; break;		// 指令O, 设置APN
		case CMD_P:	 _ptcmdx=",CMD-P,"; break;		// 指令P, 设置位移报警
		case CMD_Q1: _ptcmdx=",CMD-Q1,";break;		// 指令Q1,设置SOS是不需要打出电话
		case CMD_Q2: _ptcmdx=",CMD-Q2,";break;		// 指令Q2,设置是否需要验证来电为授权号码
		case CMD_Q3: _ptcmdx=",CMD-Q3,";break;		// 指令Q3,设置来电为通话模式时,是否可以主动接听或挂断来电
		case CMD_Q4: _ptcmdx=",CMD-Q4,";break;		// 指令Q4,设置打出电话是否启用语音信箱识别
		case CMD_Q5: _ptcmdx=",CMD-Q5,";break;		// 指令Q5,设置打出短信是否需要比对密码
		case CMD_R:	 _ptcmdx=",CMD-R,"; break;		// 指令R, 设置域名
		case CMD_S:	 _ptcmdx=",CMD-S,"; break;		// 指令S
		case CMD_T:	 _ptcmdx=",CMD-T,"; break;		// 指令T, 主动发送连续定位数据
		case CMD_U:  _ptcmdx=",CMD-U,"; break;		// 指令U, OTA升级
		case CMD_U0: _ptcmdx=",CMD-U0,";break;		// 指令U0,设置打入电话为通话 
		case CMD_U1: _ptcmdx=",CMD-U1,";break;		// 指令U1,设置打入电话为监听
		case CMD_U2: _ptcmdx=",CMD-U2,";break;		// 指令U2,设置打出电话为监听
		case CMD_U3: _ptcmdx=",CMD-U3,";break;		// 指令U3,设置打出电话为通话
		case CMD_U4: _ptcmdx=",CMD-U4,";break;		// 指令U4,设置允许开启GPS电源
		case CMD_U5: _ptcmdx=",CMD-U5,";break;		// 指令U5,设置禁止开启GPS电源
		case CMD_U6: _ptcmdx=",CMD-U6,";break;		// 指令U6,设置GPRS省流量模式
		case CMD_U7: _ptcmdx=",CMD-U7,";break;		// 指令U7,设置GPRS按设置时间上传
		case CMD_U8: _ptcmdx=",CMD-U8,";break;		// 指令U8,设置GPS超强模式,不关断电源
		case CMD_U9: _ptcmdx=",CMD-U9,";break;		// 指令U9,设置GPS省电模式,会关断电源
		case CMD_V:	 _ptcmdx=",CMD-V,"; break;		// 指令V
		case CMD_V3: _ptcmdx=",CMD-V3,";break;		// 指令V3,设置打出相应电话
		case CMD_W:	 _ptcmdx=",CMD-W,"; break;		// 指令W
		case CMD_X:	 _ptcmdx=",CMD-X";	break;		// 指令X, 主动定时发送心跳数据
		case CMD_Y:	 _ptcmdx=",CMD-Y,"; break;		// 指令Y
		case CMD_Z1: _ptcmdx=",CMD-Z1,";break;		// 指令Z1
		case CMD_Z2: _ptcmdx=",CMD-Z2,";break;		// 指令Z2
		
		case ALM_A:	 _ptcmdx=",ALM-A,"; break;		// 警报A, 主动发送SOS报警
		case ALM_B1: _ptcmdx=",ALM-B1,";break;		// 警报B1,主动发送GEO1报警	
		case ALM_B2: _ptcmdx=",ALM-B2,";break;		// 警报B2,主动发送GEO2报警	
		case ALM_B3: _ptcmdx=",ALM-B3,";break;		// 警报B3,主动发送GEO3报警	
		case ALM_B4: _ptcmdx=",ALM-B4,";break;		// 警报B4,主动发送GEO4报警	
		case ALM_B5: _ptcmdx=",ALM-B5,";break;		// 警报B5,主动发送GEO5报警	
		case ALM_C:	 _ptcmdx=",ALM-C,"; break;		// 警报C, 主动发送超速报警
		case ALM_D:	 _ptcmdx=",ALM-D,";	break;		// 警报D, 主动发送低电报警
		case ALM_E:	 _ptcmdx=",ALM-E";	break;		// 警报E, 主动发送GPS长时间无有效数据报警
		case ALM_F:	 _ptcmdx=",ALM-F,"; break;		// 警报F, 主动发送充满电报警	
		case ALM_G:	 _ptcmdx=",ALM-G,"; break;		// 警报G
		case ALM_H:	 _ptcmdx=",ALM-H,"; break;		// 警报H, GPS位移报警		
		case ALM_I:	 _ptcmdx=",ALM-I,"; break;		// 警报I	
		case ALM_J:	 _ptcmdx=",ALM-J,"; break;		// 警报J
		case ALM_K:	 _ptcmdx=",ALM-K,"; break;		// 警报K
		case ALM_L:	 _ptcmdx=",ALM-L,"; break;		// 警报L		
		case ALM_M:	 _ptcmdx=",ALM-M,"; break;		// 警报M, BMA跌落报警
		case ALM_N:	 _ptcmdx=",ALM-N,"; break;		// 警报N, BMA碰撞报警 
		case ALM_O:	 _ptcmdx=",ALM-O,"; break;		// 警报O
		case ALM_P:	 _ptcmdx=",ALM-P,"; break;		// 警报P
		case ALM_Q:	 _ptcmdx=",ALM-Q,"; break;		// 警报Q
		case ALM_R:	 _ptcmdx=",ALM-R,"; break;		// 警报R
		case ALM_S:	 _ptcmdx=",ALM-S,"; break;		// 警报S
		case ALM_T:	 _ptcmdx=",ALM-T,"; break;		// 警报T
		case ALM_U:	 _ptcmdx=",ALM-U,"; break;		// 警报U, OTA升级成功
		case ALM_V:	 _ptcmdx=",ALM-V,"; break;		// 警报V
		case ALM_W:  _ptcmdx=",ALM-V"; 	break;		// 警报W
		case ALM_X:  _ptcmdx=",ALM-X"; 	break;		// 警报X
		case ALM_Y:	 _ptcmdx=",ALM-Y,"; break;		// 警报Y
		case ALM_Z:	 _ptcmdx=",ALM-Z,"; break;		// 警报Z
		
		case CMD_ERR:_ptcmdx=",ERROR!";	break;		// 指令错误
		
		default:     return;						// 指令错误
	}

	if((GPS_Type!=HISGPS)&&(GPS_Type!=NOGPS))		// 历史数据及无GPS数据,不需读取MCU_F信号强度/基站信息，电量
	{	
		Gsm_Get_Parameters(2);
	}
	
	if(GPS_Type)									// 读取GPS数据,并按相应格式打印
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
		
	if((GPRS_CMD == CMD_T) && Save)															// 如果是发连续定位数据,且需要保存,但是历史数据还未发送完,则先保存数据不发送,等待所有历史数据发送完成后再最后一起发送
	{
		if(GSM_P.send_history < GSM_P.store_history)
		{
			i=200;
		}
	}
		
	/********************************************************************************************/	
	/***********************************启动发送，检测‘>’****************************************/	
	/********************************************************************************************/
	// 示例：#867273021518134,CMD-T,A,DATE:160113,TIME:140709,LAT:22.7613506N,LOT:114.3610382E,Speed:000.3,X-0-1-100-26,000,46000-262D-FF86#	
			
	if(GPRS.b.Connected_OK_Flag && (i==0))													// 如果当前网络正常,且不符合以上条件
	{									
		for(;i<2;i++)																		// 循环2次，超过次数退出
		{	
			if(CALL.b.Ring_Flag || CALL.b.Calling_Flag)										// 如果有来电或需要立即刷屏,立即退出,并保存
			{
				i=200;break;																// 有来电		
			}
			
#if MODULE_UG96
			if(GSM_AT_Send("+QISEND=1",4,2,1,0x0002))										// 连接正常
#elif MODULE_SIM7500
			if(GSM_AT_Send("+CIPSEND=0,",3,10,1,0x0002))									// 连接正常
#endif
			{
	/********************************************************************************************/	
	/******************************************发送数据******************************************/	
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

				if(GPRS_Check_SendOK())														// 如果发送成功
				{	
					cnt_1s.HeartBeat_Cnt = 0;break;
				}
				else
				{
					GPRS_Cancel_Send();
					
					if((i>=1) || (GPRS.b.Connected_OK_Flag == false))						// 如果发送不出去,且次数达到2次
					{
						GPRS.b.Connected_OK_Flag = false;cnt_1s.GPRS_Reconnect_Cnt++;Task_AT_GSM(Gprs_ReConnect_Now,0,0,0,0,0,0);break;
					}
				}
			}
			else																			// 如果连接出现异常
			{
				GPRS_Cancel_Send();
				
				if((i>=1) || (GPRS.b.Connected_OK_Flag == false))																	
				{
					GPRS.b.Connected_OK_Flag = false;cnt_1s.GPRS_Reconnect_Cnt++;Task_AT_GSM(Gprs_ReConnect_Now,0,0,0,0,0,0);break;
				}
			}
		}		
	}
		
	if(((GPRS.b.Connected_OK_Flag == false) || (i==200)) && Save) 							// 已获取网络断线/有来电急退出发送/有断网CMD-T历史数据未发送完,需要再发CMD-T信息,并且需要保存数据
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

/*GPRS附着*/
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
		gpsx.posslnum = 1;						// 仿真GPS接收到数据
		
		p1=strstr(gsm_rx_buf,"-");
		
		if(p1 != NULL)
		{
			snprintf(GPS_RMC_Buf,80,"RMC,%c%c%c%c%c%c,,,,,,,,%c%c%c%c%c%c,",*(p1+7),*(p1+8),*(p1+10),*(p1+11),*(p1+13),*(p1+14),*(p1+4),*(p1+5),*(p1+1),*(p1+2),*(p1-2),*(p1-1));			
			NMEA_GPRMC_Analysis();
		}
	}	
}

/*查询当前GPRS连接状态, 参数: 1: 需要延时等待*/
bool GPRS_Check(void)
{											
#if MODULE_UG96
	return(GSM_AT_Send("+QISTATE=1,1",6,2,1,0x4000));
#elif MODULE_SIM7500
	return(GSM_AT_Send("+CIPOPEN?",40,2,1,0x4000));
#endif
}

/*关闭GPRS连接*/
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

/*开始配置GPRS网络参数并连接*/
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

/*启动GPRS连接*/
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
	
	if(modeflag == 2)																						// NIST联网
	{																
#if MODULE_UG96
		snprintf(dtbuf,80,"+QIOPEN=1,1,\"TCP\",\"time.nist.gov\",13,0,0");										
#elif MODULE_SIM7500
		snprintf(dtbuf,80,"+CIPOPEN=0,\"TCP\",\"time.nist.gov\",13");												
#endif
	}
	else if(modeflag == 1)																					// 域名联网
	{								
#if MODULE_UG96
		snprintf(dtbuf,80,"+QIOPEN=1,1,\"%s\",\"%s\",%s,0,0",GSM_P.TCP_UDP,GSM_P.domain_name,GSM_P.domain_port);				
#elif MODULE_SIM7500
		snprintf(dtbuf,80,"+CIPOPEN=0,\"%s\",\"%s\",%s",GSM_P.TCP_UDP,GSM_P.domain_name,GSM_P.domain_port);									
#endif
	}
	else																									// IP联网
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
		return 0;									// 否则报错
	}
		
#endif
}	

/*连接GPRS,采用AT+CIPCSGP 和AT+CIPSTART*/
// Reset_UTC 
//           0:先查询GPRS连接状态,如果失败则启动连接; 
//			 1:不查询GPRS状态,强制启动联网; 
//			 2:连接到NIST网站,获取UTC时间
bool GPRS_Connect(unsigned char Reset_UTC)
{	 
	u8 i;
	
	if((GSMA.b.GPRS_flag == false)&&(Reset_UTC != 2))									// 如果未开启GPRS功能,则直接返回			
	{
		return true;	
	}	
					
	if((Reset_UTC==0)&&(GPRS_Check()==true))											// 如果是强制重设,则不测试GPRS状态;否则等待1秒,如果网络正常则退出,否则重新配置
	{
		cnt_1s.HeartBeat_Cnt=0;
		GPRS.b.Chk_Ciprxget_Flag = true;
		GPRS.b.Connected_OK_Flag = true;return true;									
	}
	else
	{	
		GPRS.b.Connected_OK_Flag = false;
		
		if(Reset_UTC == 2)																// 需要获取UTC时间
		{
			bool GPRS_Temp_Flag = GSMA.b.GPRS_flag;
			
			GSMA.b.GPRS_flag = 1;
			
			GPRS.b.Chk_Ciprxget_Flag = true;											// 预先启动接收平台数据
					
			GPRS_Disconnect();															// 先删除原配置

			if(GPRS_APN_Connect())														// 其它模块按域名连接
			{
				GPRS_TCPIP_Connect(2);													// 如果联网成功
			}
			
			UTC();		
			GPRS_Disconnect();
			GSMA.b.GPRS_flag = GPRS_Temp_Flag;
			return false;
		}
		//wifi连接
		else if((strlen(GSM_P.domain_name)>2)&&(strlen(GSM_P.domain_port)>2))			// 检查域名和端口有无设置
		{
			for(i=0;i<2;i++)
			{
				if(CALL.b.Ring_Flag){break;} 											// 有电话打入或有按键盘需要处理,则立即退出
				
				GPRS_Disconnect();														// 先删除原配置

				if(GPRS_APN_Connect())													// 其它模块按域名连接
				{
					if(GPRS_TCPIP_Connect(1)){i=3;break;	}							// 如果联网成功
				}
			}		
		}																
		else if((strlen(GSM_P.ip_addr)>2)&&(strlen(GSM_P.ip_port)>2))					// 按TCP/IP连接			
		{
			for(i=0;i<2;i++)
			{
				if(CALL.b.Calling_Flag){break;} 										// 有电话打入或有按键盘需要处理,则立即退出
				
				GPRS_Disconnect();

				if(GPRS_APN_Connect())
				{
					if(GPRS_TCPIP_Connect(0)){i=3;break;	}
				}			
			}
		}
		else																			// 如果域名和IP均未设置,则关闭GPRS,但不保存
		{
			GSMA.b.GPRS_flag=false;return 0;
		}		
	}
	
	if(GPRS.b.Connected_OK_Flag)														// 如果联网成功
	{
		if(GPRS_Check()==true)
		{	
			cnt_1s.HeartBeat_Cnt=0;
			GPRS.b.Chk_Ciprxget_Flag = true;											// 启动接收平台数据
			Send_Msg_To_Platform(CMD_D,NOGPS,MSG_NO,No_Save);							// 发送平台GPRS已连接信息
			
			if(SMS.b.CMDD_SMS_Flag)														// 回复短信
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
		
		if(SMS.b.CMDD_SMS_Flag)															// 回复短信
		{
			SMS.b.CMDD_SMS_Flag = false;
			Send_Msg_To_Phone(SMS_Phone_Number_Backup,"Connect Fail,Please Reinstall APN,IP&Port!",NOGPS,MSG_NO);
		}
		
		return false;
	}
}

/*读GPRS数据*/
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
			Task_SPI_Flash(Flash_Reset_ALL,0,0,0);									// 重置参数后复位
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
			GPRS.b.Connected_OK_Flag = false;				// 已经断网
			Task_AT_GSM(Gprs_ReConnect_Now,0,0,0,0,0,0);	
			return;
		}
	}
	
	GPRS.b.Chk_Ciprxget_Flag = true;
}

#endif

/*分析信息内容  需要处理的指令/处理完成后回复通道*/
void analysis_sms_content(char *Str,unsigned char Reply)
{ 
	if(Str!=NULL)
	{						 			
		switch(Str[0])										// 判断第一个字符
		{
		#if (RESET_FUN==1)									// 串口设置恢复出厂默认设置 
			case '1': CMD_1_Handle(Str+1,Reply);break;		// 示例: SET1
			case '2': CMD_2_Handle(Str+1,Reply);break;		// 示例: SET2				
		#endif	
		
		#if (GPRS_FUN == 1)
			case '4': CMD_4_Handle(Str+1,Reply);break;		// 打开GPRS功能,示例: SET4
			case '5': CMD_5_Handle(Str+1,Reply);break;		// 关闭GPRS功能,示例: SET5
		#endif
				
		#if (A_FUN==1)										// 设置授权1-8号码,  		
			case 'A': CMD_A_Handle(Str+1,Reply);break;		// 示例: 123456A1,+8613145906426   删除号码123456A1,D	
		#endif
			
		#if (B_FUN==1)										// 获取IMEI号					
			case 'B': CMD_B_Handle(Str+1,Reply);break;		// 示例: 123456B		 
		#endif
		
		#if GPRS_ENABLE									 
		#if (C_FUN==1)   									// 设置网络连接参数 			    				 				
			case 'C': CMD_C_Handle(Str+1,Reply);break;		// 示例: 123456CCMNET,183.12.181.141:19003 	 
		#endif	
				
		#if (D_FUN==1)										// 短信开启GPRS									
			case 'D': CMD_D_Handle(Str+1,Reply);break;		// 示例: 123456D	
		 #endif									 
			
		#if (E_FUN==1)										// 短信或GPRS关闭GPRS  											 
			case 'E': CMD_E_Handle(Str+1,Reply);break;		// 示例: 123456E	
		#endif	
				
		#endif						
																 
		#if (F_FUN==1)										// 单次定位 									 
			case 'F': CMD_F_Handle(Str+1,Reply);break;		// 示例: 123456F		
		#endif	

		#if (G_FUN==1)										// 获取用户设置的参数 		         
			case 'G': CMD_G_Handle(Str+1,Reply);break;		// 示例: 123456G 	
		#endif	
									
		#if (H_FUN==1)										// 更改授权密码 			 											
			case 'H': CMD_H_Handle(Str+1,Reply);break;		// 示例: 123456H999999					           		
		#endif
								 
		#if (I_FUN==1)										// 设置矩形电子围栏参数  	
			case 'I': CMD_I_Handle(Str+1,Reply);break;		// 示例123456I1,1,1,51113525N009125670E50241115N011011173E
		#endif
									 
		#if (J_FUN==1)										// 设置超速报警								 
			case 'J': CMD_J_Handle(Str+1,Reply);break;		// 示例: 123456J1,080
		#endif
								
		#if (K_FUN==1)										// 关闭电源,设置BMA250	
			case 'K': CMD_K_Handle(Str+1,Reply);break;		// 示例: 123456KH,050	123456KL,050	123456K				 
		#endif 										
							 
		#if (L_FUN==1)										// 设置本地时间,调整LED显示时间					 
			case 'L': CMD_L_Handle(Str+1,Reply);break;		// 示例: 123456L+08 		123456L，170101，120000			 
		#endif

		#if (M_FUN==1)										// 设置持续定位参数,心跳时间															
			case 'M': CMD_M_Handle(Str+1,Reply);break;		// 示例: 123456M1,030S	123456M2，120		
		#endif
				
		#if (N_FUN==1)										// 设置低电报警  									
			case 'N': CMD_N_Handle(Str+1,Reply);break;		// 示例: 123456N1,40		
		#endif	

		#if (O_FUN==1)										// 设置用户名和密码											
			case 'O': CMD_O_Handle(Str+1,Reply);break;		// 示例: 123456Ointernet,internet123			
		#endif	

		#if (P_FUN==1)										// 设置圆形电子围栏 									
			case 'P': CMD_P_Handle(Str+1,Reply);break;		// 示例: 123456P1，020
		#endif	
								
		#if (Q_FUN==1)										// 设置SOS是否打电话				
			case 'Q': CMD_Q_Handle(Str+1,Reply);break;		// 示例: 123456Q1	123456Q0	
		#endif										
							 
		#if (R_FUN==1)										// 设置域名、端口					
			case 'R': CMD_R_Handle(Str+1,Reply);break;		// 示例: 123456Rhttp:160162yx73.imwork.net;1290								
		#endif	

		#if (S_FUN==1)											
			case 'S': CMD_S_Handle(Str+1,Reply);break;		// 设置开启或关闭串口调度							
		#endif	

		#if (T_FUN==1)
			case 'T':							break;				 				 
		#endif		 

		#if (U_FUN==1)										// 设置监听、通话、GPS工作模式等									
			case 'U': CMD_U_Handle(Str+1,Reply);break;		// 示例: 123456U0	123456UV 	123456UV01 
		#endif	
						 
		#if (V_FUN==1)										// 发指令打出电话,存入第八个号码,V31监听打出,V30通话打出  	 					 
			case 'V': CMD_V_Handle(Str+1,Reply);break;		// 示例: 123456V30,9963	123456V31,9963 	
		#endif	      								 

		#if (W_FUN==1)																		
			case 'W': CMD_W_Handle(Str+1,Reply);break;		//	TIM-将设置的WIFI-msg保存至结构体
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


/*发送信息到串口、短信、服务器 */
/*PC_CMD--发到串口的内容, SMS_CMD 发到短信内容, GPRS_CMD发送到GPRS内容, BackAndGPSType发送通道+GPS获取型,SMS_MSG_Type短信发送格式,GPRS_MSG_Type GPRS发送格式, Save不能正常是否需要保存*/
void send_info(char *PC_CMD,char *SMS_CMD,u8 GPRS_CMD,u8 BackAndGPSType,u8 SMS_MSG_Type,bool Save)
{
	switch(BackAndGPSType&0x70)
	{
		case RPLY_TO_PC: //发到串口
			printf("%s",PC_CMD);
			break; 

#if SMS_ENABLE
		case RPLY_TO_SMS: //收到短信指令返回	
			Send_Msg_To_Phone(SMS_Phone_Number,SMS_CMD,BackAndGPSType&0x0F,SMS_MSG_Type);  
			break;	
#endif

#if GPRS_ENABLE
		case RPLY_TO_GPRS://收到GPRS指令返回
			Send_Msg_To_Platform(GPRS_CMD,BackAndGPSType&0x0F,MSG_GPRS,Save);
			break;
#endif

		default:break;	 					
	}			  	
}

/* 设置连续定位工作模式	*/
void Set_Track_Time(void)
{	
	switch(GSM_P.Track_Set_Unit)
	{		 
		case 'S': cnt_1s.Track_Time_Set=1;   break;		 
		case 'M': cnt_1s.Track_Time_Set=60;  break;
		case 'H': cnt_1s.Track_Time_Set=3600;break;
		default:break;							 
	}	
	
	cnt_1s.Track_Time_Set = GSM_P.Track_Set_Time * cnt_1s.Track_Time_Set;	// 获取实际设置值
	
	cnt_1s.Track_Time_Cnt=0;SMS.b.Track_SMS_Flag = false;GPRS.b.Track_GPRS_Flag = false;
		
	GPS_F.b.Track_For_sms_Flag = false; GPS_F.b.Track_For_gprs_flag = false;
	
	cnt_1s.Track_Save_Cnt=0;GPRS.b.GPRS_Save_now_Flag = false;
	
	cnt_1s.Track_wifi_Cnt = 0;
				
	if((cnt_1s.Track_Time_Set > 0) && GSMA.b.Track_Flag)					// 有设置开启连续定位时间
	{		
		if(cnt_1s.Track_Time_Set <= 600)									// 如果连续定位时间小于10分钟,每次GPS电源开启时间为5秒							
		{
			Track_Time_Unit = 5;
		}
		else																// 如果连续定位时间大于10分钟,每次按15秒增加	
		{
			Track_Time_Unit = 15;
		}
						
		if(GSMA.b.GPRS_flag)												// 如果已开启GPRS功能
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

/* 串口设置恢复出厂设置	*/
void CMD_1_Handle(char *Str1,unsigned char Reply1)
{
	if(Reply1 == RPLY_TO_PC)
	{
		printf(" OK!\r\n");
		Task_SPI_Flash(Flash_Reset_ALL,0,0,0);
	}
}

/* 机器重启				*/
void CMD_2_Handle(char *Str2,unsigned char Reply2)
{
	if(Reply2 == RPLY_TO_PC)
	{
		printf(" OK!\r\n");
		Task_SPI_Flash(Flash_MCU_Reset,0,0,0);
	}
}

/* 开启GPRS设置			*/
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

/* 关闭GPRS设置			*/
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

/*设置授权号码、删除授权号码*/
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
			send_info(Reply_Data,Reply_Data,CMD_A1+(StrA[0]&0x0F)-1,ReplyA|OLDGPS,MSG_NO,Do_Save);// 先回复信息
			Task_SPI_Flash(Flash_Write,phoneaddr,		(u8*)&GSM_ph,		GSM_ph_size);
			Task_SPI_Flash(Flash_Write,phonenameaddr,	(u8*)&GSM_phname,	phonenamesize);
		}
	}
	
	if(Err == true)																															// 否则回复指令错误
	{
		send_info(" ERROR!","error phone number!",CMD_ERR,ReplyA|OLDGPS,MSG_NO,Do_Save);
	}
}

/* 输出版本信息*/
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

/* 设置GPRS APN, IP, Port 地址*/
void CMD_C_Handle(char *StrC,unsigned char ReplyC)			// SETCapn,x.x.x.x:yyyy
{
	bool Err=false;
	bool TCP=true;
	char *p1,*p2,*p3;
	unsigned char i,Di=0,Dp=0,Dp_Cnt=0;
		
	p1=StrC;												// 获取APN
	
	p2=strchr(StrC,',');									// 
	
	if(((*(p2+1) == 'u') || (*(p2+1) == 'U')) && (*(p2+2) == ','))
	{
		TCP = false;p2=p2+2;
	}

	p3=strchr(StrC,':');									// 
	
	if((p2 == NULL) && (p3 == NULL))						// 只设置APN
	{
		snprintf((char*)GSM_P.apn,30,"%s", p1);				// 最多保存20个字符
		GSM_P.ip_addr[0]='\0';	
		GSM_P.ip_port[0]='\0';
	}
	else													// 要设置APN，IP，PORT
	{	
		p2++;p3++;											// 查找','之后的字符IP地址,查找":"之后的字符端口号
		
		if(p1[0]==',')										// APN数据为空
		{
			GSM_P.apn[0]='\0';	
		}				
		else												// 有APN数据			
		{
			strtok(p1,",");
			snprintf((char*)GSM_P.apn,30,"%s", p1);			// 最多保存10个字符
		}					
		
		if(p2[0]==':')										// IP地址为空	
		{
			GSM_P.ip_addr[0]='\0';	
		}					
		else												// IP地址有数据			
		{
			strtok(p2,":");
			if(strlen(p2)<16)								// IP长度小于16
			{
				for(i=0;i<strlen(p2);i++)					// 检查IP
				{
					if(isdigit(p2[i]))						// 检查到数字
					{
						Di++;Dp=0;							// 数字统计,小数点清0
						if(Di > 3) {Err=true;break;}		// 连续数字不超出3个
					}  
					else if(p2[i] == '.') 
					{
						Dp++;Di=0;Dp_Cnt++;					// 小数点统计,数字统计清0
						if(Dp > 1) {Err=true;break;}		// 连续小数点超出1个
					}			
					else									// 非数字及小数点
					{
						Err=true;break;	
					}
				}
				if(Dp_Cnt != 3)								// 小数点总数不等于3
				{
					Err = true;
				}
				if(Err == false)
				{
					snprintf((char*)GSM_P.ip_addr,18,"%s",p2);// 最多保存15个字符
				}
			}
			else
			{
				Err=true;
			}	
		}					
		
		if(p3[0] != '\0')
		{
			if(strlen(p3) < 6) 								// 检查端口号字符长度
			{
				for(i=0;i<strlen(p3);i++)
				{
					if(!isdigit(p3[i])) {Err=true;break;}	// 非数字，出错
				}
				if(atoi(p3) > 65535)	{Err=true;		}	// 数据大于65535，出错
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

/* 开启GPRS */
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
			strcpy(SMS_Phone_Number_Backup,SMS_Phone_Number);	// 复制号码
			SMS.b.CMDD_SMS_Flag = true;
		}
	}
	else
	{
		send_info(" ERROR!","",CMD_ERR, ReplyD|NOGPS,MSG_NO,Do_Save);									 
	}
}

/* 关闭GPRS */
void CMD_E_Handle(char *StrE,unsigned char ReplyE)
{
	if((ReplyE != RPLY_TO_PC) && (StrE[0] == '\0')) 								 
	{
		GSMA.b.GPRS_flag=0;	
		GPRS.b.Connected_OK_Flag = false;			
		Task_SPI_Flash(Flash_Write,0,(u8*)&GSMA.w,1);		//保存参数GPRS_flag 
		Task_AT_GSM(Gprs_Disconnect,0,0,0,0,0,0);	
		Set_Track_Time();
		if(ReplyE != RPLY_TO_GPRS)
		{
			send_info("","Closed GPRS OK!",CMD_ERR,ReplyE|NOGPS,MSG_NO,No_Save);	
		}
	}
}

/* 单次定位 */
void CMD_F_Handle(char *StrF,unsigned char ReplyF)
{
	if(StrF[0] == '\0')
	{
		if(ReplyF == RPLY_TO_SMS)		
		{
			strcpy(SMS_Phone_Number_Backup,SMS_Phone_Number);		// 复制号码
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
	if(ReplyG == RPLY_TO_GPRS) 								// 发到服务器
	{
		send_info("","",CMD_G,ReplyG|OLDGPS,MSG_NO,Do_Save);return;
	}
		
	if(ReplyG == RPLY_TO_PC) 								// 发到串口
	{
		Task_SPI_Flash(Flash_SETG,0,0,0);
	}
	else if(ReplyG == RPLY_TO_SMS)							//发到短信									              
	{		
		snprintf(dtbuf,160,"A%s;B%s;C%s;D%s;E%s;F%s;G%s;H%s;%s;%s;%s;%s;%s;Speed:%d;%d;%d;%d;%d;%d;BAT:%d,%d%%,%d%c,Lg:%d,Hg:%d",\
		GSM_ph.temp_ph1,GSM_ph.temp_ph2,GSM_ph.temp_ph3,GSM_ph.temp_ph4,GSM_ph.temp_ph5,GSM_ph.temp_ph6,GSM_ph.temp_ph7,GSM_ph.temp_ph8,GSM_P.apn,GSM_P.APN_username,GSM_P.APN_password,
		GSM_P.ip_addr,GSM_P.ip_port,GSM_P.set_speed,GSMA.b.ALM_OverSpeed_Flag,GSMA.b.Track_Flag,
		(gpsx.AV_indicate=='A'? 1:0),GSMA.b.ALM_Circle_GEO_Flag,GSMA.b.ALM_LowBat_Flag ,gpsx.soc,GSM_P.set_soc&0x7F,GSM_P.Track_Set_Time,GSM_P.Track_Set_Unit,GSM_P.lowg,GSM_P.higg);
		send_info("",dtbuf,CMD_ERR,ReplyG|NOGPS,MSG_NO,Do_Save);	
	}		
}

/* 设置短信密码*/
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

/* 设置GEO */
void CMD_I_Handle(char *StrI,unsigned char ReplyI)
{
	bool err_data_flag=0;															// 数据格式错误标志

	char *p1;
	char pbuf1[4]={0};	

	u8 row,GEO_OnOff;	
	u8 GEO_InOut_Flag=0;		
	u8 hi=0;	
	
	p1=StrI;
	
	row = *p1-0x30; 																// 组号	
	if((row>5)||(row<1))		{err_data_flag = 1;	} 								// 组号不超过5 
										
	p1+=2;
	if(*p1!='0' && *p1!='1' )	{err_data_flag = 1;	}
	else						{GEO_OnOff = *p1;	}								// GEO 开关

	p1+=2; 																			// 指向进出开关
	if(*p1 ==0x30)				{GEO_InOut_Flag = 0;} 								// 进入报警
	else if (*p1==0x31)			{GEO_InOut_Flag = 1;} 								// 出去报警 
	else						{err_data_flag =1;	}

	if(GEO_OnOff == '1')															// 开启报警
	{ 
		p1+=2; 																		// 指向经纬度数据第一位
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

		GEO_c[row-1].lat1=dms_to_d(p1,1);											// 把度分秒格式的GPS数据转成度格式									 
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
			GEOA.w|=1<<(row-1);														// 各组GEO开关
			GEOD.w|=1<<(row-1);														// 有效	 
			if(GEO_InOut_Flag)														// 走出报警
			{
				GEOB.w|=1<<(row-1);													// 各组GEO出去标志
				GEOC.w&=~(1<<(row-1));												// 各组GEO进入标志	
			}
			else																	// 进入报警
			{
				GEOB.w&=~(1<<(row-1));												// 各组GEO出去标志
				GEOC.w|=1<<(row-1);													// 各组GEO进入标志
			}
		}									
		else
		{	
			Task_SPI_Flash(Flash_Write,(row-1)*40+setgeoaddr,(u8*)&pbuf1,setgeo_size);
			GEOA.w&=~(1<<(row-1));													// 各组GEO开关 
			GEOD.w&=~(1<<(row-1));													// 无效		
			GEOB.w&=~(1<<(row-1));													// 各组GEO出去标志
			GEOC.w&=~(1<<(row-1));													// 各组GEO进入标志											
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

/* 设置超速报警 */
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

/* 设置BMA250，关机 */
void CMD_K_Handle(char *StrK,unsigned char ReplyK)
{
	bool Err = true;

	if(StrK[0] == 'H')															// 设置BMA碰撞阀值KHxxx	
	{
		if(Isdigit(StrK+1,strlen(StrK+1)) == true)	
		{
			GSM_P.higg=Char_To_Dig(StrK+1,strlen(StrK+1));
			if(GSM_P.higg > 255){GSM_P.higg=255;}
			Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);
			if(GSM_P.higg == 0)													// 关闭HIG	
			{
				send_info(" OK!","Disabled High-g OK!",CMD_K2,ReplyK|OLDGPS,MSG_NO,Do_Save);
			}		
			else																// 开启HIG				
			{
				send_info(" OK!","SET High-g Threshold OK!",CMD_K0,ReplyK|OLDGPS,MSG_NO,Do_Save);	
			}
			
			Err = false;
			
			BMA250_init();
		}
	}
	else if(StrK[0] == 'L')														// 设置BMA跌落值KLxxx
	{
		if(Isdigit(StrK+1,strlen(StrK+1)) == true)
		{
			GSM_P.lowg=Char_To_Dig(StrK+1,strlen(StrK+1));
			if(GSM_P.lowg>255) {GSM_P.lowg=255;}
			Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);
			if(GSM_P.lowg == 0)													// 关闭LOW
			{
				send_info(" OK!","Disabled Low-g OK!",CMD_K3,ReplyK|OLDGPS,MSG_NO,Do_Save);								
			}		
			else																// 开启LOW				
			{
				send_info(" OK!","SET Low-g Threshold OK!",CMD_K1,ReplyK|OLDGPS,MSG_NO,Do_Save);								
			}
			
			Err = false;
			
			BMA250_init();
		}
	}
	else if(StrK[0] =='\0')														// 软件关机
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


/* 设置时区, 设置RTC时间, 设置*/
void CMD_L_Handle(char *StrL,unsigned char ReplyL)
{
	bool Err=true;
	unsigned char i=0;
	
	if(((StrL[0] == '+') || (StrL[0] == '-')) && isdigit(StrL[1]) && isdigit(StrL[2])) 	// 格式为 +08	或	-08								
	{
		i = (StrL[1] & 0x0F) * 10 + (StrL[2] & 0x0F);
		
		if(i < 24)										// 时区未超出24小时
		{
			if(StrL[3]==':')							// 带分
			{				
				if(isdigit(StrL[4]) && isdigit(StrL[5]))// 格式字数字
				{
					unsigned char Min=0;
					
					Min = (StrL[4] & 0x0F) * 10 + (StrL[5] & 0x0F);
					
					switch(Min)							// 设置时区分
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
	else if((StrL[0] == '1')&&(StrL[1] == ',')&&(StrL[8] == ','))					// 格式为: 1,170101,120101		设置日期时间		
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
				
				if(day_tmp > 30)					// 日为31号
				{
					if((month_tmp == 2) || (month_tmp == 4) || (month_tmp == 6) || (month_tmp == 9) || (month_tmp == 11))
					{
						Err = true;
					}
				}
				else if(day_tmp > 29)			 	// 日为30号
				{
					if(month_tmp == 2) 					{Err = true;}
				}
				else if(day_tmp > 28)				// 日为29号,如果是平年则报错
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
	else if(StrL[0] == '4')																		// 格式为4,开启RTC自动更新一次
	{
		Err = false;
		GSMB.b.RTC_Update_Flag=false;
		send_info(" OK!","SET to Get GPS New Time OK!",CMD_L4,ReplyL|OLDGPS,MSG_NO,Do_Save);
	}
#if RTC_Alarm
	else if((StrL[0] == '5') && (StrL[1] == ','))												// 格式为5,xx:xx
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

/* 设置连续定位及心跳时间*/
void CMD_M_Handle(char *StrM,unsigned char ReplyM)
{
	bool Err = true;
	unsigned int i;
	
	if((StrM[0] == '4') && (StrM[1] == ','))					// 设置GPS输出模式 格式M4,1 M4,0
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
	else if((StrM[0] == '2') && (StrM[1] == ','))				// 设置心跳间隔时间 格式: M2,120, 设置范围30 - 600
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

/* 设置低电报警 */
void CMD_N_Handle(char *StrN,unsigned char ReplyN)		// 格式 N1,40		或	N0,40
{
	bool Err = true;

	if(((StrN[0] == '0') || (StrN[0] == '1'))&& (StrN[1] == ','))								// 低电报警设置
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
	else if((StrN[0] == '2') && (StrN[1] == ',') && ((StrN[2] == '1') || (StrN[2] == '0')))		// 充满电报警设置
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

/* 设置APN */
void CMD_O_Handle(char *StrO,unsigned char ReplyO)		// SETO, 或  SETOuser,password
{
	char *p1;

	p1 = strchr(StrO,',');		// 查找','之后的字符
		
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

/* 设置圆型GEO */
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

/* 设置SOS是否打电话 */
void CMD_Q_Handle(char *StrQ,unsigned char ReplyQ)
{	
	if(((StrQ[0] >= '1') && (StrQ[0] <= '5')) && (StrQ[1] == ',') && ((StrQ[2] == '0') || (StrQ[2] == '1')))
	{
		switch(StrQ[0])
		{
			case '1':						// 1,1		或1,0		设置SOS是否打出电话
				if(StrQ[2] == '1')	{GSMA.b.SOS_CALL_ENABLE_Flag = 1;send_info(" OK!","Open SOS Call Out OK!",CMD_Q1,ReplyQ|OLDGPS,MSG_NO,Do_Save);		}
				else				{GSMA.b.SOS_CALL_ENABLE_Flag = 0;send_info(" OK!","Close SOS Call Out OK!",CMD_Q1,ReplyQ|OLDGPS,MSG_NO,Do_Save);	}
				break;
				
			case '2':						// 2,1		或2,0		设置是否需要验证授权号码
				if(StrQ[2] == '1')	{GSMA.b.Any_CallIn_Flag = 1;send_info(" OK!","Set answer any number!",CMD_Q2,ReplyQ|OLDGPS,MSG_NO,Do_Save);			}
				else				{GSMA.b.Any_CallIn_Flag = 0;send_info(" OK!","Set answer authorized number!",CMD_Q2,ReplyQ|OLDGPS,MSG_NO,Do_Save);	}
				break;
				
			case '3':						// 3,1		或3,0		设置是否可以主动接听或挂断来电
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

/*设置域名*/
void CMD_R_Handle(char *StrR,unsigned char ReplyR)
{
	bool Err = true;
		
	if(StrR[0]==';')											// 删除域名指令
	{
		Err = false;
		GSM_P.domain_name[0]=0;
		GSM_P.domain_port[0]=0;									
	}
	else														// 增加域名指令
	{
		char *p1,*p2;	
		
		p1=StrR;
		p2=strchr(StrR,';');									// ;
	
		if(p2 != NULL)											// 有;符号
		{
			*p2 = 0; p2++;										// 指向端口号
			
			if((memcmp(p1,"http://",7)==0)&&(strlen(p1)>7))
			{
				snprintf(GSM_P.domain_name,24,"%s",p1+7);		// 最多保存24个字符,不保存http://
				
				if(p2[0] == '\0')								// 无端口号
				{
					Err = false;
					GSM_P.domain_port[0]=0;	
				}
				else
				{
					if((strlen(p2) < 6) && Isdigit(p2,strlen(p2)))				// 端口号字符长度小于6
					{						
						if(atoi(p2) < 65535)
						{
							Err = false;
							snprintf(GSM_P.domain_port,6,"%s",p2);	// 保存端口号
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

/*	设置开启或关闭串口调试	*/
void CMD_S_Handle(char *StrS,unsigned char ReplyS)
{
	;
}

/*设置模式*/
void CMD_U_Handle(char *StrU,unsigned char ReplyU)
{
	bool Err = true;
	
	#if (OTA_F==1)													// OTA升级 示例：123456UV（无版本号）\123456UV0101为版本号	 
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
			if(ReplyU == RPLY_TO_PC)								// 串口升级不检测电量
			{
				GSM_OTA.OTA_state=0x02;								// 准备串口升级
			}
			else if(ReplyU == RPLY_TO_SMS)							// 升级成功后回复短信	
			{
				GSM_OTA.OTA_state=0x04;
				
				GSM_OTA.OTA_ph = Compare_phone_number(SMS_Phone_Number) - 1;
			}
			else													// 升级成功后回复GPRS
			{
				GSM_OTA.OTA_state=0x08;								// 准备OTA升级
			}
			
			GSM_OTA.OTA_errcount=0;									// 清零失败次数							
			GSM_OTA.mcu_to_spi_bk=0;								// 需要重新备份
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
			case '0'://打进电话是通话
				GSMB.b.Callin_SpkMode_Flag=1;
				send_info(" OK!","Talking ok!",CMD_U0,ReplyU|OLDGPS,MSG_NO,Do_Save);
				break;

			case '1'://打进电话是监听
				GSMB.b.Callin_SpkMode_Flag=0;
				send_info(" OK!","Listen in OK!",CMD_U1,ReplyU|OLDGPS,MSG_NO,Do_Save);
				break;
			#endif
			
			#if CALLOUT_ENABLE
			case '2'://打出电话是静音
				GSMB.b.Callout_LisMode_Flag=1;
				send_info(" OK!","Calling in silence from SOS OK!",CMD_U2,ReplyU|OLDGPS,MSG_NO,Do_Save);
				break;

			case '3'://打出电话是通话
				GSMB.b.Callout_LisMode_Flag=0;
				send_info(" OK!","Calling with voice from SOS OK!",CMD_U3,ReplyU|OLDGPS,MSG_NO,Do_Save);
				break;
			#endif

			case '4'://打开GPS
				GSMB.b.GPS_PowerOffMode_Flag=0;
				send_info(" OK!","Turn on GPS OK!",CMD_U4,ReplyU|OLDGPS,MSG_NO,Do_Save);
				break;
									
			case '5'://关闭GPS
				GSMB.b.GPS_PowerOffMode_Flag=1;
				GPS_Force_Off();
				send_info(" OK!","Turn off GPS OK!",CMD_U5,ReplyU|OLDGPS,MSG_NO,Do_Save);
				break;

			case '6'://GPRS节省流量模式
				GSMB.b.GPRS_SaveMode_Flag=1;
				Set_Track_Time();		// 重设连续定位
				send_info(" OK!","GPRS SLOW OK!",CMD_U6,ReplyU|OLDGPS,MSG_NO,Do_Save);
				break;

			case '7'://按设定时间间隔上传
				GSMB.b.GPRS_SaveMode_Flag=0;
				Set_Track_Time();		// 重设连续定位
				send_info(" OK!","GPRS NORMAL OK!",CMD_U7,ReplyU|OLDGPS,MSG_NO,Do_Save);
				break;

			case '8'://GPS工作在超强模式
				GSMB.b.GPS_PowerOffMode_Flag=0;
				GSMB.b.GPS_StrongMode_Flag=1;
				GPS_Power_On();													// 立即开启电源
				if(ECHO_F.b.GPS_MCU_Flag){NVIC_EnableIRQ(USART2_IRQn);	}
				send_info(" OK!","GPS STRONG OK!",CMD_U8,ReplyU|OLDGPS,MSG_NO,Do_Save);
				break;

			case '9'://GPS工作在省电模式
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

/* 指令打出电话 */
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
				send_info(" OK!",Buf,CMD_V3,ReplyV|OLDGPS,MSG_NO,No_Save);// 注意比较发送字符.
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
		send_info(" OK!",Vol,CMD_V,ReplyV|OLDGPS,MSG_NO,No_Save);// 注意比较发送字符.
	}
	
	if(Err == true)
	{
		send_info(" ERROR!","ERROR!",CMD_ERR,ReplyV|OLDGPS,MSG_NO,No_Save);
	}			
}




/* 	TIM-保存WIFI:ssid,key */
void CMD_W_Handle(char *StrW,unsigned char ReplyW)
{
	char * str_wifi;
	//指令格式：SETW,ssid,key
	if(StrW[0] == 'W')
	{
		str_wifi = StrW+2;
		strcpy(WIFI.wifi_ssid,strtok(str_wifi, ","));
		strcpy(WIFI.wifi_key,strtok(NULL, ","));
	}
	
}

/* 设置LOG */
void CMD_Z_Handle(char *StrZ,unsigned char ReplyZ)
{
	if((StrZ[0] <= '2') && (StrZ[1] == ','))
	{
		snprintf(GSM_OTA.Device,15,"%s",StrZ+2);send_info(" OK!","Device name OK!",CMD_Z2,ReplyZ|OLDGPS,MSG_NO,Do_Save);	// 2,xxxx,最多设置25个字符
				
		Task_SPI_Flash(Flash_Write,otaaddr,(u8*)&GSM_OTA,GSM_OTA_size);
	}
	else
	{
		send_info(" ERROR!","ERROR!",CMD_ERR,ReplyZ|OLDGPS,MSG_NO,No_Save);
	}	
}

