#include "includes.h"
//添加wifi重连
void Led_callback(void)
{
	if(MCU_F.b.Power_Off_Flag)													// 准备关机
	{
		WORK_LED_R();GPS_LED_R();GSM_LED_R();GPRS_LED_R();return;
	}
		
	if(MCU_F.b.GSM_Init_Flag) 													// 初始化每秒闪一次
	{
		if(cnt_1ms.Led_gsm_cnt++ >= 20)
		{
			cnt_1ms.Led_gsm_cnt = 0;
		}
		
		if(cnt_1ms.Led_gsm_cnt == 1)		
		{			
			GSM_LED_On();	
		}
		else if(cnt_1ms.Led_gsm_cnt == 2)	
		{
			GSM_LED_Off();							
		}
		
		return;
	}
		
	if((GSM_OTA.OTA_state & 0x0E) && !MCU_F.b.USB_OTA_Flag)						// 正在OTA下载		
	{		
		if(cnt_1ms.Led_gsm_cnt)
		{
			cnt_1ms.Led_gsm_cnt = 0;
			
			GSM_LED_On(); GPS_LED_Off();
		}
		else
		{
			cnt_1ms.Led_gsm_cnt = 1;
			GSM_LED_Off();GPS_LED_On();
		}
		
		return;		
	}

	
	switch(cnt_1ms.Led_net_cnt++)												// 5秒,网络状态
	{
		case 1:  if(network >=2)	{GPRS_LED_On();	}break;
		case 8:  if(network >=2)	{GPRS_LED_On();	}break;
		case 15: if(network >=3)	{GPRS_LED_On();	}break;
		case 22: if(network ==4)	{GPRS_LED_On();	}break;
			
		case 4:	
		case 11:
		case 18:
		case 25: GPRS_LED_Off();break;
		
		case 100:cnt_1ms.Led_net_cnt = 0;break;
		
		default:break;
	}
			
	
	if(GPRS.b.Connected_Now_Flag)												// 正在重联,每两秒闪一次
	{
		if(cnt_1ms.Led_gsm_cnt++ >= 40)
		{
			cnt_1ms.Led_gsm_cnt = 0;
		}
		
		if(cnt_1ms.Led_gsm_cnt == 1)		
		{			
			GSM_LED_On();	
		}
		else if(cnt_1ms.Led_gsm_cnt == 2)	
		{
			GSM_LED_Off();							
		}

		return;	
	}
	
	if(cnt_1ms.Led_pwr_cnt++ >= 100)
	{
		cnt_1ms.Led_pwr_cnt = 0;
	}

	if(GSMD.b.CMDF_SOS_Flag || GSMD.b.CMDF_BMAL_Flag || GSMD.b.CMDF_BMAH_Flag)			// SOS报警,每4秒闪二次
	{
		switch(cnt_1ms.Led_pwr_cnt)
		{
			case 1:
			case 8: WORK_LED_On();break;
			
			case 4:
			case 11:WORK_LED_Off();break;
			
			case 80:cnt_1ms.Led_pwr_cnt = 0;break;
			
			default: break;
		}
	}
	else if(GSM_P.set_soc & 0x80)														// 低电,每5秒闪三次
	{
		switch(cnt_1ms.Led_pwr_cnt)
		{
			case 1:
			case 8: 
			case 15: WORK_LED_On();break;
			
			case 4:
			case 11:
			case 18: WORK_LED_Off();break;
			
			default: break;
		}	
	}
	else																				// 正常模式,每2秒闪一次
	{
		switch(cnt_1ms.Led_pwr_cnt)
		{
			case 1: WORK_LED_On();break;
			
			case 4: WORK_LED_Off();break;
			
			case 40:cnt_1ms.Led_pwr_cnt = 0;break;
			
			default: break;
		}	
	}
	
	if(key_num_buf)																		// 按键
	{
		if(key_num_buf == 1)							{GPRS_LED_R();				}
		else if(key_num_buf == 2)						{WORK_LED_R();				}
		else if(key_num_buf == 4)						{GPS_LED_R();				}
		else if(key_num_buf == 8)						{GSM_LED_R();				}
	}
	
	if(cnt_1ms.Led_gsm_cnt++ >= 100)
	{
		cnt_1ms.Led_gsm_cnt = 0; 
	}
	
	switch(cnt_1ms.Led_gsm_cnt)															// 正常模式
	{
		case 1: if(MCU_F.b.No_Simcard_Flag == false){GSM_LED_On(); }break;
		case 8: if((MCU_F.b.No_Simcard_Flag == false) && GPRS.b.Connected_OK_Flag && (gpsx.sig_strength > 15)) {GSM_LED_On();} break;
		
		case 4:
		case 11: GSM_LED_Off(); break;
		
		case 50: if(GPS_F.b.AV_Flag){GPS_LED_On();}break;
		case 52: GPS_LED_Off(); break;
		
		default: break;
	}
}

