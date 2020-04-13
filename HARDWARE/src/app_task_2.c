#include "includes.h"

/* GSM		 */
void App_Task2(void const * argument)							
{
	type_gsm 	GSM_Task2;
		
	MCU_F.b.GSM_Init_Flag = true;
	
	GSM_Wakeup();vTaskDelay(500);
		
	if(!__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))			
	{								
		ZDMD_ON();SOUND_BI(BIBI);vTaskDelay(1000);
		
		GSMB.b.RTC_Update_Flag = false;												
			
		RTC_Write_Year_Time(gpsx.utc.year,gpsx.utc.month,gpsx.utc.date,gpsx.utc.hour,gpsx.utc.min,gpsx.utc.sec);			// 初始化设置RTC时间		
	}
		
#if RTC_Alarm
	
	if(GSMC.b.RTC_Alarm_Flag == true)
	{
		RTC_Write_Alarm(GSM_P.RTC_Alarm_Hour,GSM_P.RTC_Alarm_Min);
	}
		
#endif
			
	ATE0_Test();

	if(MCU_F.b.No_Simcard_Flag == false)
	{	
		GSM_init();		
	}
	
	if(MCU_F.b.No_Simcard_Flag == false)
	{	
	
#if SMS_ENABLE && !DEBUG	
		
		New_SMS_Handle();
	
#endif	

#if GPRS_ENABLE

#if !DEBUG		
		if(GSMB.b.RTC_Update_Flag == false)												// 如果UTC未更新RTC时间
		{
			GPRS_Connect(2);
		}
#endif
		
		Gsm_Get_Parameters(0x82);
		
		if(GSMA.b.GPRS_flag)														
		{			
			if(GPRS_Connect(0)==false)													// 如果首次重联失败,则启动重联计数器
			{
				cnt_1s.GPRS_Reconnect_Cnt=0;cnt_1s.GPRS_Reconnect_Set=60;
				GPRS.b.Reconnect_Cnt_Flag = true;										// 配置首次重联计数值,计时一分钟
			}
		}

#endif	
	}
	else
	{
		Gsm_Get_Parameters(0x80);
	}
	
	GSM_Sleep();
	
	MCU_F.b.GSM_Init_Flag = false;
	//开机检查是否需要OTA升级
	xEventGroupSetBits(EventGroupHandler_MCU,MCU_OTA_Flag);
	//开机检查是否有异常信息需要处理
	if(GSMD.b.CMDF_SOS_Flag)		{SOS_BMA_F_GPS_Start(3);}
	else if(GSMD.b.CMDF_BMAL_Flag)	{SOS_BMA_F_GPS_Start(4);}
	else if(GSMD.b.CMDF_BMAH_Flag)	{SOS_BMA_F_GPS_Start(5);}
	
	while(1)
	{
		if(Queue_GSM != NULL)
		{					
			if(xQueueReceive(Queue_GSM,(u8*)&GSM_Task2,portMAX_DELAY) != NULL)
			{												
				switch(GSM_Task2.TaskCmd)
				{
					case Gsm_CBC_CSQ:			// 读取CSQ
						if(MCU_F.b.No_Simcard_Flag)	{Gsm_Get_Parameters(0x82);}
						else						{Gsm_Get_Parameters(0x81);}
						break;
				
					case Gsm_Call_In:			// 处理来电
						if(GSM_Callin_Handle()){GSM_Calling_Handle();} break;
				
					case Gsm_SOS_BMA_Call_out:	// SOS BMA
						if(SOS_BMA_Handle()){GSM_Calling_Handle();} break;
						
					case Gsm_Call_Out:			// 拨打电话
						if(Callout_Phone(GSM_Task2.PhoneNum,3,!CALL.b.SOS_HangUp_En_Flag,CALL.b.SOS_HangUp_En_Flag,0) == 1)
						{
							GSM_Calling_Handle();
						}
						else
						{
							Gsm_Calling_Clr_Flag();
						}
						break;

					case Gsm_SMS_Full:
						
						cmgd_p(true);break;
											
					case Gsm_New_SMS:			// 短信处理
						
						New_SMS_Handle();break;
					
					case Gsm_Send_SMS:			// 发送短信
						
						Send_Msg_To_Phone(GSM_Task2.PhoneNum,GSM_Task2.PhoneMsg,GSM_Task2.GpsType,GSM_Task2.MsgType);	break;	
				
					case Gprs_New_Msg:			// 平台数据处理
						//wifi数据获取
						GPRS_Data_Handle();break;

					case Gprs_Send_Msg:			// 发送平台数据
						//wifi数据发送
						Send_Msg_To_Platform(GSM_Task2.GprsMsg,GSM_Task2.GpsType,GSM_Task2.MsgType,GSM_Task2.Save);	break;
										
					case Gprs_Send_Msg_H:		// 发送历史数据
						Send_Msg_To_Platform(GSM_Task2.GprsMsg,HISGPS,MSG_GPRS,No_Save);
						break;
						//wifi重连
					case Gprs_ReConnect_Now:	// 重联	
						
						GPRS.b.Connected_Now_Flag = true;	
					
						if(cnt_1s.GPRS_Reconnect_Cnt == 0)												
						{
							if(GPRS_Connect(0) == false)													
							{
								cnt_1s.GPRS_Reconnect_Set=60;
								GPRS.b.Reconnect_Cnt_Flag = true;		
							}
						}
						else
						{
							if(GPRS_Connect(1) == true)														
							{
								cnt_1s.GPRS_Reconnect_Cnt = 0;GPRS.b.Reconnect_Cnt_Flag = false;	
							}
							else if(GPRS.b.Reconnect_Cnt_Flag == false)
							{
								cnt_1s.GPRS_Reconnect_Set = 60;GPRS.b.Reconnect_Cnt_Flag = true;
							}
						}
						
						GPRS.b.Connected_Now_Flag = false;
						
						break;
						
					case Gprs_Disconnect:
						//断开连接
						GPRS_Disconnect();break;

					case Http_OTA:
						OTA();break;
						
					case Gsm_Pwr_off:	
						Task_SPI_Flash(Flash_Write,gpsadd,(u8*)&gpsx,gpsx_All_Size);					
						power_off();break;
						
					case Gsm_cfun_reset:
						
						vTaskDelay(1000);						
#if MODULE_UG96								
						GSM_AT_Send("+CFUN=4",6,2,2,0x0001);
						GSM_AT_Send("+CFUN=1",6,2,2,0x0001);		
#elif MODULE_SIM7500		
						GSM_AT_Send("+CFUN=6,0",6,2,2,0x0001);
#endif			
						Task_SPI_Flash(Flash_MCU_Reset,0,0,0);break;
				}
				
				GSM_Sleep();
			}			
		}
		else
		{
			vTaskDelay(1000);
		}
	}
}

