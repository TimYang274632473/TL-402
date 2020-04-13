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

extern char			 *HS_VER;												// 定义硬件软件版本号

extern unsigned char Bat_Val[3];	
extern unsigned char Bat_Cnt;

#define Queue_SPI_Num   10   												// 队列的数量 
#define Queue_SPI_Len  	15  												// 队列的长度

#define Queue_GSM_Num   20   												// 队列的数量 
#define Queue_GSM_Len  	15  												// 队列的长度

extern SemaphoreHandle_t			Semap_Gps;
extern SemaphoreHandle_t			Semap_PcCmd;

extern QueueHandle_t				Queue_GSM;								// 队列句柄
extern QueueHandle_t				Queue_SPI;								// 队列句柄
	
#define MCU_OTA_Flag					(1<<0)								// 机器MCU准备OTA	

#define Bat_Charge_Full_Flag 			(1<<1)								// 核实到充满电标志
#define Bat_Charging_Flag	 			(1<<2)								// 核实到充电中标志

#define Key_Power_S_Flag				(1<<3)								// 通话过程中PWR短按 
#define Key_SOS_S_Flag					(1<<4)								// 通话过程中SOS有效短按下标志; 	
#define Key_SOS_L_Flag					(1<<5)								// SOS按键有效长按下
#define Key_Call1_S_Flag				(1<<6)
#define Key_Call1_L_Flag				(1<<7)
#define Key_Call2_S_Flag				(1<<8)
#define Key_Call2_L_Flag				(1<<9)								// 选择电话号码时长按退出
#define BMA250_Int_Flag					(1<<10)								// BMA250有中断

#define USB_Connected_Flag				(1<<11)								// USB连接状态
#define USB_Disconnected_Flag			(1<<12)								// USB断开

#define MCU_RTC_Alarm_Flag				(1<<13)								// RTC闹铃事件

#define Event_Key_All					(Key_Power_S_Flag|Key_SOS_S_Flag|Key_SOS_L_Flag|Key_Call1_S_Flag|Key_Call1_L_Flag|Key_Call2_S_Flag|Key_Call2_L_Flag)
#define EVENT_BIT_MAIN					(MCU_OTA_Flag|Bat_Charge_Full_Flag|BMA250_Int_Flag|Key_SOS_L_Flag|Key_Call1_L_Flag|Key_Call2_L_Flag|MCU_RTC_Alarm_Flag)

extern EventGroupHandle_t				EventGroupHandler_MCU;				// 事件标志组句柄


#define CMGS_Send_OK_Flag				(1<<0)								// 已检查到短信发送成功								
#define CMGS_Send_NG_Flag				(1<<1)								// 已检查到短信发送失败
#define CMGL_OK_Flag					(1<<2)								// 已检查完所有未读取过的短信列表
#define AT_Cnt_OK_Flag					(1<<3)								// 已检查完MCU_F串口接收到字符个数OK
#define Gprs_Send_OK_Flag				(1<<4)						       	// 平台已成功发送数据标志	
#define Gprs_Send_NG_Flag				(1<<5)								// 平台发送失败
#define Gprs_Connected_OK_Flag			(1<<6)						    	// GPRS已连接;	
#define Gprs_Connected_NG_Flag			(1<<7)
#define DTMF_OK_Flag					(1<<8)								// 按键检测OK
#define Audio_Finish_Flag				(1<<9)								// 播放完成
#define Gprs_OTA_DMA_Flag				(1<<10)								// 接收到OTA数据	
#define Gps_GPRMC_GGA_OK_Flag			(1<<11)								// 接收到RMC GGA数据
#define Wifi_Finish_Flag				(1<<12)								// 接收WIFI数据完成
#define BUSY_wait_Flag					(1<<13)

//#define WIFI_Connetced_Flag             (1<<14)								//TIM-连接上无线网络标志，用于建立TCP连接任务

#define Event_Gsm_All					(CMGS_Send_OK_Flag|CMGS_Send_NG_Flag|CMGL_OK_Flag|AT_Cnt_OK_Flag|Gprs_Send_OK_Flag|Gprs_Connected_OK_Flag|Gprs_OTA_DMA_Flag)

extern EventGroupHandle_t				EventGroupHandler_OTH;				// 事件标志组句柄


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
		bool PC_MCU_Flag:1;			// 1: 开启PC发送MCU的数据回显功能
		bool GPS_MCU_Flag:1;		// 1: 开启GPS发送到MCU的数据回显功能
		bool MCU_GSM_Flag:1;		// 1: 开启MCU发送MCU_F的数据回显到PC端
		bool GSM_MCU_Flag:1;		// 1: 开户MCU_F发送MCU的数据回显到PC端
		bool MCU_WIFI_Flag:1;		
		bool WIFI_MCU_Flag:1;
		bool BYPASS_GSM_Flag:1;		// 1: 开启PC数据直接转到MCU_F功能
		bool BYPASS_WIFI_Flag:1;	// 1: 开启PC数据直接到WIFI
	}b;
	u8 w;
}ECHO_Para_Def;
extern  ECHO_Para_Def ECHO_F;


