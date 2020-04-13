#include <includes.h>

/******************************************************************************
*	函 数 名: IIC1_Configuration
*	功能说明: IIC初始化
*	形    参：无
*	返 回 值: 无
********************************************************************************/
void IIC1_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOD, ENABLE); 

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	/* PC11_SCL */
	GPIO_InitStructure.GPIO_Pin =  IIC1_SCL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	/* PD2_SDA */
	GPIO_InitStructure.GPIO_Pin =  IIC1_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	/* PA11_WP */
	GPIO_InitStructure.GPIO_Pin =  IIC1_WP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/******************************************************************************
*	函 数 名: Delay
*	功能说明: 软件延时
*	形    参：时间
*	返 回 值: 无
********************************************************************************/
void Delay1(uint32_t x)
{
	uint32_t i, j;
	
	for (i=0; i<x; i++) 
	{
		for (j=0; j<1000; j++);
	}
}

/******************************************************************************
*	函 数 名: IIC1_Delay
*	功能说明: 软件延时
*	形    参：时间
*	返 回 值: 无
********************************************************************************/
void IIC1_Delay(void)
{
	uint8_t i;
	
	for (i = 0; i < 50; i++);
}

/******************************************************************************
*	函 数 名: IIC1_Start
*	功能说明: IIC启动
*	形    参：无
*	返 回 值: 无
********************************************************************************/
void IIC1_Start(void)
{
	IIC1_SDA_1();
	IIC1_SCL_1();
	IIC1_Delay();
	IIC1_SDA_0();
	IIC1_Delay();
	IIC1_SCL_0();
}

/******************************************************************************
*	函 数 名: IIC1_Stop
*	功能说明: IIC停止
*	形    参：无
*	返 回 值: 无
********************************************************************************/
void IIC1_Stop(void)
{
	IIC1_SCL_0();
	IIC1_SDA_0();
	IIC1_Delay();
	IIC1_SCL_1();
	IIC1_Delay();
	IIC1_SDA_1();
	IIC1_Delay();
}

/******************************************************************************
*	函 数 名: IIC1_Write
*	功能说明: IIC写八位数据
*	形    参：数据
*	返 回 值: 结果
********************************************************************************/
s8 IIC1_Write(u8 dat)
{
	u8 mask;

	for (mask=0x80; mask!=0; mask>>=1)
	{
		IIC1_SCL_0();
		if ((mask&dat) == 0)
		{  
			IIC1_SDA_0();
		}
		else 
		{  
			IIC1_SDA_1(); 
		}
		IIC1_Delay();
		IIC1_SCL_1();
		IIC1_Delay();
	}
	IIC1_SCL_0();
	IIC1_SDA_1();
	IIC1_Delay();
	IIC1_SCL_1();
	IIC1_Delay();
    mask = IIC1_SDA_READ();
	IIC1_SCL_0();
    
    return mask;
}

/******************************************************************************
*	函 数 名: IIC1_Read
*	功能说明: IIC读取八位数据
*	形    参：无
*	返 回 值: 数据
********************************************************************************/
static u8 IIC1_Read(void)
{
	u8 mask;
	u8 byte;

	byte = 0;
	for (mask=0x80; mask!=0; mask>>=1)
	{
		IIC1_SCL_0();
		IIC1_SDA_1();
		IIC1_Delay();
		IIC1_SCL_1();
		IIC1_Delay();
		if(IIC1_SDA_READ() != 0)
		{
			byte |= mask;
		}
	}

	return byte;
}

/******************************************************************************
*	函 数 名: IIC1_ACK
*	功能说明: IIC应答
*	形    参：无
*	返 回 值: 数据
********************************************************************************/
static void IIC1_ACK(void)
{
	IIC1_SCL_0();
	IIC1_SDA_0();
	IIC1_Delay();
	IIC1_SCL_1();
	IIC1_Delay();
	IIC1_SCL_0();
}

