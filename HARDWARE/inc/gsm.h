#ifndef __GSM_H
#define __GSM_H

#include "includes.h"

#define	CMD_A	0x01		// 指令A
#define	CMD_A1	0x02		// 指令A1
#define	CMD_A2	0x03		// 指令A2
#define	CMD_A3	0x04		// 指令A3
#define	CMD_A4	0x05		// 指令A4
#define	CMD_A5	0x06		// 指令A5
#define	CMD_A6	0x07		// 指令A6
#define	CMD_A7	0x08		// 指令A7
#define	CMD_A8	0x09		// 指令A8
#define	CMD_B	0x0A		// 指令B
#define	CMD_C	0x0B		// 指令C
#define	CMD_D	0x0C		// 指令D
#define	CMD_E	0x0D		// 指令E
#define	CMD_F	0x0E		// 指令F
#define CMD_F1	0x0F		// 
#define	CMD_G	0x10		// 指令G
#define	CMD_H	0x11		// 指令H
#define	CMD_I	0x12		// 指令I
#define	CMD_J	0x13		// 指令G
#define	CMD_K	0x14		// 指令K
#define	CMD_K0	0x15		// 指令K
#define	CMD_K1	0x16	 	// 指令K
#define	CMD_K2	0x17		// 指令K
#define	CMD_K3	0x18		// 指令K
#define	CMD_L	0x19		// 指令L
#define	CMD_L1	0x1A		// 指令L
#define	CMD_L2	0x1B		// 指令L
#define	CMD_L3	0x1C		// 指令L
#define	CMD_L4	0x1D		// 指令L
#define CMD_L5	0x1E		// 指令L5
#define	CMD_M	0x1F		// 指令M
#define	CMD_M2	0x20		// 指令M2
#define CMD_M3	0x21		// 指令M3
#define CMD_M4	0x22		// 指令M4
#define	CMD_N	0x23		// 指令N
#define	CMD_O	0x24		// 指令O
#define	CMD_P	0x25		// 指令P
#define	CMD_Q1	0x26		// 指令Q1
#define	CMD_Q2	0x27		// 指令Q2
#define	CMD_Q3	0x28		// 指令Q3
#define CMD_Q4	0x29		// 指令Q4
#define CMD_Q5	0x2A		// 指令Q5
#define	CMD_R	0x2B		// 指令R
#define	CMD_S	0x2C		// 指令S
#define	CMD_T	0x2D		// 指令T
#define	CMD_U	0x2E		// 指令U
#define	CMD_U0	0x2F		// 指令U0
#define	CMD_U1	0x30		// 指令U1
#define	CMD_U2	0x31		// 指令U2
#define	CMD_U3	0x32		// 指令U3
#define	CMD_U4	0x33		// 指令U4
#define	CMD_U5	0x34		// 指令U5
#define	CMD_U6	0x35		// 指令U6
#define	CMD_U7	0x36		// 指令U7
#define	CMD_U8	0x37		// 指令U8
#define	CMD_U9	0x38		// 指令U9
#define	CMD_V	0x39		// 指令V
#define	CMD_V3	0x3A		// 指令V
#define	CMD_W	0x3B		// 指令W
#define	CMD_X	0x3C		// 指令X
#define	CMD_Y	0x3D		// 指令Y
#define	CMD_Z1	0x3E		// 指令Z
#define CMD_Z2	0x3F		// 指令Z2

#define	ALM_A	0x81		// 警报A
#define	ALM_B1	0x82		// 警报B1
#define	ALM_B2	0x83		// 警报B2
#define	ALM_B3	0x84		// 警报B3
#define	ALM_B4	0x85		// 警报B4
#define	ALM_B5	0x86		// 警报B5
#define	ALM_C	0x87		// 警报C
#define	ALM_D	0x88		// 警报D
#define	ALM_E	0x89		// 警报E
#define	ALM_F	0x8A		// 警报F
#define	ALM_G	0x8B		// 警报G
#define	ALM_H	0x8C		// 警报H
#define	ALM_I	0x8D		// 警报I
#define	ALM_J	0x8E		// 警报G
#define	ALM_K	0x8F		// 警报K
#define	ALM_L	0x90		// 警报L
#define	ALM_M	0x91		// 警报M
#define	ALM_N	0x92		// 警报N
#define	ALM_O	0x93		// 警报O
#define	ALM_P	0x94		// 警报P
#define	ALM_Q	0x95		// 警报Q
#define	ALM_R	0x96		// 警报R
#define	ALM_S	0x97		// 警报S
#define	ALM_T	0x98		// 警报T
#define	ALM_U	0x99		// 警报U
#define	ALM_V	0x9A		// 警报V
#define	ALM_W	0x9B		// 警报W
#define	ALM_X	0x9C		// 警报X
#define	ALM_Y	0x9D		// 警报Y
#define	ALM_Z	0x9E		// 警报Z
	
