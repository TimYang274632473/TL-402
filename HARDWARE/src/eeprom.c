#include <includes.h>

/******************************************************************************
*	�� �� ��: IIC1_Configuration
*	����˵��: IIC��ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
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
*	�� �� ��: Delay
*	����˵��: �����ʱ
*	��    �Σ�ʱ��
*	�� �� ֵ: ��
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
*	�� �� ��: IIC1_Delay
*	����˵��: �����ʱ
*	��    �Σ�ʱ��
*	�� �� ֵ: ��
********************************************************************************/
void IIC1_Delay(void)
{
	uint8_t i;
	
	for (i = 0; i < 50; i++);
}

/******************************************************************************
*	�� �� ��: IIC1_Start
*	����˵��: IIC����
*	��    �Σ���
*	�� �� ֵ: ��
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
*	�� �� ��: IIC1_Stop
*	����˵��: IICֹͣ
*	��    �Σ���
*	�� �� ֵ: ��
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
*	�� �� ��: IIC1_Write
*	����˵��: IICд��λ����
*	��    �Σ�����
*	�� �� ֵ: ���
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
*	�� �� ��: IIC1_Read
*	����˵��: IIC��ȡ��λ����
*	��    �Σ���
*	�� �� ֵ: ����
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
*	�� �� ��: IIC1_ACK
*	����˵��: IICӦ��
*	��    �Σ���
*	�� �� ֵ: ����
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
*	�� �� ��: IIC1_NACK
*	����˵��: IICӦ����
*	��    �Σ���
*	�� �� ֵ: ����
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
*	�� �� ��: IIC1_ReadACK
*	����˵��: IIC��ȡACK
*	��    �Σ���
*	�� �� ֵ: ����
********************************************************************************/
u8 IIC1_ReadACK(void)
{
    u8 byte;
    
    byte = IIC1_Read();
    IIC1_ACK();
    
    return byte;
}

/******************************************************************************
*	�� �� ��: IIC1_ReadNAK
*	����˵��: IIC��ȡNACK
*	��    �Σ���
*	�� �� ֵ: ����
********************************************************************************/
u8 IIC1_ReadNAK(void)
{
    u8 byte;
    
    byte = IIC1_Read();
    IIC1_NACK();

    return byte;
}

/******************************************************************************
*	�� �� ��: IIC1_WaitThenStart
*	����˵��: IIC�ȴ�д��
*	��    �Σ���ַ
*	�� �� ֵ: ���
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
*	�� �� ��: IIC1_BufferWrite
*	����˵��: IIC����д��
*	��    �Σ���ַ,����,����
*	�� �� ֵ: ���
********************************************************************************/
int IIC1_BufferWrite(u8* pBuffer, u16 WriteAddr, u16 NumByteToWrite)
{
	u16 cnt;
	u16 border;
	u8  devAddr;

	IIC1_WP_0();																		/* ȡ��д���� */
	if (((u32)WriteAddr+(u32)NumByteToWrite) > ((u32)CHIP1_SIZE_Kbits*1024/8)) {
		return -1;
	}
	devAddr = DEV1_ADDR + SUB1_ADDR;
	cnt = NumByteToWrite;
	while (cnt > 0)
	{
		border = WriteAddr - (WriteAddr%PAGE1_SIZE_Bytes) + PAGE1_SIZE_Bytes;  		/* ��ҳԤ��������ҳ�߽�, border��ֵ����д���ַ���ڵ�ҳ�Ľ�����ַ+1 */
		if (border > (WriteAddr+cnt)) {
			border = WriteAddr + cnt;
		}
#if (CHIP1_SIZE_Kbits <= 16) 														/* 24C16������������ȡ��ַΪ1�ֽ� */
		devAddr |= (e2addr >> 8);
		if (IIC1_WaitThenStart(devAddr) != 0) {
			return -1;
		}
		if (IIC1_Write((u8)(e2addr&0xFF)) != 0) {
			I2CStop();
			return -1;
		}
#else  																				/* 24C32������������ȡ��ַΪ2�ֽ� */
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
		while (WriteAddr < border) {  												/* ������ҳд������ */
			if(IIC1_Write(*pBuffer) != 0) {
				IIC1_Stop();
				return -1;
			}
			pBuffer++;
			WriteAddr++;
			cnt--;
			LED1_Blink();
		}
		IIC1_Stop();																	/* һҳд���Ժ���Ҫ����I2Cʱ�� */
		Delay1(30);
	}
	IIC1_WP_1(); 																	/* ʹ��д����  */
	return NumByteToWrite;	
}

/******************************************************************************
*	�� �� ��: IIC1_BufferRead
*	����˵��: IIC������ȡ
*	��    �Σ���ַ,����,����
*	�� �� ֵ: ���
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
#if (CHIP1_SIZE_Kbits <= 16) 														/* 24C16������������ȡ��ַΪ1�ֽ� */
	devAddr |= (e2addr >> 8);
	if (IIC1_WaitThenStart(devAddr) != 0) {
		return -1;
	}
	if (IIC1_Write((u8)(e2addr&0xFF)) != 0) {
		I2CStop();
		return -1;
	}
#else  																				/* 24C32������������ȡ��ַΪ2�ֽ� */
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
	IIC1_Start();  																	/* �������������� */
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
*	�� �� ��: IIC1_WriteU32
*	����˵��: д��32λ����
*	��    �Σ���ַ,����
*	�� �� ֵ: ���
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
*	�� �� ��: IIC1_ReadU32
*	����˵��: ��ȡ32λ����
*	��    �Σ���ַ
*	�� �� ֵ: ����
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
*	�� �� ��: IIC1_ERASE
*	����˵��: �������
*	��    �Σ���ַ,����
*	�� �� ֵ: ���
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
