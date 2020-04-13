#ifndef _iic_H
#define _iic_H 

#define IIC_WR_BIT					0			/* Ð´¿ØÖÆbit 		*/
#define IIC_RD_BIT					1			/* ¶Á¿ØÖÆbit 		*/
					 
#define IIC_SCL_H()  				HAL_GPIO_WritePin(IIC_SCL_GPIO, IIC_SCL_Pin, GPIO_PIN_SET)	 
#define IIC_SCL_L()  				HAL_GPIO_WritePin(IIC_SCL_GPIO, IIC_SCL_Pin, GPIO_PIN_RESET);	
							 										 
#define IIC_SDA_H()  				HAL_GPIO_WritePin(IIC_SDA_GPIO, IIC_SDA_Pin, GPIO_PIN_SET)	 
#define IIC_SDA_L()  				HAL_GPIO_WritePin(IIC_SDA_GPIO, IIC_SDA_Pin, GPIO_PIN_RESET)		
#define IIC_SDA_READ()				(HAL_GPIO_ReadPin(IIC_SDA_GPIO,IIC_SDA_Pin) ? 1 : 0)	 

extern void IIC_WRITE_REGISTER(unsigned char DEVICE, unsigned char ADDRESS, unsigned char SENDDATA);
extern unsigned char IIC_READ_REGISTER(unsigned char DEVICE, unsigned char ADDRESS);

#endif

