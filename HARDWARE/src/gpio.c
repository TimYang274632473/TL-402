#include "includes.h"

void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();

	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	
	GPIO_InitStruct.Pin = PWRON_Pin;								// PWRON
	HAL_GPIO_Init(PWRON_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(PWRON_GPIO_Port,PWRON_Pin,GPIO_PIN_SET);
	
	GPIO_InitStruct.Pin = WORK_LED_Pin;								// WORK
	HAL_GPIO_Init(WORK_LED_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(WORK_LED_GPIO_Port, WORK_LED_Pin, GPIO_PIN_SET);
	
	GPIO_InitStruct.Pin = HL_DTR_Pin;								// DTR
	HAL_GPIO_Init(HL_DTR_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(HL_DTR_GPIO_Port, HL_DTR_Pin, GPIO_PIN_SET);
	
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	
	GPIO_InitStruct.Pin = ZDMD_Pin;									// ZDMD
	HAL_GPIO_Init(ZDMD_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(ZDMD_GPIO_Port, ZDMD_Pin, GPIO_PIN_RESET);
	
	GPIO_InitStruct.Pin = WIFI_Pin;									// 初始化WIFI电源关闭
	HAL_GPIO_Init(WIFI_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(WIFI_GPIO_Port, WIFI_Pin, GPIO_PIN_RESET);
	
	GPIO_InitStruct.Pin = CS_W25_Pin;								// CS_W25
	HAL_GPIO_Init(CS_W25_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(CS_W25_GPIO_Port, CS_W25_Pin, GPIO_PIN_SET);
				
	GPIO_InitStruct.Pin = WM8960_EN_Pin;							// WM8960_EN
	HAL_GPIO_Init(WM8960_EN_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(WM8960_EN_GPIO_Port, WM8960_EN_Pin, GPIO_PIN_RESET);
	
	GPIO_InitStruct.Pin = GSMON_Pin;								// GSMON
	HAL_GPIO_Init(GSMON_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GSMON_GPIO_Port, GSMON_Pin, GPIO_PIN_RESET);	
	
	GPIO_InitStruct.Pin = PWM_AUDIO_GPIO_Pin;						// AUDIO
	HAL_GPIO_Init(PWM_AUDIO_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(PWM_AUDIO_GPIO_Port, PWM_AUDIO_GPIO_Pin, GPIO_PIN_RESET);
		
	GPIO_InitStruct.Pin = GPS_VCON_Pin;								// GPS_VCON
	HAL_GPIO_Init(GPS_VCON_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPS_VCON_GPIO_Port, GPS_VCON_Pin, GPIO_PIN_RESET);

	// 以下为OD
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	
	GPIO_InitStruct.Pin = GPS_LED_Pin;								// GPS
	HAL_GPIO_Init(GPS_LED_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPS_LED_GPIO_Port, GPS_LED_Pin, GPIO_PIN_SET);

	GPIO_InitStruct.Pin = GSM_LED_Pin;								// GSM
	HAL_GPIO_Init(GSM_LED_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GSM_LED_GPIO_Port, GSM_LED_Pin, GPIO_PIN_SET);
	
	GPIO_InitStruct.Pin = GPRS_LED_Pin;								// GPRS
	HAL_GPIO_Init(GPRS_LED_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPRS_LED_GPIO_Port, GPRS_LED_Pin, GPIO_PIN_SET);

	GPIO_InitStruct.Pin = IIC_SCL_Pin;								// SCL
    HAL_GPIO_Init(IIC_SCL_GPIO, &GPIO_InitStruct);
	HAL_GPIO_WritePin(IIC_SCL_GPIO,PWRON_Pin,GPIO_PIN_SET);
	
	GPIO_InitStruct.Pin = IIC_SDA_Pin;								// SDA
    HAL_GPIO_Init(IIC_SDA_GPIO, &GPIO_InitStruct);
	HAL_GPIO_WritePin(IIC_SDA_GPIO,IIC_SDA_Pin,GPIO_PIN_SET);

	// 以下为中断
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;

	GPIO_InitStruct.Pin = CHK_VIN_Pin;								// CHK_VIN
	HAL_GPIO_Init(CHK_VIN_GPIO_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = CHK_CH_Pin;								// CHK_CH 上升下降中断
	HAL_GPIO_Init(CHK_CH_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;					

	GPIO_InitStruct.Pin = KEY_SOS_GPIO_Pin;							// SOS
	HAL_GPIO_Init(KEY_SOS_GPIO_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = KEY_CALL1_GPIO_Pin;						// CALL1
	HAL_GPIO_Init(KEY_CALL1_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = KEY_CALL2_GPIO_Pin;						// CALL2
	HAL_GPIO_Init(KEY_CALL2_GPIO_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin= BMA250_GPIO_Pin;							// BMA250
	HAL_GPIO_Init(BMA250_GPIO_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pull = GPIO_NOPULL;
			
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;	
	
	GPIO_InitStruct.Pin = KEY_CHKPWR_GPIO_Pin;						// CHKPWR
	HAL_GPIO_Init(KEY_CHKPWR_GPIO_Port, &GPIO_InitStruct);
}

void power_off(void)
{
	if(CALL.b.Calling_Flag == false)
	{		
		SOUND_BI(BIBI);	ZDMD_ON();vTaskDelay(100);
																		
		while(PWRkey_chk() == SET) {vTaskDelay(100);}			
			
		while(1) 
		{	
			HAL_GPIO_WritePin(PWRON_GPIO_Port,PWRON_Pin,GPIO_PIN_RESET);
			
			vTaskDelay(500);												
		}
	}
}

void motor_on(void)
{
	HAL_GPIO_WritePin(ZDMD_GPIO_Port,ZDMD_Pin,GPIO_PIN_SET);
}	

void motor_off(void)
{
	HAL_GPIO_WritePin(ZDMD_GPIO_Port,ZDMD_Pin,GPIO_PIN_RESET);
}	

void ZDMD_ON(void)
{
	motor_on();
	cnt_1ms.Motor_Off_Cnt = 3;
}

//GPS上电
void GPS_Power_On(void)
{   		
	if(!GSMB.b.GPS_PowerOffMode_Flag)									// 已设定可以对GPS上电
	{
		GPS_Force_On();
	}	
}

//GPS关电
void GPS_Power_Off(void) 
{   
	if((!GSMB.b.GPS_StrongMode_Flag) && (!GPS_F.b.CMDF_Cnt_Flag) && (!GPS_F.b.Poweron_For_Track_Flag))
	{
		if((!GSMA.b.Track_Flag) || (GSMA.b.Track_Flag && (cnt_1s.Track_Time_Set > 20)))
		{	
			GPS_Force_Off();
		}
	}		
}

// GPS强行上电
void GPS_Force_On(void)
{   		
	if(GPS_F.b.Power_On_Flag == false)
	{
		GPS_F.b.Power_On_Flag = true;
		HAL_GPIO_WritePin(GPS_VCON_GPIO_Port,GPS_VCON_Pin,GPIO_PIN_SET);
	}
}

// GPS强行关机
void GPS_Force_Off(void)
{
	if(GPS_F.b.Power_On_Flag == true)
	{
		GPS_F.b.Power_On_Flag = false;
		HAL_GPIO_WritePin(GPS_VCON_GPIO_Port,GPS_VCON_Pin,GPIO_PIN_RESET);
	}
}

// GSMON_H
void GSMON_H(void)
{
	HAL_GPIO_WritePin(GSMON_GPIO_Port, GSMON_Pin, GPIO_PIN_SET);
}

// GSMON_L
void GSMON_L(void)
{
	HAL_GPIO_WritePin(GSMON_GPIO_Port, GSMON_Pin, GPIO_PIN_RESET);
}


void WORK_LED_On(void)
{
	HAL_GPIO_WritePin(WORK_LED_GPIO_Port, WORK_LED_Pin, GPIO_PIN_SET);
}

void WORK_LED_Off(void)
{
	HAL_GPIO_WritePin(WORK_LED_GPIO_Port, WORK_LED_Pin, GPIO_PIN_RESET);
}

void WORK_LED_R(void)
{
	HAL_GPIO_TogglePin(WORK_LED_GPIO_Port,WORK_LED_Pin);
}

void GPRS_LED_On(void)
{
	HAL_GPIO_WritePin(GPRS_LED_GPIO_Port, GPRS_LED_Pin, GPIO_PIN_RESET);
}

void GPRS_LED_Off(void)
{
	HAL_GPIO_WritePin(GPRS_LED_GPIO_Port, GPRS_LED_Pin, GPIO_PIN_SET);
}

void GPRS_LED_R(void)
{
	HAL_GPIO_TogglePin(GPRS_LED_GPIO_Port,GPRS_LED_Pin);
}

void GPS_LED_On(void)
{
	HAL_GPIO_WritePin(GPS_LED_GPIO_Port, GPS_LED_Pin, GPIO_PIN_RESET);
}

void GPS_LED_Off(void)
{
	HAL_GPIO_WritePin(GPS_LED_GPIO_Port, GPS_LED_Pin, GPIO_PIN_SET);
}

void GPS_LED_R(void)
{
	HAL_GPIO_TogglePin(GPS_LED_GPIO_Port,GPS_LED_Pin);
}

void GSM_LED_On(void)
{
	HAL_GPIO_WritePin(GSM_LED_GPIO_Port, GSM_LED_Pin, GPIO_PIN_RESET);
}

void GSM_LED_Off(void)
{
	HAL_GPIO_WritePin(GSM_LED_GPIO_Port, GSM_LED_Pin, GPIO_PIN_SET);
}

void GSM_LED_R(void)
{
	HAL_GPIO_TogglePin(GSM_LED_GPIO_Port,GSM_LED_Pin);
}

//SIM800退出睡眠模式
void GSM_Wakeup(void)
{
	HAL_GPIO_WritePin(HL_DTR_GPIO_Port,HL_DTR_Pin,GPIO_PIN_RESET);
	
	if(!MCU_F.b.GSM_wakeup_Flag)		
	{
		MCU_F.b.GSM_wakeup_Flag = true;vTaskDelay(100);	
	}
}

//SIM800进入睡眠模式
void GSM_Sleep(void)
{				
	if((CALL.b.Calling_Flag == false) && (GPRS.b.GPRS_send_history_Flag == false) && MCU_F.b.GSM_wakeup_Flag)
	{
		HAL_GPIO_WritePin(HL_DTR_GPIO_Port,HL_DTR_Pin,GPIO_PIN_SET);	
		MCU_F.b.GSM_wakeup_Flag = false;
	}
}

//USB插入检测
GPIO_PinState USBinsert_chk(void)
{	
	return(HAL_GPIO_ReadPin(CHK_VIN_GPIO_Port,CHK_VIN_Pin)); 
}

GPIO_PinState PWRkey_chk(void)
{
	return(HAL_GPIO_ReadPin(KEY_CHKPWR_GPIO_Port,KEY_CHKPWR_GPIO_Pin)); 
}	

GPIO_PinState CHK_CH(void)
{
	return(HAL_GPIO_ReadPin(CHK_CH_GPIO_Port,CHK_CH_Pin)); 
}	



