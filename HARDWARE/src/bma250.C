#include <includes.h>

unsigned char BMA250_Ang_x=0;
unsigned char BMA250_Ang_z=0;
unsigned char BMA250_Ang_x_buf=0;
unsigned char BMA250_Ang_z_buf=0;

/* BMA250 配置参数				*/
void BMA250_init(void)  
{		
	unsigned char BMA250_Int = 0;
	
	if(GSM_P.lowg)	{BMA250_Int|=0x08;}							// 如果已设置LOW报警
	if(GSM_P.higg)	{BMA250_Int|=0x07;}							// 如果已设置HIG报警

	BMA250_Write_Register(BMA2x2_RANGE_SELECT_REG,	0x0C);		// 0x0F寄存器,选择BMA250的测量范围，±16g
	BMA250_Write_Register(BMA2x2_INTR_SET_REG,		0x00);		// 0x20寄存器,配置BMA250的中断脚，OPEN DRIVE,active level 0
	BMA250_Write_Register(BMA2x2_INTR_SOURCE_REG,	0x00);		// 0x1E寄存器,中断源使用FILTER-ACCdata
	BMA250_Write_Register(BMA2x2_LOW_HIGH_HYST_REG,	0x86);		// 0x24寄存器,low-g模式和迟滞量
#if DEBUG
	BMA250_Write_Register(BMA2x2_INTR1_PAD_SELECT_REG,0x07);	// 0x19寄存器,映射中断脚INT1,high_g,low_g,slop
	BMA250_Write_Register(BMA2x2_INTR_CTRL_REG,		0x8F);		// 0x21寄存器,中断模式为：temporary 12.5ms 
#else
	BMA250_Write_Register(BMA2x2_INTR1_PAD_SELECT_REG,0x03);	// 0x19寄存器,映射中断脚INT1,high_g,low_g
	BMA250_Write_Register(BMA2x2_INTR_CTRL_REG,0x8F);			// 0x21寄存器,中断模式为：temporary 12.5ms 
#endif	
	
	// LOW
	BMA250_Write_Register(BMA2x2_LOW_DURN_REG,0x28);			// 0x22寄存器,low-g中断持续最小时间
	BMA250_Write_Register(BMA2x2_LOW_THRES_REG,GSM_P.lowg);		// 0x23寄存器,low-g中断阈值
	
	// HIGH
	BMA250_Write_Register(BMA2x2_HIGH_DURN_REG,0x1E);			// 0x25寄存器,high-g中断持续最小时间,32ms
	BMA250_Write_Register(BMA2x2_HIGH_THRES_REG,GSM_P.higg);	// 0x26寄存器,high-g中断阈值
		
	// TAP
	BMA250_Write_Register(BMA2x2_TAP_THRES_REG,0x08);			// 0x2B寄存器,TAP中断阈值
	BMA250_Write_Register(BMA2x2_TAP_PARAM_REG,0x04);			// 0x2A寄存器,TAP检测shock为50ms,quiet为30ms,dur为250ms
	
	// SLOPE
#if DEBUG
	BMA250_Write_Register(BMA2x2_SLOPE_DURN_REG,0x01);			// 0x27寄存器,slope中断超过阈值2次
	BMA250_Write_Register(BMA2x2_SLOPE_THRES_REG,0x0F);			// 0x28寄存器,slope中断阈值1252mg 0x28
#else

	BMA250_Write_Register(BMA2x2_SLOPE_DURN_REG,0x00);			// 0x27寄存器,slope中断超过阈值1次
	BMA250_Write_Register(BMA2x2_SLOPE_THRES_REG,0x00);			// 0x28寄存器,slope中断阈值1252mg 0x28
	
#endif

	// 设置低功耗
#if DEBUG
	BMA250_Write_Register(BMA2x2_MODE_CTRL_REG,0x54);			// 0x11寄存器,工作在low-power为0x5e,每0.025秒唤醒一次
#else
	BMA250_Write_Register(BMA2x2_MODE_CTRL_REG,0x5A);			// 0x11寄存器,工作在low-power为0x5e,每0.1秒唤醒一次
#endif
	BMA250_Write_Register(BMA2x2_LOW_NOISE_CTRL_REG,0x00);		// 0x12寄存器

	// 设置启用中断
#if DEBUG
	BMA250_Write_Register(BMA2x2_INTR_ENABLE1_REG,0x07);		// 0x16寄存器,SLOP三轴允许中断
#else
	BMA250_Write_Register(BMA2x2_INTR_ENABLE1_REG,0x00);		// 0x16寄存器,SLOP三轴都不允许中断
#endif
	BMA250_Write_Register(BMA2x2_INTR_ENABLE2_REG,BMA250_Int);	// 0x17寄存器,允许low-g,high-g中断		
}

/* 读取BMA250是否发生跌落中断			*/
bool BMA250_Is_LowINT(void)
{
	if(BMA250_Read_Register(BMA2x2_STAT1_REG)&0x01)	{return true;	}
	else											{return false;	}
}

/* 读取BMA250是否发生碰撞中断			*/
bool BMA250_Is_HigINT(void)
{
	if(BMA250_Read_Register(BMA2x2_STAT1_REG)&0x02)	{return true;	}
	else											{return false;	}
}

/* 读取BMA250是否发生SLOP中断			*/
bool BMA250_Is_SlopINT(void)
{
	if(BMA250_Read_Register(BMA2x2_STAT1_REG)&0x04)	{return true;	}
	else											{return false;	}
}

/* 读取BMA250是否发生双TAP中断			*/
#if DEBUG
bool BMA250_Is_DTapINT(void)
{
	if(BMA250_Read_Register(BMA2x2_STAT1_REG)&0x30)	{return true;	}
	else											{return false;	}
}
#endif

/* 读取中断源						*/
unsigned char BMA250_IS_INT(void)
{
	return(BMA250_Read_Register(BMA2x2_STAT1_REG));
}


/* 10位补码转换成16位的原码				*/
signed short int LSB_MSB_TO_ACCEL(unsigned char LSB,unsigned char MSB)
{    
	bool Neg = 0;
	signed short int ACCEL;
	
	ACCEL = MSB;

	if(MSB & 0x80){Neg = 1;ACCEL = ACCEL & 0x7f;}    	// 负数
	
	ACCEL = (ACCEL << 2) | (LSB >> 6);
	if(Neg) ACCEL -= 512;
	
	return ACCEL;
}

/* 读取X轴当前角度*/
void BM250_GET_Slop(void)
{
	unsigned char X_LSB;
	signed short int X,Y,Z;
	signed int XX,YY,ZZ;
	double SUM;
	
	X_LSB = BMA250_Read_Register(BMA2x2_X_AXIS_LSB_REG);
	
	if((X_LSB & 0x01) == 0x00)									// 如果第一次读取为旧数据,则再读一次
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

/* BMA250清除中断						*/
void BMA250_Clear_INT(void)
{
	IIC_WRITE_REGISTER(BMA250_DEVICE,BMA2x2_INTR_CTRL_REG,0x8F);
}

/* BMA250 写入寄存器设置					*/
void BMA250_Write_Register (unsigned char reg, unsigned char data)
{
	IIC_WRITE_REGISTER(BMA250_DEVICE,reg,data);
}	

/* BMA250 读取寄存器参数					*/
unsigned char BMA250_Read_Register (unsigned char reg)
{
	return(IIC_READ_REGISTER(BMA250_DEVICE,reg));
}	
