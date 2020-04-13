#ifndef __OTA_H
#define __OTA_H

#include "string.h"
#include "stdbool.h"
#include "type1.h"

__packed typedef struct  
{
	unsigned char  		OTA_state;      // OTA����״̬	
	char 								program_ver[5];	// OTA�����汾��
	unsigned char  		OTA_errcount;   // OTAʧ�ܴ���
	unsigned char  		mcu_to_spi_bk;  // MCU���ݵ�SPI״̬
	char								Device[15];		// ��������
	unsigned char			OTA_ph;
}GSM_OTA_def; 

extern GSM_OTA_def 		GSM_OTA;

#if MODULE_SIM7500
extern void Http_Disconnect(void);
#endif
extern void OTA(void);
extern bool OTA_Store(void);
extern void Readly_For_OTA(unsigned char usart);

#endif