#define	CMD_ERR	0xFE		// 错误指令


#if  MODULE_UG96

#define MIC_Spk	13
#define MIC_Lis	14

#elif MODULE_SIM7500

#define MIC_Spk	3
#define MIC_Lis	4

#endif
	
typedef enum
{
	No_Save = 0x00,
	Do_Save	= 0x01,
}Save_or_Not;

typedef enum
{
	RPLY_TO_PC  =0x10,
	RPLY_TO_SMS =0x20,
	RPLY_TO_GPRS=0x30,	
}sendto_Type;
	
typedef enum
{
	NOGPS    =0x00,
	NEWGPS   =0x01,
	CMDFGPS  =0x02,
	OLDGPS   =0x03,
	HISGPS   =0x04,
	ALMGPS   =0x05,
}getgps_Type;

typedef enum
{
	MSG_NO		= 0x00,
	MSG_GPRS 	= 0x01,
	MSG_GOOGLE 	= 0x02,
}
Def_MSG_Type;

typedef union 
{
	struct
	{
		bool GPRS_flag:1;          		// 1: 开启GPRS有效标志
		bool Track_Flag:1;         		// 1: 开启连续跟踪标志		
		bool ALM_OverSpeed_Flag:1; 		// 1: 超速报警打开标志
		bool ALM_LowBat_Flag:1;    		// 1: 低电报警打开标志
		bool ALM_Circle_GEO_Flag:1;    	// 1: 圆心电子围栏打开标志
		bool SOS_CALL_ENABLE_Flag:1;	// 1: 是否允许拨打SOS电话
		bool Any_CallIn_Flag:1;			// 1: 接听任意电话,0:比较授权号码
		bool CallIn_ByMe_Flag:1;		// 1: 由自已主动接通或挂机; 	0: 自动接收授权号码
  }b;
  u8 w;
}GSM_A_def;
extern  GSM_A_def GSMA;

typedef union 
{
	struct
	{
		bool Callin_SpkMode_Flag:1;		// 1: 打进通话					0: 监听
		bool Callout_LisMode_Flag:1;	// 1: 打出静音					0: 通话          
		bool GPS_PowerOffMode_Flag:1;	// 1: 不开启GPS电源				0: 可以开启GPS电源    
		bool GPRS_SaveMode_Flag:1;		// 1: GPRS省流量模式			0: 按时上传数据			
		bool GPS_StrongMode_Flag:1;		// 1: GPS超强模式,不关电		0: 省电模式
		bool ALM_Bat_Full_On_Flag:1;	// 1: 开启充满电报警标志
		bool ALM_Bat_Full_Send_Flag:1;	// 1: 已发送充满电标志
		bool RTC_Update_Flag:1;			// 1: 已更新过RTC时间;			0: 未更新RTC时间		
  }b;
  u8 w;
}GSM_B_def;
extern  GSM_B_def GSMB;

typedef union 
{
	struct
	{
		bool GPS_Tran_Mode_Flag:1;		// 1: GPS默认输出所有数据		0: 只输出出GGA RMC
		bool DTMF_Det_Flag:1;			// 1: 开启语音邮箱检测
		bool SMS_Check_psw_Flag:1;		// 1: 短信需要比对密码
		bool RTC_Alarm_Flag:1;			// 1: 已开启RTC闹铃
		u8 reserved:4;
	}b;
	u8 w;
}GSM_C_def;
extern  GSM_C_def GSMC;

typedef union 
{
	struct
	{
		bool CMDF_SOS_Flag:1;						// 需要发送SOS报警定位信息
		bool CMDF_BMAL_Flag:1;						// 需要发送BMA跌落报警定位信息
		bool CMDF_BMAH_Flag:1;
		u8 reserved:5;
	}b;
	u8 w;
}GSM_D_def;
extern  GSM_D_def GSMD;

__packed typedef struct  
{	
	char	IMEI[16];           // IMEI号
	char	apn[30];            // APN
	char	TCP_UDP[4];			// TCP
	char	APN_username[23];   // APN用户名
	char	APN_password[12];   // APN密码
	char	ip_addr[18];        // IP地址
	char	ip_port[6];         // IP端口
	char	domain_name[24];    // 域名
	char	domain_port[6];     // 端口
	u8		Track_Set_Time;     // 连续定位间隔时间（不带时间单位）
	char	Track_Set_Unit;     // 连续定位时间单位
	u8		TimeZone;           // 时区时
	u8		set_speed;          // 超速阈值
	u8		set_soc;            // 低电阈值
	u16		heartbeatset;      	// 发送心跳间隔时间
	u8		geo_radius;         // 圆心电子围栏半径
	float	center_lat;        	// 圆心电子围栏中心纬度
	float	center_lot;        	// 圆心电子围栏中心经度
	u32		send_history;       // 已发送历史数据条数	
	u32		store_history;      // 已存入历史数据条数
	u8		spk_volume;	     	// 通话音量
	u8		lowg;				// 跌落阀值
	u8		higg;				// 碰撞阀值
	u8		ATE0_Fail_Cnt;		// GSM ATEO自动波特率测试失败次数
	u8		RTC_Alarm_Hour;		// 闹铃时
	u8		RTC_Alarm_Min;		// 闹铃分
	char	cmd_key[7];         // 指令密码，初始值：123456
}GSM_para; 

