#ifndef _MAINCONTROL_H
#define _MAINCONTROL_H

#include "stdbool.h"
#include "type1.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#if MODULE_SIM7500

#define BIBI	80

#elif MODULE_UG96

#define BIBI	100

#endif

extern char			 *HS_VER;												// ����Ӳ������汾��

extern unsigned char Bat_Val[3];	
extern unsigned char Bat_Cnt;

#define Queue_SPI_Num   10   												// ���е����� 
#define Queue_SPI_Len  	15  												// ���еĳ���

#define Queue_GSM_Num   20   												// ���е����� 
#define Queue_GSM_Len  	15  												// ���еĳ���

extern SemaphoreHandle_t			Semap_Gps;
extern SemaphoreHandle_t			Semap_PcCmd;

extern QueueHandle_t				Queue_GSM;								// ���о��
extern QueueHandle_t				Queue_SPI;								// ���о��
	
#define MCU_OTA_Flag					(1<<0)								// ����MCU׼��OTA	

#define Bat_Charge_Full_Flag 			(1<<1)								// ��ʵ���������־
#define Bat_Charging_Flag	 			(1<<2)								// ��ʵ������б�־

#define Key_Power_S_Flag				(1<<3)								// ͨ��������PWR�̰� 
#define Key_SOS_S_Flag					(1<<4)								// ͨ��������SOS��Ч�̰��±�־; 	
#define Key_SOS_L_Flag					(1<<5)								// SOS������Ч������
#define Key_Call1_S_Flag				(1<<6)
#define Key_Call1_L_Flag				(1<<7)
#define Key_Call2_S_Flag				(1<<8)
#define Key_Call2_L_Flag				(1<<9)								// ѡ��绰����ʱ�����˳�
#define BMA250_Int_Flag					(1<<10)								// BMA250���ж�

#define USB_Connected_Flag				(1<<11)								// USB����״̬
#define USB_Disconnected_Flag			(1<<12)								// USB�Ͽ�

#define MCU_RTC_Alarm_Flag				(1<<13)								// RTC�����¼�

#define Event_Key_All					(Key_Power_S_Flag|Key_SOS_S_Flag|Key_SOS_L_Flag|Key_Call1_S_Flag|Key_Call1_L_Flag|Key_Call2_S_Flag|Key_Call2_L_Flag)
#define EVENT_BIT_MAIN					(MCU_OTA_Flag|Bat_Charge_Full_Flag|BMA250_Int_Flag|Key_SOS_L_Flag|Key_Call1_L_Flag|Key_Call2_L_Flag|MCU_RTC_Alarm_Flag)

extern EventGroupHandle_t				EventGroupHandler_MCU;				// �¼���־����


#define CMGS_Send_OK_Flag				(1<<0)								// �Ѽ�鵽���ŷ��ͳɹ�								
#define CMGS_Send_NG_Flag				(1<<1)								// �Ѽ�鵽���ŷ���ʧ��
#define CMGL_OK_Flag					(1<<2)								// �Ѽ��������δ��ȡ���Ķ����б�
#define AT_Cnt_OK_Flag					(1<<3)								// �Ѽ����MCU_F���ڽ��յ��ַ�����OK
#define Gprs_Send_OK_Flag				(1<<4)						       	// ƽ̨�ѳɹ��������ݱ�־	
#define Gprs_Send_NG_Flag				(1<<5)								// ƽ̨����ʧ��
#define Gprs_Connected_OK_Flag			(1<<6)						    	// GPRS������;	
#define Gprs_Connected_NG_Flag			(1<<7)
#define DTMF_OK_Flag					(1<<8)								// �������OK
#define Audio_Finish_Flag				(1<<9)								// �������
#define Gprs_OTA_DMA_Flag				(1<<10)								// ���յ�OTA����	
#define Gps_GPRMC_GGA_OK_Flag			(1<<11)								// ���յ�RMC GGA����
#define Wifi_Finish_Flag				(1<<12)								// ����WIFI�������
#define BUSY_wait_Flag					(1<<13)

//#define WIFI_Connetced_Flag             (1<<14)								//TIM-���������������־�����ڽ���TCP��������

#define Event_Gsm_All					(CMGS_Send_OK_Flag|CMGS_Send_NG_Flag|CMGL_OK_Flag|AT_Cnt_OK_Flag|Gprs_Send_OK_Flag|Gprs_Connected_OK_Flag|Gprs_OTA_DMA_Flag)

extern EventGroupHandle_t				EventGroupHandler_OTH;				// �¼���־����


typedef enum 
{
	LOW = 0,		
	HIGH = !LOW
}HighLowState;

typedef enum 
{
	OFF= 0,		
	ON = !OFF
}OnOffState;

typedef union 
{
	struct
	{
		bool PC_MCU_Flag:1;			// 1: ����PC����MCU�����ݻ��Թ���
		bool GPS_MCU_Flag:1;		// 1: ����GPS���͵�MCU�����ݻ��Թ���
		bool MCU_GSM_Flag:1;		// 1: ����MCU����MCU_F�����ݻ��Ե�PC��
		bool GSM_MCU_Flag:1;		// 1: ����MCU_F����MCU�����ݻ��Ե�PC��
		bool MCU_WIFI_Flag:1;		
		bool WIFI_MCU_Flag:1;
		bool BYPASS_GSM_Flag:1;		// 1: ����PC����ֱ��ת��MCU_F����
		bool BYPASS_WIFI_Flag:1;	// 1: ����PC����ֱ�ӵ�WIFI
	}b;
	u8 w;
}ECHO_Para_Def;
extern  ECHO_Para_Def ECHO_F;


