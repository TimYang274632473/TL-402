#ifndef __WIFI_H
#define __WIFI_H

#include  <stdbool.h>

#define wifi_rx_len 	20
#define wifi_row_len	3
#define wifi_low_len	60

// TIM add
__packed typedef struct  
{
	char wifi_ssid[20];
	char wifi_key [20];
}WIFI_set_msg; 

extern WIFI_set_msg WIFI;


extern unsigned char wifi_rx_cnt;
extern unsigned char wifi_row_cnt;
extern unsigned char wifi_low_cnt;

extern char wifi_rx_buf[wifi_rx_len];
extern char wifi_mac[wifi_row_len][wifi_low_len];

extern void WIFI_on(void);
extern void WIFI_off(void);
extern bool Wifi_Check_mac(bool Force);

// TIM-add
extern bool WIFI_Search(void);						//Ѱ��Ŀ��wifi���ҵ�����true
extern bool WIFI_Connect_Router(void);				//�ն��豸���ӿͻ�·����
extern bool WIFI_Tcp_Connect(void);					//����TCP����
extern void WIFI_Msg_Cmp(char (*p)[wifi_low_len]);	//��ȡ�û�wifi������Ϣ	  
extern void WIFI_Generat_Str(void);					//����wifi�����ַ���
#endif