extern GSM_para GSM_P;

extern  const GSM_para default_GSM_P;

__packed typedef struct  
{
	char temp_ph1[24];
	char temp_ph2[24];
	char temp_ph3[24];
	char temp_ph4[24];
	char temp_ph5[24];
	char temp_ph6[24];
	char temp_ph7[24];
	char temp_ph8[24];
}GSM_phone; 

extern GSM_phone GSM_ph;

__packed typedef struct  
{
	char name_ph1[13];
	char name_ph2[13];
	char name_ph3[13];
	char name_ph4[13];
	char name_ph5[13];
	char name_ph6[13];
	char name_ph7[13];
	char name_ph8[13];
}GSM_phonename; 

extern GSM_phonename GSM_phname;

typedef union 
{
	struct
	{
		bool Chk_CMGL_Flag:1;						// 检查所有未读取过的短信列表
		bool Chk_CMGS_Flag:1;						// 检查发送短信数据结果	
		bool CMDD_SMS_Flag:1;						// D指令回复短信
		bool Track_SMS_Flag:1;						// 执行连续定位发送到手机计时	
		bool New_SMS_Flag:1;						// 收到新短信标志
		u8  reserved:3;
	}b;
	u8 w;
}GSM_sms_def;
extern  GSM_sms_def SMS;

typedef union 
{
	struct
	{
		bool Ring_Flag:1;							// 有来电响铃标志
		bool Ring_Cnt_Flag:1;						// 启动来电响铃计时	
		bool Calling_Flag:1;						// 正在通话标志	
		bool Turn_Vol_En_Flag:1;					// 允许通话过程中调节音量标志;
		bool Lis_To_Spk_En_Flag:1;					// 在通话过程中将静音切换为通话	
		bool SOS_HangUp_En_Flag:1;					// 允许SOS在通话过程中挂断标志; 				
		bool CallInPhone_Save_Flag:1;				// 已保存来电号码	
		bool Play_Ring_Flag:1;						// 播放来电响铃音频
		bool Play_Audio_Flag:1;						// 播放音频
		bool Dtmf_Flag:1;							// 开启dtmf检测
		u16  reserved:6;
	}b;
	u16 w;
}GSM_call_def;
extern GSM_call_def CALL;

typedef union 
{
	struct
	{	
		bool Connected_OK_Flag:1;					// GPRS网络OK
		bool Connected_Now_Flag:1;
		
		bool Chk_Notif_Flag:1;				   		// 发送GPRS后,启动串口获取到模块标志
		
		bool Chk_Connected_Flag:1;					// 通过串口检查联网状态	
		bool Chk_Ciprxget_Flag:1;					// 启动GPRS获取平台发来的数据
		
		bool Track_GPRS_Flag:1;						// 执行连续定位发送到GPRS计时
				
		bool Reconnect_Cnt_Flag:1;					// 启动GPRS重联定时计数器	
		
		bool OTA_Buf_Flag:1;						// OTA接收到DMA数据组	
		bool OTA_NOW_Flag:1;						// 正在OTA	
		bool OTA_Get_Head_Flag:1;					// OTA获取数据头
		
		bool GPRS_send_history_Flag:1;				// 正在发送历史数据
		
		bool GPRS_Save_now_Flag:1;					// 已工作在省电模式
		
		bool WIFI_Tcp_Connected_Flag:1;				// TIM-是否通过wifi建立tcp连接平台	
		
		u16  reserved:3;		
	}b;
	u16 w;
}GSM_gprs_def;
extern  GSM_gprs_def GPRS;




#define gsm_rx_len	200
extern char gsm_rx_buf[gsm_rx_len];  
#if MODULE_SIM7500
extern unsigned short int gsm_rx_sta;
#else
extern unsigned char gsm_rx_sta; 
#endif
extern unsigned char gsm_rx_req;	

extern unsigned char network;

extern signed char temperature;

extern char Ring_Phone_Number[20];
extern char SMS_Phone_Number_Backup[24];

