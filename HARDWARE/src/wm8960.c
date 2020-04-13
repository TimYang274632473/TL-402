#include <includes.h>

void PWM_AUDIO_L(void)
{
	HAL_GPIO_WritePin(PWM_AUDIO_GPIO_Port,PWM_AUDIO_GPIO_Pin,GPIO_PIN_RESET);
}

void PWM_AUDIO_R(void)				
{
	HAL_GPIO_TogglePin(PWM_AUDIO_GPIO_Port,PWM_AUDIO_GPIO_Pin);
}

/* ����WM8960ʱ��					*/
void Speak_Amp_On(void)
{
	HAL_GPIO_WritePin(WM8960_EN_GPIO_Port,WM8960_EN_Pin,GPIO_PIN_SET);
}	

/* �ر�WM8960ʱ��					*/
void Speak_Amp_Off(void)
{
	HAL_GPIO_WritePin(WM8960_EN_GPIO_Port,WM8960_EN_Pin,GPIO_PIN_RESET);
}

/* ����������						*/
void SOUND_BI(unsigned char Tim)
{	
	if(CALL.b.Calling_Flag)	{return;	}
			
	Speak_Amp_On();
	
	for(u8 i=0;i<Tim;i++)
	{
		PWM_AUDIO_R();vTaskDelay(1);
	}
		
	Speak_Amp_Off();	
	PWM_AUDIO_L();
}