__packed typedef struct  
{
	unsigned char Chk_Bat_Charge_Cnt;			// 检查充电状态
	unsigned char Chk_Bat_Full_Cnt;				// 检查充满电状态
	unsigned char Chk_USB_Connect_Cnt;			// 检查USB
	unsigned char Motor_Off_Cnt;
	unsigned char Led_pwr_cnt;
	unsigned char Led_gsm_cnt;
	unsigned char Led_net_cnt;
	unsigned char BMA_Low_Cnt;
}cnt_1ms_def;
extern cnt_1ms_def cnt_1ms;

__packed typedef struct  
{
	unsigned char _1S_Cnt;						// 1秒计时
	unsigned char WTD_Cnt;						// 看门狗定时器

	unsigned char BiBi_Cnt;						// 蜂鸣声计数					
	unsigned char Ring_Time;   					// 响铃时间
	unsigned char HeartBeat_Cnt;				// 心跳计时	
	unsigned char SOC_CSQ_Cnt;					// SOC和CSQ查询计时
	unsigned char Chk_GPRS_Cnt;					// 主动查询GPRS平台数据
	unsigned char Track_SOC_CSQ_Cnt;			// 连续定位时,每隔60秒读取一次CSQ,CBC等
	unsigned char Track_wifi_Cnt;
	
	unsigned short int CMD_F_Count;				// CMD-F指令获取数据无效后访问GPS间隔时间计数
	unsigned short int Chk_GEO1_Cnt;
	unsigned short int Chk_GEO2_Cnt;
	unsigned short int Chk_GEO3_Cnt;
	unsigned short int Chk_GEO4_Cnt;
	unsigned short int Chk_GEO5_Cnt;
	unsigned short int Chk_GEO6_Cnt;

	unsigned short int Chk_SMS_Ten_Cnt;			// 定时读取短信
	unsigned short int Chk_GPS_Ten_Cnt;			// 定时十分钟读取一次GPS
	unsigned short int GPRS_Reconnect_Cnt;		// gprs重连时间秒计数	
	unsigned short int GPRS_Reconnect_Set;		// gprs重联时间设定值
	unsigned short int GPS_Time_Update_Cnt;		// GPS时间定时更新计时
	
	unsigned int Track_Save_Cnt;				// GPRS进入省电模式计时
	unsigned int Track_Time_Cnt;				// 秒计数，用于连续定位计时 
	unsigned int Track_Time_Set;				// 连续定时要求时长	
}cnt_1s_def; 
extern cnt_1s_def cnt_1s;

typedef union 
{
	struct
	{
		bool Chk_Key_Flag:1;					// 需要执行扫描仪KEY		
		bool Chk_Bat_Charing_Flag:1;			// 执行扫描充电中
		bool Chk_Bat_Charge_Full_Flag:1;		// 执行扫描充满电
		bool Chk_USB_Connect_Flag:1;			// 执行扫描USB连接
		bool Chk_USB_Disconnect_Flag:1;			// 执行扫描USB断开
		u8  reserved:3;
	}b;
	u8 w;
}Def_Flag_100mS;
extern  Def_Flag_100mS Tim_F;

typedef union 
{
	struct
	{	
		bool GSM_Init_Flag:1;						 // MCU_F初始化开始		
		bool No_Simcard_Flag:1;					 // 未检测到SIM CARD
		bool Imei_Err_Flag:1;						 // MCU_F IMEI读取出错
		bool Chk_AT_Cnt_Flag:1;					 // 检查MCU_F串口接收到字符个数	
		bool W25_wakeup_Flag:1;					 // W25进入睡眠
		bool Uart1_to_Luart1:1;					 // 开启数据转发功能
		bool Power_Connected_Flag:1;		 // 连接电源
		bool First_Read_CBC_Flag:1;			 // TIM
		bool BMA_Low_Flag:1;             // TIM
		bool Charging_Flag:1;            // TIM
		bool Wifi_mac_Flag:1;            // TIM-是否获取MAC地址
		bool Wifi_mac_str_Flag:1;        // TIM-是否正在接收一组MAC地址
		bool USB_OTA_Flag:1;             // TIM
		bool Power_Off_Flag:1;           // TIM
		bool GSM_wakeup_Flag:1;					 // TIM
		
		bool WIFI_Search_Flag:1;				 // TIM-是否扫描到目标WIFI
		bool WIFI_Connected_Flag:1;			 // TIM-是否已连接到WIFI
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