/* app_task_callback function */
void app_timer_50ms_callback(void const * argument)
{
	Led_callback();
	
	if(Tim_F.b.Chk_Key_Flag == true)											// 按键扫描
	{
		key_scan();
	}
	
	if(cnt_1ms.Motor_Off_Cnt != 0)
	{
		if(--cnt_1ms.Motor_Off_Cnt == 0)
		{
			motor_off();
		}
	}
	 	
	if(Tim_F.b.Chk_USB_Connect_Flag == true)									// 扫描USB连接
	{
		if(cnt_1ms.Chk_USB_Connect_Cnt++ >= 2)
		{
			Tim_F.b.Chk_USB_Connect_Flag = false;
			
			if(USBinsert_chk() == SET)										
			{
				MCU_F.b.Power_Connected_Flag = true;
				xEventGroupSetBits(EventGroupHandler_MCU,USB_Connected_Flag);	
				xEventGroupClearBits(EventGroupHandler_MCU,USB_Disconnected_Flag);

				if(MCU_F.b.USB_OTA_Flag == true)
				{
					MCU_F.b.USB_OTA_Flag = false;
					xEventGroupSetBits(EventGroupHandler_MCU,MCU_OTA_Flag);	
				}
			}		
		}
	}
	
	if(Tim_F.b.Chk_USB_Disconnect_Flag == true)									// 扫描USB断开
	{
		if(cnt_1ms.Chk_USB_Connect_Cnt++ >= 2)
		{	
			Tim_F.b.Chk_USB_Disconnect_Flag = false;
			
			if(USBinsert_chk() == RESET)									
			{
				MCU_F.b.Charging_Flag = false;
				MCU_F.b.Power_Connected_Flag = false;
				xEventGroupSetBits(EventGroupHandler_MCU,USB_Disconnected_Flag);
				xEventGroupClearBits(EventGroupHandler_MCU,Bat_Charging_Flag|USB_Connected_Flag);						
			}			
		}
	}
	
	if(Tim_F.b.Chk_Bat_Charge_Full_Flag)										// 外部中断检测到有充满电标志
	{
		if(cnt_1ms.Chk_Bat_Full_Cnt++ >= 2)								
		{			
			Tim_F.b.Chk_Bat_Charge_Full_Flag = false;
							
			if((CHK_CH() == SET)&&(USBinsert_chk()==SET))					
			{
				MCU_F.b.Charging_Flag = false;
				xEventGroupSetBits(EventGroupHandler_MCU,Bat_Charge_Full_Flag);
				xEventGroupClearBits(EventGroupHandler_MCU,Bat_Charging_Flag);	
			}
		}
	}		
		
	if(Tim_F.b.Chk_Bat_Charing_Flag)											// 外部中断检测到有充电中标志
	{
		if(cnt_1ms.Chk_Bat_Charge_Cnt++ >= 2)	
		{
			Tim_F.b.Chk_Bat_Charing_Flag = false;
			
			if((CHK_CH() == RESET)&&(USBinsert_chk()==SET))					
			{
				MCU_F.b.Charging_Flag = true;
				xEventGroupSetBits(EventGroupHandler_MCU,Bat_Charging_Flag);				
				xEventGroupClearBits(EventGroupHandler_MCU,Bat_Charge_Full_Flag);
			}			
		}
	}
	
	if(MCU_F.b.BMA_Low_Flag)													// 
	{
		if(++cnt_1ms.BMA_Low_Cnt >= 20)										
		{
			MCU_F.b.BMA_Low_Flag = false;
			xEventGroupSetBits(EventGroupHandler_MCU,BMA250_Int_Flag);	
		}
	}
		
	if(++cnt_1s._1S_Cnt >= 20)													// 一秒计时
	{
		cnt_1s._1S_Cnt = 0;

#if WATCHDOG_ON	
		
		if(cnt_1s.WTD_Cnt++ > 10)
		{
			cnt_1s.WTD_Cnt = 0;
			HAL_IWDG_Refresh(&hiwdg);											// 内部看门狗喂狗
		}
		
#endif
		
		if(GPRS.b.OTA_NOW_Flag || MCU_F.b.GSM_Init_Flag) {return;}
				
		if(cnt_1s.BiBi_Cnt != 0)												// 客户跌落后，每5秒报警一次
		{
			cnt_1s.BiBi_Cnt--;
			SOUND_BI(BIBI);
		}	
		
#if CALLIN_ENABLE
		
		if(CALL.b.Ring_Cnt_Flag)												// 来电响铃计时
		{
			if(cnt_1s.Ring_Time++>30)											// 如果30秒后,主程序未作任何处理,可能正在通话中，则清除来电标志,重新计数,对方继续等待接听
			{
				Ring_Clip_Count=0;
				cnt_1s.Ring_Time=0;
				CALL.b.Ring_Flag = false;
				CALL.b.Ring_Cnt_Flag = false;
				CALL.b.CallInPhone_Save_Flag = false;	
			}
		}

#endif
		
#if CALLOUT_ENABLE && MODULE_SIM7500
		
		if(CALL.b.Dtmf_Flag)
		{
			cnt_1s.Ring_Time++;
		}
#endif
				
		if(GPS_F.b.Gps_time_update_Flag == true)								// 定时更新GPS显示时间
		{
			if(cnt_1s.GPS_Time_Update_Cnt++ >= 65500)
			{
				GPS_F.b.Gps_time_update_Flag = false;
				cnt_1s.GPS_Time_Update_Cnt = 0;
			}
		}
								
		if((cnt_1s.SOC_CSQ_Cnt++ >= 250) || (MCU_F.b.Charging_Flag && (cnt_1s.SOC_CSQ_Cnt >= 60)))	// 如果计时超时250S,或在充电或连续定位时间小于1分钟,计时大于60秒
		{
			cnt_1s.SOC_CSQ_Cnt = 0;Task_AT_GSM(Gsm_CBC_CSQ,0,0,0,0,0,0);
		}
		
	
#if GEO_FUN1	
		
		if(GEOA.b.GEO1_flag)
		{
			if(cnt_1s.Chk_GEO1_Cnt++ > 600)
			{
				cnt_1s.Chk_GEO1_Cnt = 0;GPS_F.b.Send_Geo1_ALM_flag = true;
			}
		}
		if(GEOA.b.GEO2_flag)
		{
			if(cnt_1s.Chk_GEO2_Cnt++ > 600)
			{
				cnt_1s.Chk_GEO2_Cnt = 0;GPS_F.b.Send_Geo2_ALM_flag = true;
			}
		}
		if(GEOA.b.GEO3_flag)
		{
			if(cnt_1s.Chk_GEO3_Cnt++ > 600)
			{
				cnt_1s.Chk_GEO3_Cnt = 0;GPS_F.b.Send_Geo3_ALM_flag = true;
			}
		}
		if(GEOA.b.GEO4_flag)
		{
			if(cnt_1s.Chk_GEO4_Cnt++ > 600)
			{
				cnt_1s.Chk_GEO4_Cnt = 0;GPS_F.b.Send_Geo4_ALM_flag = true;
			}
		}
		if(GEOA.b.GEO5_flag)
		{
			if(cnt_1s.Chk_GEO5_Cnt++ > 600)
			{
				cnt_1s.Chk_GEO5_Cnt = 0;GPS_F.b.Send_Geo5_ALM_flag = true;
			}
		}
		
#endif	

#if GEO_FUN2
		
		if(GSMA.b.ALM_Circle_GEO_Flag)
		{
			if(cnt_1s.Chk_GEO6_Cnt++ > 600)
			{
				cnt_1s.Chk_GEO6_Cnt = 0;
				GPS_F.b.Send_Geo6_ALM_flag = true;
			}
		}
		
#endif
		
#if SMS_ENABLE && !DEBUG
										
		if(cnt_1s.Chk_SMS_Ten_Cnt++ > 600)									
		{						
			cnt_1s.Chk_SMS_Ten_Cnt = 0;														// 清除计数
			Task_AT_GSM(Gsm_New_SMS,0,0,0,0,0,0);										// 任务中执行读取短信
		}
		
#endif	

		if(cnt_1s.Chk_GPS_Ten_Cnt++ > 600)
		{
			cnt_1s.Chk_GPS_Ten_Cnt = 0;SOS_BMA_F_GPS_Start(7);
		}
		
		if(GPS_F.b.CMDF_Cnt_Flag)													// 处理单次定位计时
		{			
			if((cnt_1s.CMD_F_Count++ % 10) == 0)
			{
				xSemaphoreGive(Semap_Gps);
			}
		}
		
		if(GSMA.b.Track_Flag)
		{
			if(cnt_1s.Track_SOC_CSQ_Cnt < 200) {cnt_1s.Track_SOC_CSQ_Cnt++;}
	
#if Wifi_Enable
			
			if(GPRS.b.Track_GPRS_Flag)	
			{
				if((gpsx.AV_indicate != 'A') && (cnt_1s.Track_wifi_Cnt < 254)){cnt_1s.Track_wifi_Cnt++;	}
			}
			
			

#endif
			
			if(cnt_1s.Track_Time_Set > 20)
			{
				if((cnt_1s.Track_Time_Set - cnt_1s.Track_Time_Cnt) == (Track_Time_Unit * open_gps_count))
				{
					GPS_F.b.Poweron_For_Track_Flag = true;GPS_Power_On();
				}
			}
						
			if(++cnt_1s.Track_Time_Cnt >= cnt_1s.Track_Time_Set)
			{	
				cnt_1s.Track_Time_Cnt = 0;	
				
				GPS_F.b.Track_For_sms_Flag = SMS.b.Track_SMS_Flag;
				
				GPS_F.b.Track_For_gprs_flag= GPRS.b.Track_GPRS_Flag;
				
				xSemaphoreGive(Semap_Gps);	
			}
							
			if(GSMB.b.GPRS_SaveMode_Flag)												// 已开启自动省电模式
			{
				if(GPRS.b.GPRS_Save_now_Flag)
				{
					if(++cnt_1s.Track_Save_Cnt >= cnt_1s.Track_Time_Set)
					{
						cnt_1s.Track_Save_Cnt = 0;
						cnt_1s.Track_Time_Set = 2 * cnt_1s.Track_Time_Set;
						
						if(cnt_1s.Track_Time_Set > 3600){cnt_1s.Track_Time_Set = 3600;}
					}	
				}
				else
				{
					if(++cnt_1s.Track_Save_Cnt >= 180)
					{
						cnt_1s.Track_Save_Cnt = 0;
						
						if(cnt_1s.Track_Time_Set < 3600)
						{
							GPRS.b.GPRS_Save_now_Flag = true;cnt_1s.Track_Time_Cnt = 0;
							
							if(cnt_1s.Track_Time_Set < 300)
							{
								cnt_1s.Track_Time_Set = 300;
							}
							
							if(cnt_1s.Track_Time_Set <= 600)															
							{
								Track_Time_Unit = 5;
							}
							else															
							{
								Track_Time_Unit = 15;
							}
						}
					}
				}
			}			
		}
		
#if GPRS_ENABLE																	
	
		if(GSMA.b.GPRS_flag)
		{
			if(GPRS.b.Connected_OK_Flag)
			{
				if(++cnt_1s.Chk_GPRS_Cnt >= 180)
				{
					cnt_1s.Chk_GPRS_Cnt = 0;
					
					if(!CALL.b.Calling_Flag) 
					{
						Task_AT_GSM(Gprs_New_Msg,0,0,0,0,0,0);
					}
				}
				
				if(++cnt_1s.HeartBeat_Cnt >= GSM_P.heartbeatset)
				{
					cnt_1s.HeartBeat_Cnt=0;
					
					if((!GSMA.b.Track_Flag) || ((cnt_1s.Track_Time_Set-cnt_1s.Track_Time_Cnt) >= 30))
					{
						Task_AT_GSM(Gprs_Send_Msg,0,0,CMD_X,NOGPS,MSG_NO,No_Save);
					}	
				}	
			}
				
			if(GPRS.b.Reconnect_Cnt_Flag)													// 如果启动了GPRS重联计数器
			{
				if(++cnt_1s.GPRS_Reconnect_Cnt >= cnt_1s.GPRS_Reconnect_Set)				// 如果已到时
				{						
					cnt_1s.GPRS_Reconnect_Set = cnt_1s.GPRS_Reconnect_Set * 2;

					if(cnt_1s.GPRS_Reconnect_Set < 15360)									
					{
						if((!CALL.b.Ring_Flag) && (!CALL.b.Calling_Flag))
						{
							
							Task_AT_GSM(Gprs_ReConnect_Now,0,0,0,0,0,0);					// 启动主程序重联处理程序
						}
					}
					else																		
					{
						Task_SPI_Flash(Flash_MCU_Reset,0,0,0);
					}				
				}
			}
		}		
#endif	
		
	}
}