__packed typedef struct  
{
	unsigned char Chk_Bat_Charge_Cnt;			// �����״̬
	unsigned char Chk_Bat_Full_Cnt;				// ��������״̬
	unsigned char Chk_USB_Connect_Cnt;			// ���USB
	unsigned char Motor_Off_Cnt;
	unsigned char Led_pwr_cnt;
	unsigned char Led_gsm_cnt;
	unsigned char Led_net_cnt;
	unsigned char BMA_Low_Cnt;
}cnt_1ms_def;
extern cnt_1ms_def cnt_1ms;

__packed typedef struct  
{
	unsigned char _1S_Cnt;						// 1���ʱ
	unsigned char WTD_Cnt;						// ���Ź���ʱ��

	unsigned char BiBi_Cnt;						// ����������					
	unsigned char Ring_Time;   					// ����ʱ��
	unsigned char HeartBeat_Cnt;				// ������ʱ	
	unsigned char SOC_CSQ_Cnt;					// SOC��CSQ��ѯ��ʱ
	unsigned char Chk_GPRS_Cnt;					// ������ѯGPRSƽ̨����
	unsigned char Track_SOC_CSQ_Cnt;			// ������λʱ,ÿ��60���ȡһ��CSQ,CBC��
	unsigned char Track_wifi_Cnt;
	
	unsigned short int CMD_F_Count;				// CMD-Fָ���ȡ������Ч�����GPS���ʱ�����
	unsigned short int Chk_GEO1_Cnt;
	unsigned short int Chk_GEO2_Cnt;
	unsigned short int Chk_GEO3_Cnt;
	unsigned short int Chk_GEO4_Cnt;
	unsigned short int Chk_GEO5_Cnt;
	unsigned short int Chk_GEO6_Cnt;

	unsigned short int Chk_SMS_Ten_Cnt;			// ��ʱ��ȡ����
	unsigned short int Chk_GPS_Ten_Cnt;			// ��ʱʮ���Ӷ�ȡһ��GPS
	unsigned short int GPRS_Reconnect_Cnt;		// gprs����ʱ�������	
	unsigned short int GPRS_Reconnect_Set;		// gprs����ʱ���趨ֵ
	unsigned short int GPS_Time_Update_Cnt;		// GPSʱ�䶨ʱ���¼�ʱ
	
	unsigned int Track_Save_Cnt;				// GPRS����ʡ��ģʽ��ʱ
	unsigned int Track_Time_Cnt;				// �����������������λ��ʱ 
	unsigned int Track_Time_Set;				// ������ʱҪ��ʱ��	
}cnt_1s_def; 
extern cnt_1s_def cnt_1s;

typedef union 
{
	struct
	{
		bool Chk_Key_Flag:1;					// ��Ҫִ��ɨ����KEY		
		bool Chk_Bat_Charing_Flag:1;			// ִ��ɨ������
		bool Chk_Bat_Charge_Full_Flag:1;		// ִ��ɨ�������
		bool Chk_USB_Connect_Flag:1;			// ִ��ɨ��USB����
		bool Chk_USB_Disconnect_Flag:1;			// ִ��ɨ��USB�Ͽ�
		u8  reserved:3;
	}b;
	u8 w;
}Def_Flag_100mS;
extern  Def_Flag_100mS Tim_F;

typedef union 
{
	struct
	{	
		bool GSM_Init_Flag:1;						 // MCU_F��ʼ����ʼ		
		bool No_Simcard_Flag:1;					 // δ��⵽SIM CARD
		bool Imei_Err_Flag:1;						 // MCU_F IMEI��ȡ����
		bool Chk_AT_Cnt_Flag:1;					 // ���MCU_F���ڽ��յ��ַ�����	
		bool W25_wakeup_Flag:1;					 // W25����˯��
		bool Uart1_to_Luart1:1;					 // ��������ת������
		bool Power_Connected_Flag:1;		 // ���ӵ�Դ
		bool First_Read_CBC_Flag:1;			 // TIM
		bool BMA_Low_Flag:1;             // TIM
		bool Charging_Flag:1;            // TIM
		bool Wifi_mac_Flag:1;            // TIM-�Ƿ��ȡMAC��ַ
		bool Wifi_mac_str_Flag:1;        // TIM-�Ƿ����ڽ���һ��MAC��ַ
		bool USB_OTA_Flag:1;             // TIM
		bool Power_Off_Flag:1;           // TIM
		bool GSM_wakeup_Flag:1;					 // TIM
		
		bool WIFI_Search_Flag:1;				 // TIM-�Ƿ�ɨ�赽Ŀ��WIFI
		bool WIFI_Connected_Flag:1;			 // TIM-�Ƿ������ӵ�WIFI
		u32  reserved:15;
	}b;
	u32 w;
}MCU_F_def;
extern  MCU_F_def MCU_F;

extern u8  Track_Time_Unit;

extern unsigned char InValid_GPS_Count;

extern int fputc(int ch, FILE *f);
extern void USART1_SendData(char dat);
extern void USART2_SendData(char dat);
extern void LUSART1_SendData(char dat);
extern void USART4_SendData(char dat);

extern void initialize_all(unsigned char Ota);
extern void Read_Flash_Para(void);
extern void MCU_Sleep(void);
extern unsigned char adc_get_soc(void);

#if Audio_Enable	
extern void Audio_play(unsigned char num,bool wait);
#endif

#endif