/******************************************************************************
*	函 数 名: IIC1_NACK
*	功能说明: IIC应答无
*	形    参：无
*	返 回 值: 数据
********************************************************************************/
static void IIC1_NACK(void)
{
	IIC1_SCL_0();
	IIC1_SDA_1();
	IIC1_Delay();
	IIC1_SCL_1();
	IIC1_Delay();
	IIC1_SCL_0();
}

/******************************************************************************
*	函 数 名: IIC1_ReadACK
*	功能说明: IIC读取ACK
*	形    参：无
*	返 回 值: 数据
********************************************************************************/
u8 IIC1_ReadACK(void)
{
    u8 byte;
    
    byte = IIC1_Read();
    IIC1_ACK();
    
    return byte;
}

/******************************************************************************
*	函 数 名: IIC1_ReadNAK
*	功能说明: IIC读取NACK
*	形    参：无
*	返 回 值: 数据
********************************************************************************/
u8 IIC1_ReadNAK(void)
{
    u8 byte;
    
    byte = IIC1_Read();
    IIC1_NACK();

    return byte;
}

/******************************************************************************
*	函 数 名: IIC1_WaitThenStart
*	功能说明: IIC等待写入
*	形    参：地址
*	返 回 值: 结果
********************************************************************************/
static int IIC1_WaitThenStart(u8 addr)
{
	uint16_t dly=0;

	do {
		IIC1_Start();
		if (IIC1_Write(addr<<1) == 0) {
			return 0;
		}
		IIC1_Stop();
		IIC1_Delay ();
		IIC1_Delay ();
		IIC1_Delay ();
		IIC1_Delay ();
		dly++;
	} while (dly <= ACCESS1_DELAY_ms);

	return -1;
}

/******************************************************************************
*	函 数 名: IIC1_BufferWrite
*	功能说明: IIC连续写入
*	形    参：地址,数据,长度
*	返 回 值: 结果
********************************************************************************/
int IIC1_BufferWrite(u8* pBuffer, u16 WriteAddr, u16 NumByteToWrite)
{
	u16 cnt;
	u16 border;
	u8  devAddr;

	IIC1_WP_0();																		/* 取消写保护 */
	if (((u32)WriteAddr+(u32)NumByteToWrite) > ((u32)CHIP1_SIZE_Kbits*1024/8)) {
		return -1;
	}
	devAddr = DEV1_ADDR + SUB1_ADDR;
	cnt = NumByteToWrite;
	while (cnt > 0)
	{
		border = WriteAddr - (WriteAddr%PAGE1_SIZE_Bytes) + PAGE1_SIZE_Bytes;  		/* 跨页预处理，计算页边界, border的值就是写入地址所在的页的结束地址+1 */
		if (border > (WriteAddr+cnt)) {
			border = WriteAddr + cnt;
		}
#if (CHIP1_SIZE_Kbits <= 16) 														/* 24C16及以下容量存取地址为1字节 */
		devAddr |= (e2addr >> 8);
		if (IIC1_WaitThenStart(devAddr) != 0) {
			return -1;
		}
		if (IIC1_Write((u8)(e2addr&0xFF)) != 0) {
			I2CStop();
			return -1;
		}
#else  																				/* 24C32及以上容量存取地址为2字节 */
		if (IIC1_WaitThenStart(devAddr) != 0) {
			return -1;
		}
		if (IIC1_Write((u8)(WriteAddr>>8)) != 0) {
			IIC1_Stop();
			return -1;
		}
		if (IIC1_Write((u8)(WriteAddr&0xFF)) != 0) {
			IIC1_Stop();
			return -1;
		}
#endif
		while (WriteAddr < border) {  												/* 启动按页写入数据 */
			if(IIC1_Write(*pBuffer) != 0) {
				IIC1_Stop();
				return -1;
			}
			pBuffer++;
			WriteAddr++;
			cnt--;
			LED1_Blink();
		}
		IIC1_Stop();																	/* 一页写完以后需要重启I2C时序 */
		Delay1(30);
	}
	IIC1_WP_1(); 																	/* 使能写保护  */
	return NumByteToWrite;	
}

