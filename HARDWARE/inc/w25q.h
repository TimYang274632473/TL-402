#ifndef __W25Q_H
#define __W25Q_H	

#include "includes.h"

#define W25X_WriteEnable		  0x06 
#define W25X_WriteDisable		  0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			    0x03 
#define W25X_FastReadData		  0x0B 
#define W25X_FastReadDual		  0x3B 
#define W25X_PageProgram		  0x02 
#define W25X_BlockErase			  0xD8 
#define W25X_SectorErase		  0x20 
#define W25X_ChipErase			  0xC7 
#define W25X_PowerDown			  0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			    0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 

extern u8 SPI_FLASH_BUF[4096];

u16  SPI_Flash_ReadID(void);  	    //??FLASH ID
u8	 SPI_Flash_ReadSR(void);        //??????? 
void SPI_FLASH_Write_SR(u8 sr);  	//??????
void SPI_FLASH_Write_Enable(void);  //??? 
void SPI_FLASH_Write_Disable(void);	//???
void SPI_Flash_Read(u32 ReadAddr,u8* pBuffer,u16 NumByteToRead);   //??flash
void SPI_Flash_Write(u32 WriteAddr,u8* pBuffer,u16 NumByteToWrite);//??flash
void SPI_Flash_Erase_Chip(void);    	  //????
void SPI_Flash_Erase_Sector(u32 Dst_Addr);//????
void SPI_Flash_Wait_Busy(void);           //????
void SPI_Flash_PowerDown(void);           //??????
void SPI_Flash_WAKEUP(void);			  //??
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);


u8 SPIx_ReadWriteByte(u8 TxData);
void SPI_Flash_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);

#endif
