#include "includes.h"

EventBits_t Event_Main;
	
/* MAIN 		*/
void App_Task5(void const * argument)
{	
#if WATCHDOG_ON
	
	MX_IWDG_Init();HAL_IWDG_Start(&hiwdg);									
												
#endif						
				
	while(1)
	{
		if(EventGroupHandler_MCU != NULL)
		{
			Event_Main = xEventGroupWaitBits(EventGroupHandler_MCU, EVENT_BIT_MAIN, pdFALSE,pdFALSE,portMAX_DELAY);
						
			if(Event_Main & MCU_OTA_Flag)									// ��������
			{
				xEventGroupClearBits(EventGroupHandler_MCU,MCU_OTA_Flag);			

				if(GSM_OTA.OTA_state)										// ��OTAִ������
				{				
					if((GSM_OTA.OTA_state & 0x81) == 0x80)					// �����ɹ�
					{								
						if(GSM_OTA.OTA_state & 0x02)						// ���ɴ���֪ͨ����
						{
							printf("OTA OK!\r\n");
						}
						else if(GSM_OTA.OTA_state & 0x04)					// ���ɶ���֪ͨ����
						{					
							Task_AT_GSM(Gsm_Send_SMS,GSM_ph.temp_ph1 + (GSM_OTA.OTA_ph * 24),"OTA OK!",0,OLDGPS,MSG_NO,0);								
						}
						else if(GSM_OTA.OTA_state & 0x08)					// ����GPRS֪ͨ����
						{					
							if(GSMA.b.GPRS_flag){Task_AT_GSM(Gprs_Send_Msg,0,0,ALM_U,OLDGPS,MSG_GPRS,1);									}
						}
						
						GSM_OTA.OTA_state = 0;								// ���OTA��־
						GSM_OTA.OTA_errcount = 0;

						Task_SPI_Flash(Flash_Write,otaaddr, (u8*)&GSM_OTA,GSM_OTA_size);
						
						if(GSM_OTA.mcu_to_spi_bk != 0xee)					// ���δ���ݳɹ�,����������
						{
							Task_SPI_Flash(Flash_MCU_Reset,0,0,0);
						}
					}
					else													// ��Ҫִ������
					{	
						if((USBinsert_chk() == SET)||(gpsx.soc >= 50))		// ��������ſ�������
						{							
							if(GSM_OTA.OTA_errcount >= 4)					// ���س����������3��,ֹͣ����
							{						
								if(GSM_OTA.OTA_state & 0x02)				// ���ɴ���֪ͨ����
								{
									printf("OTA Fail!\r\n");		
								}
								else if(GSM_OTA.OTA_state & 0x04)			// ���ɶ���֪ͨ����
								{							
									Task_AT_GSM(Gsm_Send_SMS,GSM_ph.temp_ph1 + (GSM_OTA.OTA_ph * 24),"OTA Fail!",0,NOGPS,MSG_NO,0);										
								}
								else										// ����GPRS֪ͨ����
								{						
									if(GSMA.b.GPRS_flag){Task_AT_GSM(Gprs_Send_Msg,0,0,ALM_V,OLDGPS,MSG_GPRS,1);									}
								}
								
								GSM_OTA.OTA_state = 0;
								GSM_OTA.OTA_errcount = 0;

								Task_SPI_Flash(Flash_Write,otaaddr, (u8*)&GSM_OTA,GSM_OTA_size);
							}
							else
							{
								if(GSMA.b.GPRS_flag)
								{
									GSMA.b.GPRS_flag=0;						 						  	
									Task_AT_GSM(Gprs_Disconnect,0,0,0,0,0,0);
								}
							
								GSMA.b.Track_Flag = false;
															
								if(GSM_OTA.OTA_errcount == 0)				// ��һ������ʱ,�ȱ���һ����ЧGPS
								{
									Task_SPI_Flash(Flash_Write,gpsadd,(u8*)&gpsx,gpsx_All_Size);	// ota����֮ǰ����һ��GPS����	
								}
								
								if((GSM_OTA.OTA_state  & 0xC0) == 0x00)		// δ���ػ�֮ǰ���س���
								{
									Task_SPI_Flash(Flash_Erase_Sector,0,0,0);
									
									vTaskDelay(1000);
									
									Readly_For_OTA(GSM_OTA.OTA_state & 0x0C);															
									
									if(GSM_OTA.OTA_state & 0x0C)			// ����OTA�ӷ��������س���
									{
										if(MCU_F.b.No_Simcard_Flag == false)
										{
											Task_AT_GSM(Http_OTA,0,0,0,0,0,0);
										}
									}
									else if(GSM_OTA.OTA_state & 0x02)		// ���봮�ڴӱ��ص������س���
									{
										USART_Update();					
									}
								}
								else										// �Է���������
								{
									Task_SPI_Flash(Flash_Reset_ALL,0,0,0);
								}
							}
						}
						else
						{
							MCU_F.b.USB_OTA_Flag = true;
						}
					}
				}		
			}
					
			if(Event_Main & (Key_SOS_L_Flag | Key_Call1_L_Flag | Key_Call2_L_Flag))			// ִ��SOS
			{
				xEventGroupClearBits(EventGroupHandler_MCU,Event_Key_All);
										
				if((CALL.b.Calling_Flag == false) && (!GSMD.b.CMDF_SOS_Flag) && (!GSMD.b.CMDF_BMAL_Flag) && (!GSMD.b.CMDF_BMAH_Flag))
				{								
					if(Event_Main & Key_Call1_L_Flag)
					{
						SOUND_BI(BIBI);	
						Calling_Set_Mode(1,1,0);
						Task_AT_GSM_Front(Gsm_Call_Out,GSM_ph.temp_ph4,0,0,0,0,0);	
					}
					else if(Event_Main & Key_Call2_L_Flag)
					{
						SOUND_BI(BIBI);	
						Calling_Set_Mode(1,1,0);
						Task_AT_GSM_Front(Gsm_Call_Out,GSM_ph.temp_ph5,0,0,0,0,0);
					}	
					else
					{
						cnt_1s.Track_Save_Cnt = 0;	
						if(GPRS.b.GPRS_Save_now_Flag){GPRS.b.GPRS_Save_now_Flag = false;Set_Track_Time();}
						if(!GSMB.b.Callout_LisMode_Flag){cnt_1s.BiBi_Cnt = 30;}
						SOS_BMA_F_GPS_Start(3);	
					}
				}		
			}
												
#if BMA250_ALARM																// ����BMA250�¼�
			
			if(Event_Main & BMA250_Int_Flag)
			{				
				unsigned char i=0;
				
				xEventGroupClearBits(EventGroupHandler_MCU,BMA250_Int_Flag);
								
				i = BMA250_IS_INT();
				
				BM250_GET_Slop();
				
				if((i == 0) && (cnt_1ms.BMA_Low_Cnt >= 10))
				{
					cnt_1ms.BMA_Low_Cnt = 0;
					
					if((CALL.b.Calling_Flag == false) && (!GPS_F.b.CMDF_Sendto_SOS_BMA_Flag) && ((abs(BMA250_Ang_x - BMA250_Ang_x_buf) >= 30) || (abs(BMA250_Ang_z - BMA250_Ang_z_buf) >= 30)))
					{	
						BMA250_Ang_x_buf = BMA250_Ang_x; BMA250_Ang_z_buf = BMA250_Ang_z;
#if !DEBUG						
						if(!GSMB.b.Callout_LisMode_Flag){cnt_1s.BiBi_Cnt = 30;}						
							
						SOS_BMA_F_GPS_Start(4);		
#else
						SOUND_BI(BIBI);	
#endif
					}
				}
				else
				{
					if(CALL.b.Calling_Flag == false)
					{					
						if(i & 0x01)		
						{
							BMA250_Ang_x_buf = BMA250_Ang_x; BMA250_Ang_z_buf = BMA250_Ang_z;
						
							MCU_F.b.BMA_Low_Flag = true; cnt_1ms.BMA_Low_Cnt = 0;
						}				
						else if((i & 0x02) && (!GPS_F.b.CMDF_Sendto_SOS_BMA_Flag))	
						{
#if !DEBUG
							if(!GSMB.b.Callout_LisMode_Flag){cnt_1s.BiBi_Cnt = 30;}
							
							SOS_BMA_F_GPS_Start(5);	
#else
							SOUND_BI(BIBI);
#endif
						}										
					}
					
					if((GSMB.b.GPRS_SaveMode_Flag) && (i & 0x07))
					{
						cnt_1s.Track_Save_Cnt = 0;
						if(GPRS.b.GPRS_Save_now_Flag)	
						{
							GPRS.b.GPRS_Save_now_Flag = false;Set_Track_Time();
						}	
					}
					
					i = 0;
					
					while((BMA250_INT_READ() == 0)&&(i++ < 20))
					{
						BMA250_Clear_INT();				
						vTaskDelay(100);
					}	
				}									
			}
			
#endif

#if RTC_Alarm
			
			if(Event_Main & MCU_RTC_Alarm_Flag)
			{
				xEventGroupClearBits(EventGroupHandler_MCU,MCU_RTC_Alarm_Flag);
				cnt_1s.BiBi_Cnt = 60;
				Task_SPI_Oled(Oled_Preogress,"Clock Alarm!","",0,0);
			}
			
#endif
			
			if(Event_Main & Bat_Charge_Full_Flag)								// ��Ҫ���ͳ����籨��
			{
				xEventGroupClearBits(EventGroupHandler_MCU,Bat_Charge_Full_Flag);
			
				if((USBinsert_chk() == SET)&&(!GSMB.b.ALM_Bat_Full_Send_Flag))	
				{
					if(gpsx.soc > 98)
					{
						gpsx.soc=Bat_Val[0]=Bat_Val[1]=Bat_Val[2]=100;			
						
						GSMB.b.ALM_Bat_Full_Send_Flag=true;
						
						Task_SPI_Flash(Flash_Write,GSM_paraddr,(u8*)&GSM_P,GSM_para_size);	
						Task_SPI_Flash(Flash_Write,1,&GSMB.w,1);
#if Bat_Full_ON			
						if(GSMB.b.ALM_Bat_Full_On_Flag)						
						{
							send_alarm(0x01,"BAT FULL!",ALM_F,NOGPS);
						}	
#endif
					}
				}
			}				
		}

	}
}