/******************************************************************************
*	函 数 名: IIC1_BufferRead
*	功能说明: IIC连续读取
*	形    参：地址,数据,长度
*	返 回 值: 结果
********************************************************************************/
int IIC1_BufferRead(u8* pBuffer, u16 ReadAddr, u16 NumByteToRead)
{
	uint16_t cnt;
	uint8_t  devAddr;
	uint8_t *dest;

	if (((u32)ReadAddr+(u32)NumByteToRead) > ((u32)CHIP1_SIZE_Kbits*1024/8)) {
		return -1;
	}
	devAddr = DEV1_ADDR + SUB1_ADDR;		
#if (CHIP1_SIZE_Kbits <= 16) 														/* 24C16及以下容量存取地址为1字节 */
	devAddr |= (e2addr >> 8);
	if (IIC1_WaitThenStart(devAddr) != 0) {
		return -1;
	}
	if (IIC1_Write((u8)(e2addr&0xFF)) != 0) {
		I2CStop();
		return -1;
	}
#else  																				/* 24C32及以上容量存取地址为2字节 */
	if (IIC1_WaitThenStart(devAddr) != 0) { 
		return -1;
	}
	
	if (IIC1_Write((u8)(ReadAddr>>8)) != 0) {
		IIC1_Stop();
		return -1;
	}
	if (IIC1_Write((u8)(ReadAddr&0xFF)) != 0) {
		IIC1_Stop();
		return -1;
	}
#endif
	IIC1_Start();  																	/* 启动连续读数据 */
	if (IIC1_Write((devAddr<<1)|0x1) != 0) {
		IIC1_Stop();
		return -1;
	}
	cnt = NumByteToRead;
	dest = (u8 *)pBuffer;
	while (cnt > 1) {
		*dest = IIC1_ReadACK();
		dest++;
		cnt--;
		LED1_Blink();
	}
	if (cnt == 1) {
		*dest = IIC1_ReadNAK();
	}
	IIC1_Stop();
	Delay1(5);
	return NumByteToRead;
}

/******************************************************************************
*	函 数 名: IIC1_WriteU32
*	功能说明: 写入32位数据
*	形    参：地址,数据
*	返 回 值: 结果
********************************************************************************/
int IIC1_WriteU32(u16 WriteAddr, uint32_t dat)
{
	uint8_t buff[4] = {0, 0, 0, 0};
	
	buff[0] = (uint8_t)(dat >> 24);
	buff[1] = (uint8_t)(dat >> 16);
	buff[2] = (uint8_t)(dat >> 8);
	buff[3] = (uint8_t)(dat >> 0);
	
	return (IIC1_BufferWrite((u8*)buff, WriteAddr, 4));
}

/******************************************************************************
*	函 数 名: IIC1_ReadU32
*	功能说明: 读取32位数据
*	形    参：地址
*	返 回 值: 数据
********************************************************************************/
uint32_t IIC1_ReadU32(u16 ReadAddr)
{
	uint32_t dat = 0;
	uint8_t buff[4] = {0, 0, 0, 0};
	
	IIC1_BufferRead((u8*)buff, ReadAddr, 4);
	dat = ((buff[0]<<24) | (buff[1]<<16) | (buff[2]<<8) | buff[3]);
	return (dat);
}

/******************************************************************************
*	函 数 名: IIC1_ERASE
*	功能说明: 数据清除
*	形    参：地址,长度
*	返 回 值: 结果
********************************************************************************/
int IIC1_ERASE(u16 StartAddr, uint16_t Numofbytes)
{
	uint16_t i;
	u8 *pbuff = malloc (Numofbytes);
	
	if (pbuff != NULL) {
		for (i=0; i<Numofbytes; i++) {
			pbuff[i] = 0x00;
		}
		IIC1_BufferWrite(pbuff, StartAddr, Numofbytes);
	}
	else {
		printf ("malloc failed!\r\n");
		return -1;
	}
	free (pbuff);
	return Numofbytes;
}
