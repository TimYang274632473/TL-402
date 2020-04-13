
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
	u8 rly_operating_time;//�̵�������ʱ��
	u8 rly_period_time;   //�̵�����������
	u8 rotate_time;       //����תʱ��
	u8 ACCPWR_state;      //ACC״̬
}
vehicle1_def;

extern vehicle1_def vehicle;


typedef union 
{
  struct
	{
		bool BRE_ACT_flag:1;//BRE��Ч��ƽѡ��
		bool DOR_ACT_flag:1;//DOOR����Ч��ƽѡ��        
		bool BRE_SMS_flag:1;//�ѷ�BRE������Ϣ            
		bool DOR_SMS_flag:1;//�ѷ�DOOR������Ϣ
    bool BRE_FUN_flag:1;//BRE����ѡ��1��绰��0�������룩	
		bool ACC_SMS_flag:1;//�ѷ�ACC������Ϣ
		bool PWR_SMS_flag:1;//�ѷ�PWR������Ϣ
		bool ALM_ALW_flag:1;//1�����0����			
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
		bool acc_pwr_flag:1;//�Ƿ���ⲿ��Դ��־
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
