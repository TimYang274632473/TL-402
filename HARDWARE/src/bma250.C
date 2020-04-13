#include <includes.h>

unsigned char BMA250_Ang_x=0;
unsigned char BMA250_Ang_z=0;
unsigned char BMA250_Ang_x_buf=0;
unsigned char BMA250_Ang_z_buf=0;

/* BMA250 ���ò���				*/
void BMA250_init(void)  
{		
	unsigned char BMA250_Int = 0;
	
	if(GSM_P.lowg)	{BMA250_Int|=0x08;}							// ���������LOW����
	if(GSM_P.higg)	{BMA250_Int|=0x07;}							// ���������HIG����

	BMA250_Write_Register(BMA2x2_RANGE_SELECT_REG,	0x0C);		// 0x0F�Ĵ���,ѡ��BMA250�Ĳ�����Χ����16g
	BMA250_Write_Register(BMA2x2_INTR_SET_REG,		0x00);		// 0x20�Ĵ���,����BMA250���жϽţ�OPEN DRIVE,active level 0
	BMA250_Write_Register(BMA2x2_INTR_SOURCE_REG,	0x00);		// 0x1E�Ĵ���,�ж�Դʹ��FILTER-ACCdata
	BMA250_Write_Register(BMA2x2_LOW_HIGH_HYST_REG,	0x86);		// 0x24�Ĵ���,low-gģʽ�ͳ�����
#if DEBUG
	BMA250_Write_Register(BMA2x2_INTR1_PAD_SELECT_REG,0x07);	// 0x19�Ĵ���,ӳ���жϽ�INT1,high_g,low_g,slop
	BMA250_Write_Register(BMA2x2_INTR_CTRL_REG,		0x8F);		// 0x21�Ĵ���,�ж�ģʽΪ��temporary 12.5ms 
#else
	BMA250_Write_Register(BMA2x2_INTR1_PAD_SELECT_REG,0x03);	// 0x19�Ĵ���,ӳ���жϽ�INT1,high_g,low_g
	BMA250_Write_Register(BMA2x2_INTR_CTRL_REG,0x8F);			// 0x21�Ĵ���,�ж�ģʽΪ��temporary 12.5ms 
#endif	
	
	// LOW
	BMA250_Write_Register(BMA2x2_LOW_DURN_REG,0x28);			// 0x22�Ĵ���,low-g�жϳ�����Сʱ��
	BMA250_Write_Register(BMA2x2_LOW_THRES_REG,GSM_P.lowg);		// 0x23�Ĵ���,low-g�ж���ֵ
	
	// HIGH
	BMA250_Write_Register(BMA2x2_HIGH_DURN_REG,0x1E);			// 0x25�Ĵ���,high-g�жϳ�����Сʱ��,32ms
	BMA250_Write_Register(BMA2x2_HIGH_THRES_REG,GSM_P.higg);	// 0x26�Ĵ���,high-g�ж���ֵ
		
	// TAP
	BMA250_Write_Register(BMA2x2_TAP_THRES_REG,0x08);			// 0x2B�Ĵ���,TAP�ж���ֵ
	BMA250_Write_Register(BMA2x2_TAP_PARAM_REG,0x04);			// 0x2A�Ĵ���,TAP���shockΪ50ms,quietΪ30ms,durΪ250ms
	
	// SLOPE
#if DEBUG
	BMA250_Write_Register(BMA2x2_SLOPE_DURN_REG,0x01);			// 0x27�Ĵ���,slope�жϳ�����ֵ2��
	BMA250_Write_Register(BMA2x2_SLOPE_THRES_REG,0x0F);			// 0x28�Ĵ���,slope�ж���ֵ1252mg 0x28
#else

	BMA250_Write_Register(BMA2x2_SLOPE_DURN_REG,0x00);			// 0x27�Ĵ���,slope�жϳ�����ֵ1��
	BMA250_Write_Register(BMA2x2_SLOPE_THRES_REG,0x00);			// 0x28�Ĵ���,slope�ж���ֵ1252mg 0x28
	
#endif

	// ���õ͹���
#if DEBUG
	BMA250_Write_Register(BMA2x2_MODE_CTRL_REG,0x54);			// 0x11�Ĵ���,������low-powerΪ0x5e,ÿ0.025�뻽��һ��
#else
	BMA250_Write_Register(BMA2x2_MODE_CTRL_REG,0x5A);			// 0x11�Ĵ���,������low-powerΪ0x5e,ÿ0.1�뻽��һ��
#endif
	BMA250_Write_Register(BMA2x2_LOW_NOISE_CTRL_REG,0x00);		// 0x12�Ĵ���

	// ���������ж�
#if DEBUG
	BMA250_Write_Register(BMA2x2_INTR_ENABLE1_REG,0x07);		// 0x16�Ĵ���,SLOP���������ж�
#else
	BMA250_Write_Register(BMA2x2_INTR_ENABLE1_REG,0x00);		// 0x16�Ĵ���,SLOP���ᶼ�������ж�
#endif
	BMA250_Write_Register(BMA2x2_INTR_ENABLE2_REG,BMA250_Int);	// 0x17�Ĵ���,����low-g,high-g�ж�		
}