//向队列头部插入数据
void Task_AT_GSM_Front(u8 GSM_cmd,char *Phone_Num,char *Phone_Msg,u8 GPRS_Msg,u8 Gps_Type,u8 Msg_Type,char GPRS_Save)
{
	type_gsm GSM_Send;
		
	GSM_Send.TaskCmd	= GSM_cmd;
	GSM_Send.PhoneNum	= Phone_Num;
	GSM_Send.PhoneMsg	= Phone_Msg;
	GSM_Send.GprsMsg 	= GPRS_Msg;
	GSM_Send.GpsType	= Gps_Type;
	GSM_Send.MsgType	= Msg_Type;
	GSM_Send.Save		= GPRS_Save;
	
	xQueueSendToFront(Queue_GSM, (u8*)&GSM_Send,10);
}

//向队列尾部插入数据
void Task_AT_GSM(u8 GSM_cmd,char *Phone_Num,char *Phone_Msg,u8 GPRS_Msg,u8 Gps_Type,u8 Msg_Type,char GPRS_Save)
{
	type_gsm GSM_Send;
		
	GSM_Send.TaskCmd	= GSM_cmd;
	GSM_Send.PhoneNum	= Phone_Num;
	GSM_Send.PhoneMsg	= Phone_Msg;
	GSM_Send.GprsMsg 	= GPRS_Msg;
	GSM_Send.GpsType	= Gps_Type;
	GSM_Send.MsgType	= Msg_Type;
	GSM_Send.Save		= GPRS_Save;
	
	xQueueSendToBack(Queue_GSM, (u8*)&GSM_Send,10);
}	

//Task_AT_GSM_Callback回调向队列头部插入数据
void Task_AT_GSM_Callback(u8 GSM_cmd)
{
	type_gsm GSM_Send_ISR;
	
	BaseType_t xHigherPriorityTaskWoken;
	
	GSM_Send_ISR.TaskCmd	= GSM_cmd;
	GSM_Send_ISR.PhoneNum	= 0;
	GSM_Send_ISR.PhoneMsg	= 0;
	GSM_Send_ISR.GprsMsg 	= 0;
	GSM_Send_ISR.GpsType	= 0;
	GSM_Send_ISR.MsgType	= 0;
	GSM_Send_ISR.Save		= 0;
	//Task_AT_GSM_Callback回调向队列头部插入数据					
	if(xQueueSendToFrontFromISR(Queue_GSM, (u8*)&GSM_Send_ISR,&xHigherPriorityTaskWoken) != pdFAIL)
	{
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}	

