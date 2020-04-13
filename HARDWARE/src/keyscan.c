#include "includes.h"

unsigned char  	key_num_counter=0;			// �ж�ɨ�谴������					
unsigned char 	key_num_now=0;				// ��ǰ�ж�ɨ�谴��ֵ				
unsigned char 	key_num_buf=0;				// �ϴ��ж�ɨ�谴��ֵ	
unsigned char	key_call_cs=0;

/* ��ȡPOWER����	*/
bool KEY_CHKPWR_READ(void)		
{
	return(HAL_GPIO_ReadPin(KEY_CHKPWR_GPIO_Port,	KEY_CHKPWR_GPIO_Pin	) ? 1 : 0);
}

/* ��ȡSOS���� 		*/
bool KEY_SOS_READ(void)
{
	return(HAL_GPIO_ReadPin(KEY_SOS_GPIO_Port,		KEY_SOS_GPIO_Pin	) ? 0 : 1);
}

/* ��ȡCALL1����	*/
bool KEY_CALL1_READ(void)		
{
	return(HAL_GPIO_ReadPin(KEY_CALL1_GPIO_Port,	KEY_CALL1_GPIO_Pin	) ? 0 : 1);
}

/* ��ȡCALL2����	*/
bool KEY_CALL2_READ(void)		
{
	return(HAL_GPIO_ReadPin(KEY_CALL2_GPIO_Port,	KEY_CALL2_GPIO_Pin	) ? 0 : 1);
}

/* ɨ���ȡ���ƺ�		*/
unsigned char key_num(void)
{
	if(KEY_CHKPWR_READ())		{return 0x01;}
	
	else if(KEY_SOS_READ())		{return 0x02;}
		
	else if(KEY_CALL1_READ())	{return 0x04;}
	
	else if(KEY_CALL2_READ())	{return 0x08;}
	
	else						{return 0x00;}
}

/* ����ɨ�����		*/
void key_scan_End(void)
{
	key_call_cs=0;
	key_num_buf=0;																		// ������Ч����,����ֵΪ0				
	key_num_counter=0;
	Tim_F.b.Chk_Key_Flag = false;
}

/* 
ͨ����ʱ��ɨ�谴��	
�����ڶ̻�,����,�޻����Ƽ��
*/																					
void key_scan(void)
{		
 	key_num_now=key_num();																// ɨ�谴��,����ȡ��ǰ����ֵ		

	if(key_num_now>0)																	// ����ǰ�а��ư���					
	{
		if(key_num_buf==key_num_now)													// ���˴�ɨ�����ϴν����ͬ,����Ч,���ڰ�������	
		{									
			if(xEventGroupGetBits(EventGroupHandler_MCU) & Event_Key_All)
			{
				key_scan_End();return;													// ����а���δ�������,��ֱ�ӷ���
			}
			
			key_num_counter++;
			
			if(key_num_counter == 1)
			{
				cnt_1s.BiBi_Cnt = 0;
			}
				
			switch(key_num_now)
			{
				case 1:																	// ��Դ��
					if(CALL.b.Calling_Flag)												
					{
						if(key_num_counter>=1)											
						{
							xEventGroupSetBits(EventGroupHandler_MCU,Key_Power_S_Flag);	key_num_now=250;											
						}
					}
					else																
					{
						if(key_num_counter>=80)
						{
							MCU_F.b.Power_Off_Flag = true;
							
							if(MCU_F.b.GSM_Init_Flag)
							{
								Task_SPI_Flash(Flash_MCU_Power_Off,0,0,0);
							}
							else
							{				
								Task_AT_GSM(Gsm_Pwr_off,0,0,0,0,0,0);		
							}
							key_num_now=250;						
						}
					}
					break;
					
				case 2:																	// SOS����
					if(CALL.b.Calling_Flag)									
					{
						if(key_num_counter>=1)
						{
							xEventGroupSetBits(EventGroupHandler_MCU,Key_SOS_S_Flag);key_num_now=250;												
						}
					}
					else 																
					{
						if(key_num_counter>=60)
						{
							xEventGroupSetBits(EventGroupHandler_MCU,Key_SOS_L_Flag);key_num_now=250;											
						}
					}
					break;	
				
				case 4:																	// CALL1����
					if(CALL.b.Calling_Flag)	
					{
						if(key_num_counter >= 1)											
						{									
							xEventGroupSetBits(EventGroupHandler_MCU,Key_Call1_S_Flag);key_num_now=250;			
						}
					}
					else
					{
						if(key_num_counter>=40)
						{
							xEventGroupSetBits(EventGroupHandler_MCU,Key_Call1_L_Flag);key_num_now=250;												
						}
					}
					break;	

				case 8:	
					if(CALL.b.Calling_Flag)	
					{
						if(key_num_counter >= 1)											
						{									
							xEventGroupSetBits(EventGroupHandler_MCU,Key_Call2_S_Flag);key_num_now=250;			
						}
					}
					else
					{
						if(key_num_counter>=40)
						{
							xEventGroupSetBits(EventGroupHandler_MCU,Key_Call2_L_Flag);key_num_now=250;												
						}
					}
					break;
					
				default:
					key_num_now=250;break;			
			}
			
			if((key_num_counter > 85)||(key_num_now==250))								
			{
				key_scan_End();ZDMD_ON();
			}
		}
		else																				
		{
			key_num_buf = key_num_now; key_num_counter=0;																
		}
	} 	
	else																									
	{		
		key_scan_End();
	}
}
