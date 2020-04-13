#ifndef __GSM_H
#define __GSM_H

#include "includes.h"

#define	CMD_A	0x01		// ָ��A
#define	CMD_A1	0x02		// ָ��A1
#define	CMD_A2	0x03		// ָ��A2
#define	CMD_A3	0x04		// ָ��A3
#define	CMD_A4	0x05		// ָ��A4
#define	CMD_A5	0x06		// ָ��A5
#define	CMD_A6	0x07		// ָ��A6
#define	CMD_A7	0x08		// ָ��A7
#define	CMD_A8	0x09		// ָ��A8
#define	CMD_B	0x0A		// ָ��B
#define	CMD_C	0x0B		// ָ��C
#define	CMD_D	0x0C		// ָ��D
#define	CMD_E	0x0D		// ָ��E
#define	CMD_F	0x0E		// ָ��F
#define CMD_F1	0x0F		// 
#define	CMD_G	0x10		// ָ��G
#define	CMD_H	0x11		// ָ��H
#define	CMD_I	0x12		// ָ��I
#define	CMD_J	0x13		// ָ��G
#define	CMD_K	0x14		// ָ��K
#define	CMD_K0	0x15		// ָ��K
#define	CMD_K1	0x16	 	// ָ��K
#define	CMD_K2	0x17		// ָ��K
#define	CMD_K3	0x18		// ָ��K
#define	CMD_L	0x19		// ָ��L
#define	CMD_L1	0x1A		// ָ��L
#define	CMD_L2	0x1B		// ָ��L
#define	CMD_L3	0x1C		// ָ��L
#define	CMD_L4	0x1D		// ָ��L
#define CMD_L5	0x1E		// ָ��L5
#define	CMD_M	0x1F		// ָ��M
#define	CMD_M2	0x20		// ָ��M2
#define CMD_M3	0x21		// ָ��M3
#define CMD_M4	0x22		// ָ��M4
#define	CMD_N	0x23		// ָ��N
#define	CMD_O	0x24		// ָ��O
#define	CMD_P	0x25		// ָ��P
#define	CMD_Q1	0x26		// ָ��Q1
#define	CMD_Q2	0x27		// ָ��Q2
#define	CMD_Q3	0x28		// ָ��Q3
#define CMD_Q4	0x29		// ָ��Q4
#define CMD_Q5	0x2A		// ָ��Q5
#define	CMD_R	0x2B		// ָ��R
#define	CMD_S	0x2C		// ָ��S
#define	CMD_T	0x2D		// ָ��T
#define	CMD_U	0x2E		// ָ��U
#define	CMD_U0	0x2F		// ָ��U0
#define	CMD_U1	0x30		// ָ��U1
#define	CMD_U2	0x31		// ָ��U2
#define	CMD_U3	0x32		// ָ��U3
#define	CMD_U4	0x33		// ָ��U4
#define	CMD_U5	0x34		// ָ��U5
#define	CMD_U6	0x35		// ָ��U6
#define	CMD_U7	0x36		// ָ��U7
#define	CMD_U8	0x37		// ָ��U8
#define	CMD_U9	0x38		// ָ��U9
#define	CMD_V	0x39		// ָ��V
#define	CMD_V3	0x3A		// ָ��V
#define	CMD_W	0x3B		// ָ��W
#define	CMD_X	0x3C		// ָ��X
#define	CMD_Y	0x3D		// ָ��Y
#define	CMD_Z1	0x3E		// ָ��Z
#define CMD_Z2	0x3F		// ָ��Z2

