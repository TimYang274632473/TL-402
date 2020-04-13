#ifndef __APP_TASK_2_H
#define __APP_TASK_2_H

#include "includes.h"

#define Gsm_CBC_CSQ				0x01
#define Gsm_Call_In				0x02
#define Gsm_SOS_BMA_Call_out	0x03
#define Gsm_Call_Out			0x04
#define Gsm_SMS_Full			0x05
#define Gsm_New_SMS				0x06
#define Gsm_Send_SMS			0x07
#define Gprs_New_Msg			0x08
#define Gprs_Send_Msg			0x09
#define Gprs_Send_Msg_H			0x0A
#define Gprs_ReConnect_Now		0x0B
#define Gprs_Disconnect			0x0C 
#define Http_OTA				0x0D
#define Gsm_Pwr_off				0x0E
#define Gsm_cfun_reset			0x0F

typedef struct 
{
	char 	TaskCmd;
	char*	PhoneNum;
	char*	PhoneMsg;
	u8		GprsMsg;
	u8		GpsType;
	u8		MsgType;
	u8	 	Save;
}type_gsm;


extern void App_Task2(void const * argument);
extern void Task_AT_GSM_Front(u8 GSM_cmd,char *Phone_Num,char *Phone_Msg,u8 GPRS_Msg,u8 Gps_Type,u8 Msg_Type,char GPRS_Save);
extern void Task_AT_GSM(u8 GSM_cmd,char *Phone_Num,char *Phone_Msg,u8 GPRS_Msg,u8 Gps_Type,u8 Msg_Type,char GPRS_Save);
extern void Task_AT_GSM_Callback(u8 GSM_cmd);

#endif
