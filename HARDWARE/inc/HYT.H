#ifndef __HYT_H
#define __HYT_H

/*�ͻ�*/
#define DEBUG				0	// �����������Գ���

#define	Sos_call_num		8	// sos�����ֻ�����
#define	Sos_cycle_num		20  // sosѭ���������

/*ģ��ѡ��*/
#define MODULE_UG96			0	// UG96		+	MAX9860
#define MODULE_SIM7500		1	// SIM7500	+	MAX9860

/*��Ҫ����ѡ��*/
#define CALLIN_ENABLE		1	// �Ƿ����绰�������
#define CALLOUT_ENABLE		1	// �Ƿ����绰�������
#define SMS_ENABLE			1	// �Ƿ������Ź���
#define GPRS_ENABLE			1	// �Ƿ���GPRS����
#define OTA_F				1	// �Ƿ���OTA��������
#define GEO_FUN1			1	// �Ƿ�ͨ���ε���Χ������
#define GEO_FUN2			1	// �Ƿ�ͨԲ�ĵ���Χ������
#define WATCHDOG_ON			1	// �Ƿ������Ź�
#define Bat_Full_ON			1	// �Ƿ��������籨
#define Bat_Low_ON			1	// �Ƿ����͵籨��
#define RTC_Alarm			0	// �Ƿ�������
#define GSM_Positon			0	// �Ƿ�����վ��λ
#define Audio_Enable		1	// �Ƿ�����������
#define Step_Enable			1	// �Ƿ����Ʋ�����
#define Wifi_Enable			1	// �Ƿ���WIFI��λ

/*ָ���*/
#define A_FUN				1    //���á�ɾ������
#define B_FUN				1    //��ȡIMEI��
#define C_FUN				1    //����APN���������
#define D_FUN				1    //����GPRS
#define E_FUN				1    //�ر�GPRS
#define F_FUN				1    //���ζ�λ
#define G_FUN				1    //��ȡ����
#define H_FUN				1    //����ָ������
#define I_FUN				1    //���þ��ε���Χ��
#define J_FUN				1    //���ó��ٱ���
#define K_FUN				1    //�������ȱ���10S(Զ�̹رյ�Դ)
#define L_FUN				1    //����ʱ��
#define M_FUN				1    //���ó�������
#define N_FUN				1    //���õ͵籨��
#define O_FUN				1    //�����û�������
#define P_FUN				1    //����Բ�ĵ���Χ��
#define Q_FUN				1    //����SOS������
#define R_FUN				1    //��������
#define S_FUN				0    //������������������תʱ�䣨�����������룩
#define T_FUN				0    //�Ͽ�ACC���ָ�ACC
#define U_FUN				1    //���ü�����ͨ����GPS����ģʽ��
#define V_FUN				1    //���������̵�������ʱ�䡢��������
#define W_FUN				1    // TIM-����Ŀ��wifi:ssid,key
#define X_FUN				0    //�����űߵ�ƽ��IO���ܣ�ѡ��GPS\MCU_Fģ����������
#define Y_FUN				0
#define Z_FUN				1    //����LOG����

/*��������*/
#define RESET_FUN      		1 	//�����ָ���������
#define GPRS_FUN      		1 	//��������GPRS���Թ���
#define password_check 		1	//�Ƿ񿪻����������Ч��
#define volume_control 		1 	//������������������
#define CIPRXGET_N     		1 	//�Ƿ��ȡ����©����GPRS���� 
#define BMA250_ALARM   		1 	//�Ƿ��BMA250����
#define COMPARE_PASWD  		1 	//�Ƿ�Ƚ�ָ�����룬��Ƚ�123456

/*SPIFLASH �洢��ַ*/
#define flags_addr			0		// ���ñ�־λ�����ַ
#define flags_size			10		// �豣��ı�־��ĳ���

#define GSM_paraddr			10		// MCU_F���ò��������ַ
#define GSM_para_size		176		// ����MCU_F���ò����ĳ���

#define phoneaddr			250		// ��Ȩ���������ʼ��ַ
#define	GSM_ph_size			192 	// ������Ȩ����ĳ���

#define gpsadd				500		// ��һ����Ч���ݴ�����ʼ��ַ
#define gpsx_All_Size		37		// ������һ��������Ч���ݵĳ���,����CBC,CSQ,CREG,COPS

#define geoaddr				550		// ����GEO���������ʼ��ַ
#define GEO_c_size			80		// �������GEO����ĳ���

#define phonenameaddr		650		// ��Ȩ��������
#define phonenamesize		104

#define setgeoaddr			800		// �û�����GEO��ʽ������ʼ��ַ
#define setgeo_size			38  	// �����û�����GEO�ĳ���

#define otaaddr				1000	// OTA����������ʼ��ַ
#define GSM_OTA_size		24		// ����OTA�����ĳ���

#define SPI_StartAddr_New	0x10000	// �³��򴢴���ʼ��ַ
#define hisdataaddr			0x90000	// ��ʷ���ݴ�����ʼ��ַ

#endif