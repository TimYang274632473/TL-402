/**
  ******************************************************************************
  * File Name          : RTC.c
  * Description        : This file provides code for the configuration
  *                      of the RTC instances.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "includes.h"

RTC_HandleTypeDef hrtc;
	
/* 初始化设置RTC定时器,每1S中断一次,由内部LSI提供时钟频率 */
void MX_RTC_Init(void)
{		
	hrtc.Instance			= RTC;					// 设置基准时间为1S
	hrtc.Init.HourFormat	= RTC_HOURFORMAT_24;
	hrtc.Init.AsynchPrediv	= 127;					// LSE:127; LSI:124
	hrtc.Init.SynchPrediv	= 255;					// LSE:255;	LSI:295
	hrtc.Init.OutPut		= RTC_OUTPUT_DISABLE;
	hrtc.Init.OutPutRemap	= RTC_OUTPUT_REMAP_NONE;
	hrtc.Init.OutPutPolarity= RTC_OUTPUT_POLARITY_HIGH;
	hrtc.Init.OutPutType	= RTC_OUTPUT_TYPE_OPENDRAIN;
	if(HAL_RTC_Init(&hrtc) != HAL_OK)
	{
		Error_Handler();
	}
}

/*返回1为周一, 7为周日*/
unsigned char CalculateWeekDay(int YY, int MM, int DD)
{  
    if(MM==1||MM==2) MM+=12,YY--;  
	
    int iWeek = (DD+2*MM+3*(MM+1)/5+YY+YY/4-YY/100+YY/400)%7;  
	
	return iWeek+1;
} 

void RTC_Write_Year_Time(unsigned char Year,unsigned char Mouth, unsigned char Day, unsigned char Hour, unsigned char Minutes,unsigned char Second)
{
	RTC_TimeTypeDef Set_Time;
	RTC_DateTypeDef Set_Date;
	
	if((Year < 18) || (Year > 38))return;							// 不接收2000以前的设置
	
	Set_Time.Hours   = Hour;
	Set_Time.Minutes = Minutes;
	Set_Time.Seconds = Second;
	Set_Time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	Set_Time.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(&hrtc, &Set_Time, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}
	
	Set_Date.Year		= Year;
	Set_Date.Month		= Mouth;
	Set_Date.Date 		= Day;
	Set_Date.WeekDay	= CalculateWeekDay(Year,Mouth,Day);
	if (HAL_RTC_SetDate(&hrtc, &Set_Date, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}
}

#if RTC_Alarm

void RTC_Write_Alarm(unsigned char Hour,unsigned char Minutes)
{
	RTC_AlarmTypeDef sAlarm;
	
	if((Hour > 23) || (Minutes > 59)){return;}
	
	sAlarm.AlarmTime.Hours			= Hour;
	sAlarm.AlarmTime.Minutes		= Minutes;
	sAlarm.AlarmTime.Seconds		= 0x00;
	sAlarm.AlarmTime.SubSeconds		= 0x00;
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarm.AlarmMask				= RTC_ALARMMASK_DATEWEEKDAY;
	sAlarm.AlarmSubSecondMask		= RTC_ALARMSUBSECONDMASK_NONE;
	sAlarm.AlarmDateWeekDaySel		= RTC_ALARMDATEWEEKDAYSEL_DATE;
	sAlarm.AlarmDateWeekDay			= 1;
	sAlarm.Alarm = RTC_ALARM_A;
	
	if(HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}
}

#endif

/* 启动RTC定时器设置				*/
void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{
	if(rtcHandle->Instance==RTC)
	{
		__HAL_RCC_RTC_ENABLE();
		HAL_NVIC_SetPriority(RTC_IRQn, 7, 0);
		HAL_NVIC_EnableIRQ(RTC_IRQn);
	}
}

/* 关闭RTC定时器设置				*/
void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{
	if(rtcHandle->Instance==RTC)
	{
		__HAL_RCC_RTC_DISABLE();
		HAL_NVIC_DisableIRQ(RTC_IRQn);
	}
} 
