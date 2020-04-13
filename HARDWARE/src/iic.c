#include <includes.h>

/* IIC ��ʱ			*/
void IIC_DELAY(void)
{
	unsigned char i;
	for (i=0; i<5; i++);
}

/* IIC ����			*/
void  IIC_START(void)
{
	IIC_SDA_H();
	IIC_SCL_H();IIC_DELAY();
	IIC_SDA_L();IIC_DELAY();
	IIC_SCL_L();
}

/* IIC ֹͣ			*/
void IIC_STOP(void)
{
	IIC_SDA_L();
	IIC_SCL_H();IIC_DELAY();
	IIC_SDA_H();	
	IIC_SCL_L();IIC_DELAY();
	IIC_SCL_H();
}

/* IIC��������		*/
void  IIC_WRITE(unsigned char WRITE_DATA)
{
	unsigned char i=0x80;

	for(i=0x80;i!=0;i>>=1)
	{	
		IIC_SCL_L();
		if(WRITE_DATA & i)
		{
			IIC_SDA_H();
		}
		else
		{
			IIC_SDA_L();
		}
		IIC_SCL_H();
	}
	IIC_SCL_L();IIC_DELAY();
	IIC_SDA_H();IIC_DELAY();
	IIC_SCL_H();
	i=0;
	while(IIC_SDA_READ()&&(i<250))
	{
		IIC_DELAY();
		i++;
	}
	IIC_SCL_L();
}

/* IIC ��ȡ����		*/
unsigned char IIC_READ(void)
{
	unsigned char i, READ_DATA=0;
	
	for(i=0;i<8;i++)
	{
		IIC_SCL_L();IIC_DELAY();
		IIC_SDA_H();IIC_DELAY();
		IIC_SCL_H();
		READ_DATA=READ_DATA<<1;
		if(IIC_SDA_READ())
		{
			READ_DATA++;
		}
	}
	return(READ_DATA);
}

/* IIC ������/�Ĵ�����ַд����Ӧ����	*/
void IIC_WRITE_REGISTER(unsigned char DEVICE, unsigned char ADDRESS, unsigned char SENDDATA)
{
	IIC_START();
	IIC_WRITE(DEVICE+IIC_WR_BIT);
	IIC_WRITE(ADDRESS);
	IIC_WRITE(SENDDATA);
	IIC_STOP();
}

/* IIC ������/�Ĵ�����ַ��ȡ��Ӧ����	*/
unsigned char IIC_READ_REGISTER(unsigned char DEVICE, unsigned char ADDRESS)
{
	unsigned char RECEIVE_DATA;
	
	IIC_START();
	IIC_WRITE(DEVICE+IIC_WR_BIT);
	IIC_WRITE(ADDRESS);
	IIC_START();
	IIC_WRITE((DEVICE+IIC_RD_BIT));
	RECEIVE_DATA=IIC_READ();
	IIC_STOP();
	return(RECEIVE_DATA);
}