#define	ALM_A	0x81		// ����A
#define	ALM_B1	0x82		// ����B1
#define	ALM_B2	0x83		// ����B2
#define	ALM_B3	0x84		// ����B3
#define	ALM_B4	0x85		// ����B4
#define	ALM_B5	0x86		// ����B5
#define	ALM_C	0x87		// ����C
#define	ALM_D	0x88		// ����D
#define	ALM_E	0x89		// ����E
#define	ALM_F	0x8A		// ����F
#define	ALM_G	0x8B		// ����G
#define	ALM_H	0x8C		// ����H
#define	ALM_I	0x8D		// ����I
#define	ALM_J	0x8E		// ����G
#define	ALM_K	0x8F		// ����K
#define	ALM_L	0x90		// ����L
#define	ALM_M	0x91		// ����M
#define	ALM_N	0x92		// ����N
#define	ALM_O	0x93		// ����O
#define	ALM_P	0x94		// ����P
#define	ALM_Q	0x95		// ����Q
#define	ALM_R	0x96		// ����R
#define	ALM_S	0x97		// ����S
#define	ALM_T	0x98		// ����T
#define	ALM_U	0x99		// ����U
#define	ALM_V	0x9A		// ����V
#define	ALM_W	0x9B		// ����W
#define	ALM_X	0x9C		// ����X
#define	ALM_Y	0x9D		// ����Y
#define	ALM_Z	0x9E		// ����Z
	
#define	CMD_ERR	0xFE		// ����ָ��


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
		bool GPRS_flag:1;          		// 1: ����GPRS��Ч��־
		bool Track_Flag:1;         		// 1: �����������ٱ�־		
		bool ALM_OverSpeed_Flag:1; 		// 1: ���ٱ����򿪱�־
		bool ALM_LowBat_Flag:1;    		// 1: �͵籨���򿪱�־
		bool ALM_Circle_GEO_Flag:1;    	// 1: Բ�ĵ���Χ���򿪱�־
		bool SOS_CALL_ENABLE_Flag:1;	// 1: �Ƿ�������SOS�绰
		bool Any_CallIn_Flag:1;			// 1: ��������绰,0:�Ƚ���Ȩ����
		bool CallIn_ByMe_Flag:1;		// 1: ������������ͨ��һ�; 	0: �Զ�������Ȩ����
  }b;
  u8 w;
}GSM_A_def;
extern  GSM_A_def GSMA;

typedef union 
{
	struct
	{
		bool Callin_SpkMode_Flag:1;		// 1: ���ͨ��					0: ����
		bool Callout_LisMode_Flag:1;	// 1: �������					0: ͨ��          
		bool GPS_PowerOffMode_Flag:1;	// 1: ������GPS��Դ				0: ���Կ���GPS��Դ    
		bool GPRS_SaveMode_Flag:1;		// 1: GPRSʡ����ģʽ			0: ��ʱ�ϴ�����			
		bool GPS_StrongMode_Flag:1;		// 1: GPS��ǿģʽ,���ص�		0: ʡ��ģʽ
		bool ALM_Bat_Full_On_Flag:1;	// 1: ���������籨����־
		bool ALM_Bat_Full_Send_Flag:1;	// 1: �ѷ��ͳ������־
		bool RTC_Update_Flag:1;			// 1: �Ѹ��¹�RTCʱ��;			0: δ����RTCʱ��		
  }b;
  u8 w;
}GSM_B_def;
extern  GSM_B_def GSMB;

typedef union 
{
	struct
	{
		bool GPS_Tran_Mode_Flag:1;		// 1: GPSĬ�������������		0: ֻ�����GGA RMC
		bool DTMF_Det_Flag:1;			// 1: ��������������
		bool SMS_Check_psw_Flag:1;		// 1: ������Ҫ�ȶ�����
		bool RTC_Alarm_Flag:1;			// 1: �ѿ���RTC����
		u8 reserved:4;
	}b;
	u8 w;
}GSM_C_def;
extern  GSM_C_def GSMC;

typedef union 
{
	struct
	{
		bool CMDF_SOS_Flag:1;						// ��Ҫ����SOS������λ��Ϣ
		bool CMDF_BMAL_Flag:1;						// ��Ҫ����BMA���䱨����λ��Ϣ
		bool CMDF_BMAH_Flag:1;
		u8 reserved:5;
	}b;
	u8 w;
}GSM_D_def;
extern  GSM_D_def GSMD;

