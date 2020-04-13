#ifndef __eeprom1_H
#define __eeprom1_H

/***************************************** EEPROM related **************************************************/
/*
* �����֣�SUB_ADDR1
* ��  ������ʹ�õ�E2prom�������ӵ�ַ����A2~A0����ȷ����ֵ
*/
#define SUB1_ADDR           0x0
/*
* �����֣�DEV1_ADDR
* ��  ����E2prom��I2CЭ���е����������ַ��ͨ��������������
*/
#define DEV1_ADDR            0x50
/*
* �����֣�CHIP1_SIZE_Kbits
* ��  ������KbitsΪ��λ��E2prom��������������24CXXϵ���ͺ��е�XX
*/
#define CHIP1_SIZE_Kbits      512
/*
* �����֣�PAGE1_SIZE_Bytes
* ��  ������BytesΪ��λ��E2prom������ҳ����������������Datasheet�鵽
*/
#define PAGE1_SIZE_Bytes      128
/*
* �����֣�ACCESS1_DELAY_ms
* ��  ���������ʵȴ���ʱʱ�䣬��ҳдʱ���������ȡ��ֵ��2��
*/
#define ACCESS1_DELAY_ms      20

#define	IIC1_PORT			GPIOC
#define	IIC1_SCL			GPIO_Pin_11	//C11
#define	IIC1_SDA			GPIO_Pin_2	//D2
#define	IIC1_WP				GPIO_Pin_11	//A11

#define IIC1_SCL_1()		GPIOC->BSRR = IIC1_SCL;						/* SCL = 1 */
#define IIC1_SCL_0()  		GPIOC->BRR = IIC1_SCL;						/* SCL = 0 */
#define IIC1_SDA_1()  		GPIOD->BSRR = IIC1_SDA;						/* SDA = 1 */
#define IIC1_SDA_0()  		GPIOD->BRR = IIC1_SDA;						/* SDA = 0 */
#define IIC1_WP_1()			GPIOA->BSRR = IIC1_WP;						/* WP = 1 */
#define IIC1_WP_0()			GPIOA->BRR = IIC1_WP;						/* WP = 0 */
#define IIC1_SCL_READ()  	((GPIOC->IDR & IIC1_SCL) ? 1 : 0)			/* ��SCL����״̬ */
#define IIC1_SDA_READ()  	((GPIOD->IDR & IIC1_SDA) ? 1 : 0)			/* ��SDA����״̬ */

#define IIC1_WR				0											/* д����bit */
#define IIC1_RD				1											/* ������bit */

void IIC1_Configuration(void);
int  IIC1_BufferWrite(u8* pBuffer, u16 WriteAddr, u16 NumByteToWrite);
int  IIC1_BufferRead(u8* pBuffer, u16 ReadAddr, u16 NumByteToRead);
int  IIC1_WriteU32(u16 WriteAddr, uint32_t dat);
uint32_t IIC1_ReadU32(u16 ReadAddr);
extern int IIC1_ERASE(u16 StartAddr, uint16_t Numofbytes);
void IIC1_Delay(void);

#endif
