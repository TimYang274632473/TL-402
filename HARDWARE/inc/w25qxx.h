#ifndef __W25QXX_H
#define __W25QXX_H

#include "includes.h"

#define W25X_WriteEnable				0x06 
#define W25X_WriteDisable				0x04 
#define W25X_ReadStatusReg			0x05 
#define W25X_WriteStatusReg			0x01 
#define W25X_ReadData						0x03 
#define W25X_FastReadData		  	0x0B 
#define W25X_FastReadDual		  	0x3B 
#define W25X_PageProgram		  	0x02 
#define W25X_BlockErase			  	0xD8 
#define W25X_SectorErase		  	0x20 
#define W25X_ChipErase			  	0xC7 
#define W25X_PowerDown			  	0xB9 
#define W25X_ReleasePowerDown		0xAB 
#define W25X_ManufactDeviceID		0x90 
#define W25X_JedecDeviceID			0x9F 

#define  SPI_CS_LOW   HAL_GPIO_WritePin(CS_W25_GPIO_Port,CS_W25_Pin,GPIO_PIN_RESET) 
#define  SPI_CS_HIGH  HAL_GPIO_WritePin(CS_W25_GPIO_Port,CS_W25_Pin,GPIO_PIN_SET) 

extern u8 SPI_FLASH_BUF[4096];

extern u16  SPI_Flash_ReadID(void);
extern void SPI_Flash_Erase_Sector(u32 Dst_Addr);
extern void SPI_Flash_Read(u32 ReadAddr,u8* pBuffer,u16 NumByteToRead);   
extern void SPI_Flash_Write(u32 WriteAddr,u8* pBuffer,u16 NumByteToWrite);
extern void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
extern void SPI_Flash_Sleep(void);           
extern void SPI_Flash_Wakeup(void);	

#endif
