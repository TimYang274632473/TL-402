#ifndef __DS18B20_H
#define __DS18B20_H 
#include "gpio.h"
#include "type1.h"
extern bool ds18b_err_flag;
extern bool delay_us_flag;

extern u32 _1us;

#define DS18B20_IO_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=0X00008000;}
#define DS18B20_IO_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=0X00003000;}

////IO��������											   

#define	DS18B20_DQ_OUT PBout(11) //���ݶ˿�	PB11    
#define	DS18B20_DQ_IN  PBin(11)  //���ݶ˿�	PB11

void delay_us(u32 nus); 	
u8 DS18B20_Init(void);//��ʼ��DS18B20
short DS18B20_Get_Temp(void);//��ȡ�¶�
void DS18B20_Start(void);//��ʼ�¶�ת��
void DS18B20_Write_Byte(u8 dat);//д��һ���ֽ�
u8 DS18B20_Read_Byte(void);//����һ���ֽ�
u8 DS18B20_Read_Bit(void);//����һ��λ
u8 DS18B20_Check(void);//����Ƿ����DS18B20
void DS18B20_Rst(void);//��λDS18B20    
#endif















