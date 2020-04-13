#ifndef __usart1_H
#define __usart1_H

#define USART_RXD_Group_Len		3
#define USART_RXD_Line_Len		80

extern char 			USART1_RXD_DATA[USART_RXD_Group_Len][USART_RXD_Line_Len];	// 存放串口接收的数据
extern unsigned char 	USART1_RXD_Group_Cnt;										// 组
extern unsigned char 	USART1_RXD_Line_Cnt;										// 列

#if MODULE_SIM7500
extern unsigned int 	Aduio_Cnt;
extern unsigned int 	Audio_Length;
#endif

extern void USART1_DatHandle(char dat);												// 正常接收串口数据
extern void USART_Update(void);

#endif

