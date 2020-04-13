#ifndef __eeprom1_H
#define __eeprom1_H

/***************************************** EEPROM related **************************************************/
/*
* 配置字：SUB_ADDR1
* 描  述：所使用的E2prom器件的子地址，即A2~A0引脚确定的值
*/
#define SUB1_ADDR           0x0
/*
* 配置字：DEV1_ADDR
* 描  述：E2prom在I2C协议中的器件分类地址，通常情况下无需更改
*/
#define DEV1_ADDR            0x50
/*
* 配置字：CHIP1_SIZE_Kbits
* 描  述：以Kbits为单位的E2prom器件的容量，即24CXX系列型号中的XX
*/
#define CHIP1_SIZE_Kbits      512
/*
* 配置字：PAGE1_SIZE_Bytes
* 描  述：以Bytes为单位的E2prom器件的页容量，可由器件的Datasheet查到
*/
#define PAGE1_SIZE_Bytes      128
/*
* 配置字：ACCESS1_DELAY_ms
* 描  述：最大访问等待延时时间，由页写时间决定，可取其值的2倍
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
#define IIC1_SCL_READ()  	((GPIOC->IDR & IIC1_SCL) ? 1 : 0)			/* 读SCL口线状态 */
#define IIC1_SDA_READ()  	((GPIOD->IDR & IIC1_SDA) ? 1 : 0)			/* 读SDA口线状态 */

#define IIC1_WR				0											/* 写控制bit */
#define IIC1_RD				1											/* 读控制bit */

void IIC1_Configuration(void);
int  IIC1_BufferWrite(u8* pBuffer, u16 WriteAddr, u16 NumByteToWrite);
int  IIC1_BufferRead(u8* pBuffer, u16 ReadAddr, u16 NumByteToRead);
int  IIC1_WriteU32(u16 WriteAddr, uint32_t dat);
uint32_t IIC1_ReadU32(u16 ReadAddr);
extern int IIC1_ERASE(u16 StartAddr, uint16_t Numofbytes);
void IIC1_Delay(void);

#endif
