#include "includes.h"

unsigned char  	key_num_counter=0;			// 中断扫描按键次数					
unsigned char 	key_num_now=0;				// 当前中断扫描按键值				
unsigned char 	key_num_buf=0;				// 上次中断扫描按键值	
unsigned char	key_call_cs=0;

/* 读取POWER按键	*/
bool KEY_CHKPWR_READ(void)		
{
	return(HAL_GPIO_ReadPin(KEY_CHKPWR_GPIO_Port,	KEY_CHKPWR_GPIO_Pin	) ? 1 : 0);
}

/* 读取SOS按键 		*/
bool KEY_SOS_READ(void)
{
	return(HAL_GPIO_ReadPin(KEY_SOS_GPIO_Port,		KEY_SOS_GPIO_Pin	) ? 0 : 1);
}

/* 读取CALL1按键	*/
bool KEY_CALL1_READ(void)		
{
	return(HAL_GPIO_ReadPin(KEY_CALL1_GPIO_Port,	KEY_CALL1_GPIO_Pin	) ? 0 : 1);
}

/* 读取CALL2按键	*/
bool KEY_CALL2_READ(void)		
{
	return(HAL_GPIO_ReadPin(KEY_CALL2_GPIO_Port,	KEY_CALL2_GPIO_Pin	) ? 0 : 1);
}

/* 扫描获取按制号		*/
unsigned char key_num(void)
{
	if(KEY_CHKPWR_READ())		{return 0x01;}
	
	else if(KEY_SOS_READ())		{return 0x02;}
		
	else if(KEY_CALL1_READ())	{return 0x04;}
	
	else if(KEY_CALL2_READ())	{return 0x08;}
	
	else						{return 0x00;}
}

/* 按键扫描完成		*/
void key_scan_End(void)
{
	key_call_cs=0;
	key_num_buf=0;																		// 按键无效按下,返回值为0				
	key_num_counter=0;
	Tim_F.b.Chk_Key_Flag = false;
}

/* 
通过定时器扫描按制	
可用于短击,长击,无击按制检测
*/																					
void key_scan(void)
{		
 	key_num_now=key_num();																// 扫描按制,并读取当前按制值		

	if(key_num_now>0)																	// 若当前有按制按下					
	{
		if(key_num_buf==key_num_now)													// 若此次扫描与上次结果相同,则有效,用于按键消拌	
		{									
			if(xEventGroupGetBits(EventGroupHandler_MCU) & Event_Key_All)
			{
				key_scan_End();return;													// 如果有按键未处理完成,则直接返回
			}
			
			key_num_counter++;
			
			if(key_num_counter == 1)
			{
				cnt_1s.BiBi_Cnt = 0;
			}
				
			switch(key_num_now)
			{
				case 1:																	// 电源键
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
					
				case 2:																	// SOS按键
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
				
				case 4:																	// CALL1按制
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
