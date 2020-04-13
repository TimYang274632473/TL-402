#ifndef __GPS_H
#define __GPS_H	 

#include "stdbool.h"	 		
#include "type1.h"	

#define GPS_Buf_Len		80			// ����GPS�������鳤��
#define dt_len			125			// ����dubuf����

__packed typedef struct  
{										    
 	u8 num;		  	//���Ǳ��
	u8 eledeg;		//��������
	u16 azideg;		//���Ƿ�λ��
	u8 sn;		  	//�����		   
}nmea_slmsg;  

//UTCʱ����Ϣ
__packed typedef struct  
{
 	u8 year;		//���
	u8 month;		//�·�
	u8 date;		//����
	u8 hour; 		//Сʱ
	u8 min; 		//����
	u8 sec; 		//����
}nmea_utc_time;  

__packed typedef struct  
{	
	nmea_utc_time utc;			// UTCʱ��
	u32		latitude;			// γ�� ������100000��,ʵ��Ҫ����100000 
	u32		longitude;			// ���� ������100000��,ʵ��Ҫ����100000
	u16		SPEED;				// �ٶ�
	u16		course;				// �����
	u8		ewhemi;				// ����/����,E:����;W:����
	u8		nshemi;				// ��γ/��γ,N:��γ;S:��γ		  
	u8		posslnum;			// ���ڶ�λ��������,0~12.	
	u16		level;				// ˮƽ����
	char	AV_indicate;		// �����Ƿ���Ч��־u8 a_v;='V'
	
	u8		cmd_code;			// CMD��ALM����
	
	u8		soc;				// ʣ�����  
	u8		sig_strength;		// �ź�ǿ��
	u16		gsmbase1;			// ��վ��Ϣ
	u32		gsmbase2;			// ��վ��Ϣ
	u32		operators;			// ��Ӫ��
}nmea_msg; 

extern nmea_msg gpsx;
extern nmea_msg	gpsx_his_read;
extern nmea_msg	gpsx_his_write;

typedef union 
{
	struct
	{
		bool GPRMC_Flag:1;					// 1: ����$GPRMCͷ
		bool GPGGA_Flag:1;					// 1: ����$GPGGAͷ
		bool GPRMC_END_Flag:1;				// 1: ����һ֡GPRMC����				0: δ����������
		bool GPGGA_END_Flag:1;				// 1: ����һ֡GPGGA����				0: δ����������	

		bool Send_Geo1_ALM_flag:1;			// GEO1��������־ 
		bool Send_Geo2_ALM_flag:1;			// GEO2��������־ 
		bool Send_Geo3_ALM_flag:1;			// GEO3��������־ 
		bool Send_Geo4_ALM_flag:1;			// GEO4��������־ 
		bool Send_Geo5_ALM_flag:1;			// GEO5��������־ 
		bool Send_Geo6_ALM_flag:1;			// GEO6��������־ 
		bool Send_overspd_ALM_flag:1;		// ���ٷ�������־
		
		bool CMDF_Cnt_Flag:1;				// ִ�е��ζ�λ��ʱ
						
		bool CMDF_Sendto_SMS_Flag:1;		// ��Ҫ�ظ����ֻ��Ķ�λ��Ϣ
		bool CMDF_Sendto_Gprs_Flag:1;		// ��Ҫ�ظ���GPRS�Ķ�λ��Ϣ
		bool CMDF_Sendto_SOS_BMA_Flag:1;	// ��Ҫ�ظ���SOS��BMA������λ��Ϣ
		bool CMDF_Powron_5min_Flag:1;		// ����5���ӻ�ȡ��λ��Ϣ
		bool CMDF_Ten_Track_Flag:1;			// ÿʮ�����Ӷ�λһ��
		
		bool Poweron_For_Track_Flag:1;		// ����������λ
		bool Track_For_sms_Flag:1;			// ����������λ
		bool Track_For_gprs_flag:1;			// ƽ̨����λ
			
		bool AV_Flag:1;						// LED��ʾAV��־

		bool Power_On_Flag:1;
		
		bool Gps_time_update_Flag:1;		// ��ʱ����GPS��ʾʱ��
				
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
