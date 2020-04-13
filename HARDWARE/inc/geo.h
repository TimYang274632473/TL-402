#ifndef __GEO_H
#define __GEO_H

#include "type1.h"
#include "stdbool.h"

__packed typedef struct  
{
	float lat1;
	float lot1;
	float lat2;
	float lot2;
}GEO_coordinate; 

typedef union 
{
  struct
	{
		bool GEO1_flag:1;//第一组GEO开关标志
		bool GEO2_flag:1;//第二组GEO开关标志
		bool GEO3_flag:1;//第三组GEO开关标志
		bool GEO4_flag:1;//第四组GEO开关标志
		bool GEO5_flag:1;//第五组GEO开关标志
	  u8 reserved:3;	
  }b;
  u8 w;
}	
GEO_A_def;
extern  GEO_A_def GEOA;

typedef union 
{
  struct
	{
		bool GEO1_OUT_flag:1;//第一组GEO出去标志
		bool GEO2_OUT_flag:1;//第二组GEO出去标志
		bool GEO3_OUT_flag:1;//第三组GEO出去标志
		bool GEO4_OUT_flag:1;//第四组GEO出去标志
		bool GEO5_OUT_flag:1;//第五组GEO出去标志
	  u8 reserved:3;	
  }b;
  u8 w;
}	
GEO_B_def;
extern  GEO_B_def GEOB;

typedef union 
{
  struct
	{
		bool GEO1_IN_flag:1;//第一组GEO进入标志
		bool GEO2_IN_flag:1;//第二组GEO进入标志
		bool GEO3_IN_flag:1;//第三组GEO进入标志
		bool GEO4_IN_flag:1;//第四组GEO进入标志
		bool GEO5_IN_flag:1;//第五组GEO进入标志
	  u8 reserved:3;	
  }b;
  u8 w;
}	
GEO_C_def;
extern  GEO_C_def GEOC;

typedef union 
{
  struct
	{
		bool GEO1_valid_flag:1;//第一组GEO设置有效标志
		bool GEO2_valid_flag:1;//第二组GEO设置有效标志
		bool GEO3_valid_flag:1;//第三组GEO设置有效标志
		bool GEO4_valid_flag:1;//第四组GEO设置有效标志
		bool GEO5_valid_flag:1;//第五组GEO设置有效标志
	  u8 reserved:3;	
  }b;
  u8 w;
}	
GEO_D_def;
extern  GEO_D_def GEOD;
float dms_to_d(char *str,u8 flag);
extern GEO_coordinate GEO_c[5];
extern void GPS_Alarm_Handle(void);
bool GEO_alarm_analysis(u8 _GEO_IN_flag,u8 _GEO_OUT_flag, float a,float b,float c,float d,unsigned char GEO_NUM);
float get_length(float x1,float y1,float x2, float y2);
extern void send_alarm(unsigned char Ph_Num,char *SMS_CMD,u8 GPRS_CMD,u8 GPS_Type);
#endif

