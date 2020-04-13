#ifndef __GPS_H
#define __GPS_H	 

#include "stdbool.h"	 		
#include "type1.h"	

#define GPS_Buf_Len		80			// 定义GPS接收数组长度
#define dt_len			125			// 定义dubuf长度

__packed typedef struct  
{										    
 	u8 num;		  	//卫星编号
	u8 eledeg;		//卫星仰角
	u16 azideg;		//卫星方位角
	u8 sn;		  	//信噪比		   
}nmea_slmsg;  

//UTC时间信息
__packed typedef struct  
{
 	u8 year;		//年份
	u8 month;		//月份
	u8 date;		//日期
	u8 hour; 		//小时
	u8 min; 		//分钟
	u8 sec; 		//秒钟
}nmea_utc_time;  

__packed typedef struct  
{	
	nmea_utc_time utc;			// UTC时间
	u32		latitude;			// 纬度 分扩大100000倍,实际要除以100000 
	u32		longitude;			// 经度 分扩大100000倍,实际要除以100000
	u16		SPEED;				// 速度
	u16		course;				// 方向角
	u8		ewhemi;				// 东经/西经,E:东经;W:西经
	u8		nshemi;				// 北纬/南纬,N:北纬;S:南纬		  
	u8		posslnum;			// 用于定位的卫星数,0~12.	
	u16		level;				// 水平因子
	char	AV_indicate;		// 数据是否有效标志u8 a_v;='V'
	
	u8		cmd_code;			// CMD或ALM代码
	
	u8		soc;				// 剩余电量  
	u8		sig_strength;		// 信号强度
	u16		gsmbase1;			// 基站信息
	u32		gsmbase2;			// 基站信息
	u32		operators;			// 运营商
}nmea_msg; 

extern nmea_msg gpsx;
extern nmea_msg	gpsx_his_read;
extern nmea_msg	gpsx_his_write;

typedef union 
{
	struct
	{
		bool GPRMC_Flag:1;					// 1: 出现$GPRMC头
		bool GPGGA_Flag:1;					// 1: 出现$GPGGA头
		bool GPRMC_END_Flag:1;				// 1: 收完一帧GPRMC数据				0: 未接收完或空闲
		bool GPGGA_END_Flag:1;				// 1: 收完一帧GPGGA数据				0: 未接收完或空闲	

		bool Send_Geo1_ALM_flag:1;			// GEO1发报警标志 
		bool Send_Geo2_ALM_flag:1;			// GEO2发报警标志 
		bool Send_Geo3_ALM_flag:1;			// GEO3发报警标志 
		bool Send_Geo4_ALM_flag:1;			// GEO4发报警标志 
		bool Send_Geo5_ALM_flag:1;			// GEO5发报警标志 
		bool Send_Geo6_ALM_flag:1;			// GEO6发报警标志 
		bool Send_overspd_ALM_flag:1;		// 超速发报警标志
		
		bool CMDF_Cnt_Flag:1;				// 执行单次定位计时
						
		bool CMDF_Sendto_SMS_Flag:1;		// 需要回复到手机的定位信息
		bool CMDF_Sendto_Gprs_Flag:1;		// 需要回复到GPRS的定位信息
		bool CMDF_Sendto_SOS_BMA_Flag:1;	// 需要回复到SOS或BMA报警定位信息
		bool CMDF_Powron_5min_Flag:1;		// 开机5分钟获取定位信息
		bool CMDF_Ten_Track_Flag:1;			// 每十分钟钟定位一次
		
		bool Poweron_For_Track_Flag:1;		// 正在连续定位
		bool Track_For_sms_Flag:1;			// 短信连续定位
		bool Track_For_gprs_flag:1;			// 平台连定位
			
		bool AV_Flag:1;						// LED显示AV标志

		bool Power_On_Flag:1;
		
		bool Gps_time_update_Flag:1;		// 定时更新GPS显示时间
				
		u32 reserved:9;
	}b;
	u32 w;
}Def_Flag_GPS;
extern  Def_Flag_GPS GPS_F;

extern unsigned char gps_a_count;
extern char dtbuf[dt_len];
extern char GPS_Str_Buf[6];
extern char GPS_RMC_Buf[GPS_Buf_Len];					
extern char GPS_GGA_Buf[GPS_Buf_Len];	
extern unsigned char GPS_Rx_Cnt; 
extern unsigned char open_gps_count;

extern void GPS_Mode_Setting(void);

extern void NMEA_GPGGA_Analysis(void);
extern unsigned char NMEA_GPRMC_Analysis(void);

extern unsigned char NMEA_Comma_Pos(u8 *buf,u8 cx);

extern bool New_GPS_Handle(void);
extern void Read_GPS_data(unsigned char New_Old_GPS,unsigned char MSG_Type);
extern void UTC_RTC(u8 *year,u8 *month,u8 *day,u8 *hour,u8 *min,u8 zone);

#endif  