extern bool Isdigit(char *Str, unsigned char Num);
extern bool Isxdigit(char *Str,unsigned char Num);
extern unsigned int Char_To_Dig(char *Str,unsigned char Num);
extern unsigned int Char_To_Hex(char *Str,unsigned char Num);

extern unsigned char Compare_phone_number(char*_mm);

extern u8 find_str(char *a);
extern void clr_gsm_buf(void);	
extern void send_str_to_gsm(char *pcString);

extern bool GSM_AT_Send(char *sstr,u16 rcount,u8 rtime,u8 atcount,u32 rstrtype);

extern bool CPIN_P(void);
extern void ATE0_Test(void);
extern void GSM_init(void);

extern void Gsm_Get_Parameters(u8 flag1);

#if CALLIN_ENABLE || CALLOUT_ENABLE

extern unsigned char Ring_Clip_Count;// 来电响铃次数

extern void ATH_P(void);
extern void Gsm_Calling_Clr_Flag(void);
extern void Calling_Set_Mode(bool SOS_Hangup,bool Turn_Vol,bool Lis_To_Spk);
extern void spk_mic(u8 spk,u8 mic);
extern void Spk_Vol_Turn(bool add_dec);
extern void GSM_Calling_Handle(void);
extern bool GSM_Callin_Handle(void);
extern bool SOS_BMA_Handle(void);
extern unsigned char CPAS_P(void);
extern unsigned char Callout_Phone(char*PhoneNum,unsigned char Call_Time,bool Call_LisMode, bool hangup,unsigned char SOS_BMA);
extern void SOS_BMA_Send_AlmMsg_And_Callout(unsigned char GPS_Type);
extern void SOS_BMA_F_GPS_Start(unsigned char CMD);
#if MODULE_SIM7500
extern void CSDVC_P(void);
#endif
#endif

#if SMS_ENABLE

extern unsigned char SMS_NUM;
extern void cmgd_p(bool UnRead_All);
extern void	New_SMS_Handle(void);
extern void Send_Msg_To_Phone(char *phone_num,char *str1,u8 GPS_state,u8 MSG_Type);

#endif


#if GPRS_ENABLE

#if MODULE_UG96
extern bool QIACT_P(void);
#endif

extern bool GPRS_Check_SendOK(void);
extern void GPRS_Disconnect(void);
extern void GPRS_Data_Handle(void);
extern bool GPRS_APN_Connect(void);
extern bool GPRS_Connect(unsigned char Reset_UTC);
extern void Send_Msg_To_Platform(u8 GPRS_CMD,u8 GPS_Type,u8 MSG_Type,u8 Save);

#endif

extern void Set_Track_Time(void);
extern void analysis_sms_content(char *Str,unsigned char Reply);

void CMD_1_Handle(char *Str1,unsigned char Reply1);
void CMD_2_Handle(char *Str2,unsigned char Reply2);
void CMD_4_Handle(char *Str4,unsigned char Relay4);
void CMD_5_Handle(char *Str5,unsigned char Relay5);
void CMD_A_Handle(char *StrA,unsigned char ReplyA);
void CMD_B_Handle(char *StrB,unsigned char ReplyB);
void CMD_C_Handle(char *StrC,unsigned char ReplyC);
void CMD_D_Handle(char *StrD,unsigned char ReplyD);
void CMD_E_Handle(char *StrE,unsigned char ReplyE);
void CMD_F_Handle(char *StrF,unsigned char ReplyF);
void CMD_H_Handle(char *StrH,unsigned char ReplyH);
void CMD_I_Handle(char *StrI,unsigned char ReplyI);
void CMD_J_Handle(char *StrJ,unsigned char ReplyJ);
void CMD_K_Handle(char *StrK,unsigned char ReplyK);
void CMD_G_Handle(char *StrG,unsigned char ReplyG);
void CMD_L_Handle(char *StrL,unsigned char ReplyL);
void CMD_M_Handle(char *StrM,unsigned char ReplyM);
void CMD_N_Handle(char *StrN,unsigned char ReplyN);
void CMD_O_Handle(char *StrO,unsigned char ReplyO);
void CMD_P_Handle(char *StrP,unsigned char ReplyP);
void CMD_R_Handle(char *SthR,unsigned char ReplyR);
void CMD_S_Handle(char *StrS,unsigned char ReplyS);
void CMD_Q_Handle(char *StrQ,unsigned char ReplyQ);
void CMD_U_Handle(char *StrU,unsigned char ReplyU);
void CMD_V_Handle(char *StrV,unsigned char ReplyV);
void CMD_W_Handle(char *StrW,unsigned char ReplyW);			// TIM add
void CMD_Z_Handle(char *StrZ,unsigned char ReplyZ);

extern void send_info(char *PC_CMD,char *SMS_CMD,u8 GPRS_CMD,u8 BackAndGPSType,u8 SMS_MSG_Type,bool Save);

#endif

