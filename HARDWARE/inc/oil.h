
#ifndef __OIL_H
#define __OIL_H
#include "type1.h"
#include "stdbool.h"
typedef struct
{
  signed int oil_tt;
	signed int oil_kk;
  u16 oil_v1;
  u16 oil_v2;
}
oil_def;

typedef struct
{
	u8 rly_operating_time;//继电器吸合时间
	u8 rly_period_time;   //继电器吸合周期
	u8 rotate_time;       //正反转时间
	u8 ACCPWR_state;      //ACC状态
}
vehicle1_def;

extern vehicle1_def vehicle;


typedef union 
{
  struct
	{
		bool BRE_ACT_flag:1;//BRE有效电平选择
		bool DOR_ACT_flag:1;//DOOR线有效电平选择        
		bool BRE_SMS_flag:1;//已发BRE报警消息            
		bool DOR_SMS_flag:1;//已发DOOR报警消息
    bool BRE_FUN_flag:1;//BRE功能选择（1打电话，0按键输入）	
		bool ACC_SMS_flag:1;//已发ACC报警消息
		bool PWR_SMS_flag:1;//已发PWR报警消息
		bool ALM_ALW_flag:1;//1设防，0撤防			
  }b;
  u8 w;
}	
vehicle_def;
extern vehicle_def veh1;

typedef union 
{
  struct
	{
		bool inv_c_flag:1;
		bool inv_flag:1;
		bool fwd_c_flag:1;
		bool fwd_flag:1;
		bool door_flag:1;
		bool pwr_flag:1;
		bool acc_flag:1;
		bool acc_off_flag:1;
		bool acc_pwr_flag:1;//是否接外部电源标志
		bool spkout_flag:1;
		bool spk_flag:1;
		bool read_oil_again_flag:1;		
		u16  reservd:4;
  }b;
  u16 w;
}	
vehicle2_def;
extern vehicle2_def veh2;

typedef union 
{
  struct
	{
		bool enable_temp_flag:1;
    u8  reservd:7;	
  }b;
  u8 w;
}	
vehicle3_def;
extern vehicle3_def  veh3;


extern u8 rly_op_time;
extern u8 acc_count;
extern u8 door_count;
extern u8 pwr_count;
extern u8 inv_count;
extern u8 fwd_count;
extern u8 rotate_sta;
extern u8 tim5_inc;
extern u16 acc_inval_count;
extern u16 vot_d;
extern short ds_temperature;

extern u32 spk_count;
extern u32 spk_count1;
extern u32 _10us;
extern oil_def oil_para[4];

void oilcar_GPS_Alarm_Handle(void);
void read_oil_para(void);

u32 adc_get_oil(void);
u16 chuli_oil(u32 oilvolt);

#endif
