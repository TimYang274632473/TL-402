#ifndef __APP_TASK_1_H
#define __APP_TASK_1_H

#include "includes.h"

#define Flash_Read					0x01
#define Flash_Write					0x02
#define Flash_Erase_Sector			0x03
#define Flash_Reset_ALL				0x04
#define Flash_Reset_WithoutOTA		0x05
#define Flash_Read_ID				0x06
#define Flash_SETG					0x07
#define Flash_Write_History			0x08
#define Flash_Read_History			0x09
#define Flash_MCU_Reset				0x0A
#define Flash_MCU_Power_Off			0x0B

typedef struct 
{
	char 			Cmd;
	
	u32				W25_Address;
	u8*				W25_Data;
	u16				W25_Length;
	
}type_Flash;

extern void Task_SPI_Flash(u8 SPI_cmd,u32 SPI_Addr,u8* pBuffer,u16 Length);
extern void Task_SPI_Flash_Callback(u8 SPI_cmd);
extern void App_Task1(void const * argument);

#endif







