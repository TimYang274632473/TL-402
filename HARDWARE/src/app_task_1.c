#include "includes.h"

/* SPI FLASH */
void App_Task1(void const * argument)
{	
	type_Flash SPI_Task1;
	
	Read_Flash_Para();
		
	BMA250_init();	
	
	if(USBinsert_chk() == SET)												
	{	
		MCU_F.b.Power_Connected_Flag = true;
		
		if(CHK_CH() == RESET){MCU_F.b.Charging_Flag = true;}
	}
	
	while(1)
	{			
		if(Queue_SPI != NULL)
		{						
			if(xQueueReceive(Queue_SPI,(u8*)&SPI_Task1,portMAX_DELAY) != NULL)
			{
				SPI_Flash_Wakeup();
				
				switch(SPI_Task1.Cmd)
				{
					case Flash_Read:						
						SPI_Flash_Read(SPI_Task1.W25_Address,SPI_Task1.W25_Data,SPI_Task1.W25_Length);break;

					case Flash_Write:
						SPI_Flash_Write(SPI_Task1.W25_Address,SPI_Task1.W25_Data,SPI_Task1.W25_Length);break;

					case Flash_Erase_Sector:
						for(u8 i=16;i<=79;i++){SPI_Flash_Erase_Sector(i);}break;		
					
					case Flash_Reset_ALL: 
						initialize_all(1);break;
					
					case Flash_Reset_WithoutOTA:
						initialize_all(0);break;
					
					case Flash_Read_ID:
						printf("W25 ID = %04X\r\n",SPI_Flash_ReadID());break;
					
					case Flash_SETG:
						{
							unsigned char hi;
							char StrG_tmp[40];
							
							printf("A%s",GSM_ph.temp_ph1);
							if(GSM_phname.name_ph1[0] != '\0')	{printf(",%s",GSM_phname.name_ph1);	}
							
							printf(";B%s",GSM_ph.temp_ph2);
							if(GSM_phname.name_ph2[0] != '\0')	{printf(",%s",GSM_phname.name_ph2);	}
						
							printf(";C%s",GSM_ph.temp_ph3);			
							if(GSM_phname.name_ph3[0] != '\0')	{printf(",%s",GSM_phname.name_ph3);	}
							
							printf(";D%s",GSM_ph.temp_ph4);
							if(GSM_phname.name_ph4[0] != '\0')	{printf(",%s",GSM_phname.name_ph4);	}
							
							printf(";E%s",GSM_ph.temp_ph5);
							if(GSM_phname.name_ph5[0] != '\0')	{printf(",%s",GSM_phname.name_ph5);	}
							
							printf(";F%s",GSM_ph.temp_ph6);
							if(GSM_phname.name_ph6[0] != '\0')	{printf(",%s",GSM_phname.name_ph6);	}
							
							printf(";G%s",GSM_ph.temp_ph7);
							if(GSM_phname.name_ph7[0] != '\0')	{printf(",%s",GSM_phname.name_ph7);	}
							
							printf(";H%s",GSM_ph.temp_ph8);
							if(GSM_phname.name_ph8[0] != '\0')	{printf(",%s",GSM_phname.name_ph8);	}
														
							printf(";%s;%s;%s;%s;%s;Speed:%d;%d;%d;%d;%d;%d;BAT:%d,%d%%\r\n",
							GSM_P.apn,GSM_P.APN_username,GSM_P.APN_password,GSM_P.ip_addr,GSM_P.ip_port,GSM_P.set_speed,
							GSMA.b.ALM_OverSpeed_Flag,GSMA.b.Track_Flag,(gpsx.AV_indicate=='A'? 1:0),GSMA.b.ALM_Circle_GEO_Flag,GSMA.b.ALM_LowBat_Flag ,gpsx.soc,GSM_P.set_soc&0x7F);
							
							printf("%d%d%d%d%d0000;",GSMB.b.GPRS_SaveMode_Flag,GSMA.b.SOS_CALL_ENABLE_Flag,GSMA.b.Any_CallIn_Flag,GSMA.b.CallIn_ByMe_Flag,GSMB.b.ALM_Bat_Full_On_Flag);											
							printf("Password:%s\r\n",GSM_P.cmd_key);																								
							printf("%02XH,%02XH,00H,%02XH,00H,%02XH,%02XH,",GSM_P.TimeZone,GSM_P.Track_Set_Time,GSM_P.geo_radius,(u8)(GSM_P.Track_Set_Unit),GSM_P.lowg);
						
							printf("%d;%d;%d;%d;\r\n",(GSMB.b.Callin_SpkMode_Flag? 0:1),(GSMB.b.Callout_LisMode_Flag? 0:1),GSMB.b.GPS_PowerOffMode_Flag,(GSMB.b.GPS_StrongMode_Flag? 0:1));
							printf("IMEI:%s%s\r\n",GSM_P.IMEI,HS_VER);	
														
							for(hi=0;hi<5;hi++)
							{
								memset(StrG_tmp,0,40);
								SPI_Flash_Read(setgeoaddr+40*hi,(u8*)StrG_tmp,setgeo_size);
								printf("%d:%s\r\n",hi,StrG_tmp);
							}
														
							printf("%02XH,%02XH,%02XH,\r\nHP:",GEOA.w|0xE0,GEOB.w|0xE0,GEOD.w|0xE0);	
							
							if(GSM_P.domain_name[0] == '\0')
							{
								printf(",%s",GSM_P.domain_port);
							}
							else
							{
								printf("http://%s,%s",GSM_P.domain_name,GSM_P.domain_port);
							}

							printf("\r\nLow-g:%d;High-g:%d;Disabled tap,HB:%d",GSM_P.lowg,GSM_P.higg,GSM_P.heartbeatset);
							
							if(strcmp(GSM_P.TCP_UDP,"UDP") == 0)
							{
								printf(",UDP");
							}
							
							printf("\r\n");
						}						
						break;
										
					case Flash_Write_History:				
						SPI_Flash_Write(hisdataaddr+(GSM_P.store_history)*50,(u8*)&gpsx_his_write,gpsx_All_Size);	// 保存数据	
					
						if((GSM_P.store_history++) > 150000)
						{
							GSM_P.send_history=0;GSM_P.store_history=0;			
						}	
						
						if((cnt_1s.Track_Time_Set > 5) || ((cnt_1s.Track_Time_Set <= 5) && (GSM_P.store_history%60 == 0)))
						{
							SPI_Flash_Write(GSM_paraddr,(u8*)&GSM_P,GSM_para_size);									// 保存历史数据条数	
						}
												
						break;
					
					case Flash_Read_History:		
						if(GSM_P.send_history < GSM_P.store_history)
						{
							GPRS.b.GPRS_send_history_Flag = true;
							SPI_Flash_Read(hisdataaddr+GSM_P.send_history*50,(u8*)&gpsx_his_read,gpsx_All_Size);	// 依次读出储存的数据，先进先出  
							Task_AT_GSM(Gprs_Send_Msg_H,0,0,gpsx_his_read.cmd_code,HISGPS,MSG_GPRS,No_Save);
						}
						break;
										
					case Flash_MCU_Reset:
						SPI_Flash_Write(gpsadd,(u8*)&gpsx,gpsx_All_Size);SPI_Flash_Sleep();
						NVIC_SystemReset();break;
					
					case Flash_MCU_Power_Off:
						SPI_Flash_Write(gpsadd,(u8*)&gpsx,gpsx_All_Size);SPI_Flash_Sleep();
						power_off();break;
				}
							
				SPI_Flash_Sleep();
			}			
		}
		else
		{
			vTaskDelay(1000);
		}			
	}	
}

void Task_SPI_Flash(u8 SPI_cmd,u32 SPI_Addr,u8* pBuffer,u16 Length)
{
	type_Flash SPI_Send;
	
	SPI_Send.Cmd			= SPI_cmd;
	SPI_Send.W25_Address	= SPI_Addr;
	SPI_Send.W25_Data		= pBuffer;
	SPI_Send.W25_Length		= Length;
	
	xQueueSend(Queue_SPI, (u8*)&SPI_Send,10);
}

void Task_SPI_Flash_Callback(u8 SPI_cmd)
{
	type_Flash SPI_Send_ISR;
	
	BaseType_t xHigherPriorityTaskWoken;
	
	SPI_Send_ISR.Cmd			= SPI_cmd;
	SPI_Send_ISR.W25_Address	= 0;
	SPI_Send_ISR.W25_Data		= 0;
	SPI_Send_ISR.W25_Length		= 0;
						
	if(xQueueSendToBackFromISR(Queue_SPI, (u8*)&SPI_Send_ISR,&xHigherPriorityTaskWoken) != pdFAIL)
	{
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}	