/* ��ȡBMA250�Ƿ��������ж�			*/
bool BMA250_Is_LowINT(void)
{
	if(BMA250_Read_Register(BMA2x2_STAT1_REG)&0x01)	{return true;	}
	else											{return false;	}
}

/* ��ȡBMA250�Ƿ�����ײ�ж�			*/
bool BMA250_Is_HigINT(void)
{
	if(BMA250_Read_Register(BMA2x2_STAT1_REG)&0x02)	{return true;	}
	else											{return false;	}
}

/* ��ȡBMA250�Ƿ���SLOP�ж�			*/
bool BMA250_Is_SlopINT(void)
{
	if(BMA250_Read_Register(BMA2x2_STAT1_REG)&0x04)	{return true;	}
	else											{return false;	}
}

/* ��ȡBMA250�Ƿ���˫TAP�ж�			*/
#if DEBUG
bool BMA250_Is_DTapINT(void)
{
	if(BMA250_Read_Register(BMA2x2_STAT1_REG)&0x30)	{return true;	}
	else											{return false;	}
}
#endif

/* ��ȡ�ж�Դ						*/
unsigned char BMA250_IS_INT(void)
{
	return(BMA250_Read_Register(BMA2x2_STAT1_REG));
}


/* 10λ����ת����16λ��ԭ��				*/
signed short int LSB_MSB_TO_ACCEL(unsigned char LSB,unsigned char MSB)
{    
	bool Neg = 0;
	signed short int ACCEL;
	
	ACCEL = MSB;

	if(MSB & 0x80){Neg = 1;ACCEL = ACCEL & 0x7f;}    	// ����
	
	ACCEL = (ACCEL << 2) | (LSB >> 6);
	if(Neg) ACCEL -= 512;
	
	return ACCEL;
}

/* ��ȡX�ᵱǰ�Ƕ�*/
void BM250_GET_Slop(void)
{
	unsigned char X_LSB;
	signed short int X,Y,Z;
	signed int XX,YY,ZZ;
	double SUM;
	
	X_LSB = BMA250_Read_Register(BMA2x2_X_AXIS_LSB_REG);
	
	if((X_LSB & 0x01) == 0x00)									// �����һ�ζ�ȡΪ������,���ٶ�һ��
	{
		X_LSB = BMA250_Read_Register(BMA2x2_X_AXIS_LSB_REG);
	}
	
	X  = LSB_MSB_TO_ACCEL(X_LSB,BMA250_Read_Register(BMA2x2_X_AXIS_MSB_REG));
	XX = X * X;
	
	Y = LSB_MSB_TO_ACCEL(BMA250_Read_Register(BMA2x2_Y_AXIS_LSB_REG),BMA250_Read_Register(BMA2x2_Y_AXIS_MSB_REG));
	YY = Y * Y;
	
	Z = LSB_MSB_TO_ACCEL(BMA250_Read_Register(BMA2x2_Z_AXIS_LSB_REG),BMA250_Read_Register(BMA2x2_Z_AXIS_MSB_REG));
	ZZ = Z * Z;
	
	SUM = XX + YY + ZZ;
	
	SUM = sqrt(SUM);
	
	BMA250_Ang_x = (unsigned char)(acos(X / SUM) * 57.29578049);

	BMA250_Ang_z = (unsigned char)(acos(Z / SUM) * 57.29578049);
}

/* BMA250����ж�						*/
void BMA250_Clear_INT(void)
{
	IIC_WRITE_REGISTER(BMA250_DEVICE,BMA2x2_INTR_CTRL_REG,0x8F);
}

/* BMA250 д��Ĵ�������					*/
void BMA250_Write_Register (unsigned char reg, unsigned char data)
{
	IIC_WRITE_REGISTER(BMA250_DEVICE,reg,data);
}	

/* BMA250 ��ȡ�Ĵ�������					*/
unsigned char BMA250_Read_Register (unsigned char reg)
{
	return(IIC_READ_REGISTER(BMA250_DEVICE,reg));
}	