__packed typedef struct  
{	
	char	IMEI[16];           // IMEI��
	char	apn[30];            // APN
	char	TCP_UDP[4];			// TCP
	char	APN_username[23];   // APN�û���
	char	APN_password[12];   // APN����
	char	ip_addr[18];        // IP��ַ
	char	ip_port[6];         // IP�˿�
	char	domain_name[24];    // ����
	char	domain_port[6];     // �˿�
	u8		Track_Set_Time;     // ������λ���ʱ�䣨����ʱ�䵥λ��
	char	Track_Set_Unit;     // ������λʱ�䵥λ
	u8		TimeZone;           // ʱ��ʱ
	u8		set_speed;          // ������ֵ
	u8		set_soc;            // �͵���ֵ
	u16		heartbeatset;      	// �����������ʱ��
	u8		geo_radius;         // Բ�ĵ���Χ���뾶
	float	center_lat;        	// Բ�ĵ���Χ������γ��
	float	center_lot;        	// Բ�ĵ���Χ�����ľ���
	u32		send_history;       // �ѷ�����ʷ��������	
	u32		store_history;      // �Ѵ�����ʷ��������
	u8		spk_volume;	     	// ͨ������
	u8		lowg;				// ���䷧ֵ
	u8		higg;				// ��ײ��ֵ
	u8		ATE0_Fail_Cnt;		// GSM ATEO�Զ������ʲ���ʧ�ܴ���
	u8		RTC_Alarm_Hour;		// ����ʱ
	u8		RTC_Alarm_Min;		// �����
	char	cmd_key[7];         // ָ�����룬��ʼֵ��123456
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
		bool Chk_CMGL_Flag:1;						// �������δ��ȡ���Ķ����б�
		bool Chk_CMGS_Flag:1;						// ��鷢�Ͷ������ݽ��	
		bool CMDD_SMS_Flag:1;						// Dָ��ظ�����
		bool Track_SMS_Flag:1;						// ִ��������λ���͵��ֻ���ʱ	
		bool New_SMS_Flag:1;						// �յ��¶��ű�־
		u8  reserved:3;
	}b;
	u8 w;
}GSM_sms_def;
extern  GSM_sms_def SMS;

typedef union 
{
	struct
	{
		bool Ring_Flag:1;							// �����������־
		bool Ring_Cnt_Flag:1;						// �������������ʱ	
		bool Calling_Flag:1;						// ����ͨ����־	
		bool Turn_Vol_En_Flag:1;					// ����ͨ�������е���������־;
		bool Lis_To_Spk_En_Flag:1;					// ��ͨ�������н������л�Ϊͨ��	
		bool SOS_HangUp_En_Flag:1;					// ����SOS��ͨ�������йҶϱ�־; 				
		bool CallInPhone_Save_Flag:1;				// �ѱ����������	
		bool Play_Ring_Flag:1;						// ��������������Ƶ
		bool Play_Audio_Flag:1;						// ������Ƶ
		bool Dtmf_Flag:1;							// ����dtmf���
		u16  reserved:6;
	}b;
	u16 w;
}GSM_call_def;
extern GSM_call_def CALL;

typedef union 
{
	struct
	{	
		bool Connected_OK_Flag:1;					// GPRS����OK
		bool Connected_Now_Flag:1;
		
		bool Chk_Notif_Flag:1;				   		// ����GPRS��,�������ڻ�ȡ��ģ���־
		
		bool Chk_Connected_Flag:1;					// ͨ�����ڼ������״̬	
		bool Chk_Ciprxget_Flag:1;					// ����GPRS��ȡƽ̨����������
		
		bool Track_GPRS_Flag:1;						// ִ��������λ���͵�GPRS��ʱ
				
		bool Reconnect_Cnt_Flag:1;					// ����GPRS������ʱ������	
		
		bool OTA_Buf_Flag:1;						// OTA���յ�DMA������	
		bool OTA_NOW_Flag:1;						// ����OTA	
		bool OTA_Get_Head_Flag:1;					// OTA��ȡ����ͷ
		
		bool GPRS_send_history_Flag:1;				// ���ڷ�����ʷ����
		
		bool GPRS_Save_now_Flag:1;					// �ѹ�����ʡ��ģʽ
		
		bool WIFI_Tcp_Connected_Flag:1;				// TIM-�Ƿ�ͨ��wifi����tcp����ƽ̨	
		
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

extern unsigned char Ring_Clip_Count;// �����������

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

