#include "includes.h"

/* OLED		 */
void App_Task3(void const * argument)
{
	vTaskDelay(500);
	
	GPS_Mode_Setting();	
	
	if(GSMB.b.GPS_PowerOffMode_Flag == 0)									// 如果允许开启GPS电源
	{
		SOS_BMA_F_GPS_Start(6);
	}
#if Wifi_Enable	
	Wifi_Check_mac(1);
#endif
	while(1)
	{
		if(Semap_Gps != NULL)
		{
			if(osSemaphoreWait(Semap_Gps,portMAX_DELAY) == osOK)
			{	
				if((GPS_F.b.CMDF_Cnt_Flag || GPS_F.b.Track_For_sms_Flag || GPS_F.b.Track_For_gprs_flag) && (New_GPS_Handle() == true))
				{
					GPS_F.b.CMDF_Cnt_Flag = false;
					
					GPS_F.b.CMDF_Ten_Track_Flag = false;
					
					GPS_F.b.CMDF_Powron_5min_Flag = false;
					
					if(GPS_F.b.CMDF_Sendto_SMS_Flag)
					{
						GPS_F.b.CMDF_Sendto_SMS_Flag = false;
						
						Task_AT_GSM(Gsm_Send_SMS,SMS_Phone_Number_Backup,"GPS OK!",0,CMDFGPS,MSG_GOOGLE,0);
					}
					
					if(GPS_F.b.CMDF_Sendto_Gprs_Flag)
					{
						GPS_F.b.CMDF_Sendto_Gprs_Flag = false;
						
						Task_AT_GSM(Gprs_Send_Msg,0,0,CMD_F,CMDFGPS,MSG_GPRS,Do_Save);					
					}
					
					if(GPS_F.b.Track_For_sms_Flag || GPS_F.b.Track_For_gprs_flag)
					{
						if(!CALL.b.Calling_Flag && !CALL.b.Ring_Flag && (uxQueueSpacesAvailable(Queue_GSM) > (Queue_GSM_Num/2)))
						{
							if(GPS_F.b.Track_For_sms_Flag)
							{
								Task_AT_GSM(Gsm_Send_SMS,GSM_ph.temp_ph1,"AUTO-",0,CMDFGPS,MSG_GOOGLE,0);
							}
							
							if(GPS_F.b.Track_For_gprs_flag)
							{
								Task_AT_GSM(Gprs_Send_Msg,0,0,CMD_T,CMDFGPS,MSG_GPRS,Do_Save);			
							}
						}
						else if(GPS_F.b.Track_For_gprs_flag)
						{
							memcpy(&gpsx_his_write,&gpsx,gpsx_All_Size);gpsx_his_write.cmd_code = CMD_T;
																	
							Task_SPI_Flash(Flash_Write_History,0,0,0);	
						}
												
						GPS_F.b.Track_For_sms_Flag = false;GPS_F.b.Track_For_gprs_flag = false;GPS_F.b.Poweron_For_Track_Flag = false;
					}
										
					if(GPS_F.b.CMDF_Sendto_SOS_BMA_Flag)
					{				
#if Wifi_Enable						
						if(GSMA.b.GPRS_flag){Wifi_Check_mac(1);	}
#endif						
						SOS_BMA_Send_AlmMsg_And_Callout(ALMGPS);	

						GPS_F.b.CMDF_Sendto_SOS_BMA_Flag = false;						
					}
										
					GPS_Alarm_Handle();						
				}
				else
				{
					if(GPS_F.b.Track_For_sms_Flag || GPS_F.b.Track_For_gprs_flag)
					{						
						if(GPS_F.b.Track_For_gprs_flag)
						{
							if(Wifi_Check_mac(0))
							{
								Task_AT_GSM(Gprs_Send_Msg,0,0,CMD_T,OLDGPS,MSG_GPRS,No_Save);
							}									
						}	

						GPS_F.b.Track_For_sms_Flag = false;GPS_F.b.Track_For_gprs_flag = false;GPS_F.b.Poweron_For_Track_Flag = false;
					}
															
					if(GPS_F.b.CMDF_Cnt_Flag)
					{
						if(cnt_1s.CMD_F_Count >= 30)
						{
							if(GPS_F.b.CMDF_Ten_Track_Flag)
							{
								GPS_F.b.CMDF_Ten_Track_Flag = false;
								
								if(++InValid_GPS_Count > 3)
								{
									InValid_GPS_Count = 1; 

									if(GSMA.b.GPRS_flag)
									{	
										Task_AT_GSM(Gprs_Send_Msg,0,0,ALM_E,NOGPS,MSG_NO,No_Save);	
									}			
								}
							}
							
							if(GPS_F.b.CMDF_Sendto_SOS_BMA_Flag)
							{
								if(GSMA.b.GPRS_flag){Wifi_Check_mac(1);	}
														
								if(GPS_F.b.CMDF_Sendto_SOS_BMA_Flag)
								{	
									if(gps_a_count > 0)
									{
										SOS_BMA_Send_AlmMsg_And_Callout(ALMGPS);
									}
									else
									{
										SOS_BMA_Send_AlmMsg_And_Callout(OLDGPS);
									}
									
									GPS_F.b.CMDF_Sendto_SOS_BMA_Flag = false;
								}									
							}
						}
						
						if(cnt_1s.CMD_F_Count>=60)						
						{
							if(GPS_F.b.CMDF_Sendto_Gprs_Flag)
							{
								Wifi_Check_mac(1);	
							
								GPS_F.b.CMDF_Sendto_Gprs_Flag = false;
									
								Task_AT_GSM(Gprs_Send_Msg,0,0,CMD_F,OLDGPS,MSG_GPRS,Do_Save);								
							}
						}
						
						if(cnt_1s.CMD_F_Count>=120)						
						{
							if(GPS_F.b.CMDF_Sendto_SMS_Flag)
							{
								GPS_F.b.CMDF_Sendto_SMS_Flag = false;

								Task_AT_GSM(Gsm_Send_SMS,SMS_Phone_Number_Backup,"GPS NG!",0,OLDGPS,MSG_GOOGLE,0);							
							}
						}
						
						if(cnt_1s.CMD_F_Count>=300)											
						{		
							GPS_F.b.CMDF_Powron_5min_Flag = false;
						}
						
						if((!GPS_F.b.CMDF_Ten_Track_Flag) && (!GPS_F.b.CMDF_Sendto_Gprs_Flag) && (!GPS_F.b.CMDF_Sendto_SOS_BMA_Flag) && (!GPS_F.b.CMDF_Sendto_SMS_Flag) && (!GPS_F.b.CMDF_Powron_5min_Flag))
						{
							GPS_F.b.CMDF_Cnt_Flag = false;
						}	
					}									
				}
				
				GPS_Power_Off();		
			}
		}
		else
		{
			vTaskDelay(1000); 
		}
	}
}
