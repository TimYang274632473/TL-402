#ifndef __OLED_H
#define __OLED_H

#include "stdbool.h"

typedef union 
{
	struct
	{
		bool On_Off_Flag:1;								// LCD����״̬		
		bool Angle_Now_Flag:1;						 	// LCDĿǰ��ʾ�Ƕ�						
		bool Angle_Buf_Flag:1;							// LCD֮ǰ��ʾ�Ƕ�
		bool KeepOn_Cnt_Flag:1;	 						// LCD��������ʱ��־
		bool Charging_Flag:1;							// ���ڳ���־
		bool Charging_Cnt_Flag:1;						// LCD��ʾ�������ʱ��־
		bool Turnoff_Cnt_Flag:1;						// �ڶ�ʱ����ִ��Ϩ������ʱ	
		bool Power_OFF_Flag:1;							// ��ʾ�ػ�
	}b;
	 unsigned char w;
}LCD_para_def;

extern LCD_para_def LCD_F;






extern unsigned char OLED_Charge_Value;

void OLED_DrawPoint(unsigned char x,unsigned char y,unsigned char t);
void OLED_Fill(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2,unsigned char dot);
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr,unsigned char size,unsigned char mode);
void OLED_ShowNum(unsigned char x,unsigned char y,unsigned long num,unsigned char len,unsigned char size);
void OLED_ShowString(unsigned char x,unsigned char y,char *p,unsigned char size);
		
void draw_circle(unsigned char x,unsigned char y,unsigned char r,unsigned char color);
void draw_line(unsigned char startx,unsigned char starty,unsigned char endx,unsigned char endy,unsigned char color);
void draw_vertical_line(unsigned char y0,unsigned char y1,unsigned char x,unsigned char color);
void draw_level_line(unsigned char x0,unsigned char x1,unsigned char y,unsigned char color);
unsigned int my_abs(int a);

extern void OLED_On(char Charging);
extern void OLED_Off(unsigned char ForceOff);
extern void OLED_Refresh_Gram(unsigned char Dir);
extern void OLED_Interface_2String_preogress(char *Str1,char *Str2,unsigned char progress);
extern void OLED_Interface_Main(void);
extern void OLED_Interface_Battery_Charging(void);
extern void OLED_Interface_Call_InOut(char *Str,unsigned char InOut,unsigned char SOS_BMA);
extern void Init_oled(void);

extern void OLED_SOS_Select_PhoneNumber(unsigned char PhoneNum);

#endif

