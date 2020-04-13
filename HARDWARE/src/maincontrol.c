#include <includes.h>

ECHO_Para_Def 	ECHO_F;

unsigned char Track_Time_Unit=10;				// 每次连续定位开启GPS时间			

unsigned char InValid_GPS_Count=1;				// 连续无效GPS次数
	
unsigned char Bat_Val[3]={0,0,0};	
unsigned char Bat_Cnt=0;

/*重置参数相关代码*/

const nmea_msg default_gpsx=
{ 	
	{19,1,1,12,0,0},  		// utc
	0,                  	// latitude
	0,                  	// longitude
	0,                  	// SPEED
	0,						// course
	'E',                	// ewhemi
	'N',                	// nshemi
	0,                  	// posslnum
	0,						// level
	'V',                	// AV_indicate
	
	0x0C,
	
	0,                  	// soc
	0,                  	// sig_strength
	0,                  	// gsmbase1
	0,                  	// gsmbase2
	0,                  	// operators
};

const GSM_para default_GSM_P= 
{
#if DEBUG

	"fffffffffffffff\0",   	// IMEI号
	"3gnet","TCP",
	"","",         			// APN
	"","",                 	// IP地址,// IP端口            	
	"","",					// 域名				
	5,                   	// 连续定位间隔时间（不带时间单位）
	'M',                 	// 连续定位时间单位
	0x88,                   // 时区
	80,                  	// 超速阈值
	30,                  	// 低电阈值	
	180,                  	// 发送心跳时间
	100,                 	// 圆心电子围栏半径
	22.7613506,          	// 圆心电子围栏中心纬度
	114.3610382,         	// 圆心电子围栏中心经度
	0,                  	// 已发送历史数据条数	
	0,                   	// 已存入历史数据条数
	9,                   	// 通话音量
	50,						// 跌落阀值
	0,						// 碰撞阀值
	0,
	12,0,					// 闹铃时,分
	"123456\0"            	// 指令密码，初始值：123456
	
#else

	"fffffffffffffff\0",   	// IMEI号
	"3gnet","TCP",
	"","",         			// APN
	"","",                 	// IP地址,// IP端口            	
	"","",					// 域名				
	5,                   	// 连续定位间隔时间（不带时间单位）
	'M',                 	// 连续定位时间单位
	0x88,                   // 时区
	80,                  	// 超速阈值
	30,                  	// 低电阈值	
	180,                  	// 发送心跳时间
	100,                 	// 圆心电子围栏半径
	22.7613506,          	// 圆心电子围栏中心纬度
	114.3610382,         	// 圆心电子围栏中心经度
	0,                  	// 已发送历史数据条数	
	0,                   	// 已存入历史数据条数
	9,                   	// 通话音量
	0,						// 跌落阀值
	0,						// 碰撞阀值
	0,
	12,0,					// 闹铃时,分
	"123456\0"            	// 指令密码，初始值：123456
	
#endif
};

const GSM_phone default_GSM_ph= 
{	
	"",					// temp_ph1
	"", 				// temp_ph2
	"", 				// temp_ph3
	"", 				// temp_ph4
	"", 				// temp_ph5
	"", 				// temp_ph6
	"", 				// temp_ph7
	""  				// temp_ph8
};

const GSM_phonename default_phname =
{	
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	""
};

const GSM_OTA_def default_GSM_OTA=
{	
	0x00,     				// OTA升级状态
	"",       				// OTA升级版本号
	0,        				// OTA失败次数
	0 ,        				// MCU备份到SPI状态
	"",
	0
}; 

/*0x9B默认开启：GPRS,连续跟踪，低电报警，时区+，运行拨打SOS*/

#if DEBUG
#if MODULE_UG96
const u8 default_flags[10]={0x60,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
#elif MODULE_SIM7500
const u8 default_flags[10]={0x60,0x01,0x00,0x00,0x00,0x00,0x00,0x1C,0x00,0x00};
#endif
#else
const u8 default_flags[10]={0x28,0x29,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
#endif

const GEO_coordinate default_geoc[5]=
{
	{0,0,0,0},				// 第一组GEO两点经纬度坐标
	{0,0,0,0},				// 第二组GEO两点经纬度坐标
	{0,0,0,0},				// 第三组GEO两点经纬度坐标
	{0,0,0,0},				// 第四组GEO两点经纬度坐标
	{0,0,0,0},				// 第五组GEO两点经纬度坐标
};	

cnt_1ms_def cnt_1ms={0,0,0,0,0,0,0,0};
cnt_1s_def  cnt_1s ={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	
Def_Flag_100mS	Tim_F;
MCU_F_def 		MCU_F;
	
/*	重定义putc函数，这样可以使用printf函数从串口打印输出	*/
int fputc(int ch, FILE *f)
{	
	USART1_SendData((char)ch);
	return ch;
}

/* 对串口1发送单个字符`									*/
void USART1_SendData(char dat)
{
	while(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TXE) == RESET);
	huart1.Instance->TDR=dat;
}
	
/* 对串口2发送单个字符									*/
void USART2_SendData(char dat)
{
	while(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE) == RESET);
	huart2.Instance->TDR=dat;
}

/* 对低功耗串口1送单个字符								*/
void LUSART1_SendData(char dat)
{
	while(__HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_TXE) == RESET);
	hlpuart1.Instance->TDR=dat;
}

/* 对串口4发送单个字符									*/
void USART4_SendData(char dat)
{
	while(__HAL_UART_GET_FLAG(&huart4, UART_FLAG_TXE) == RESET);
	huart4.Instance->TDR=dat;
}

//读取EEPROM保存的数据
void Read_Flash_Para(void)
{
	u8 eep_data[10]={0};
		
	SPI_Flash_Wakeup();
		
	SPI_Flash_Read(flags_addr, 	 eep_data,			flags_size		);
	SPI_Flash_Read(GSM_paraddr,	 (u8*)&GSM_P,		GSM_para_size	);
	SPI_Flash_Read(phoneaddr,	 (u8*)&GSM_ph,		GSM_ph_size		);
	SPI_Flash_Read(gpsadd,		 (u8*)&gpsx,		gpsx_All_Size	);	
	SPI_Flash_Read(phonenameaddr,(u8*)&GSM_phname,	phonenamesize	);
#if (GEO_FUN1==1)
	SPI_Flash_Read(geoaddr,		(u8*)&GEO_c,	GEO_c_size		);						//读取5组矩形GEO存储数据 
#endif
	
	SPI_Flash_Read(otaaddr, 	(u8*)&GSM_OTA,	GSM_OTA_size	);
	
	GSMA.w=eep_data[0];																	// 读取GPRS连接标志
	GSMB.w=eep_data[1];																	// 读出通话、监听、GPS工作模式等参数	
	GSMC.w=eep_data[2];	
#if (GEO_FUN1==1)																		// 5组电子围栏数据	
	GEOA.w=eep_data[3];																	// 各组GEO打开标志
	GEOB.w=eep_data[4];																	// 各组GEO出去标志
	GEOC.w=eep_data[5];																	// 各组GEO进入标志
	GEOD.w=eep_data[6];																	// 各组GEO设置有效标志  
#endif	
	ECHO_F.w=eep_data[7];	
	GSMD.w=eep_data[8];
	
	if((GSM_OTA.OTA_state&0x11) == 0x10)												// 如果APP升级成功,需要重设参数区默认设置
	{
		GSM_OTA.OTA_state = GSM_OTA.OTA_state & 0xEF;
		initialize_all(0);																// 除OTA外,重置所有默认参数
	}
	
	if(GSM_OTA.OTA_ph > 7)
	{
		GSM_OTA.OTA_ph = 0;
		SPI_Flash_Write(otaaddr,		(u8*)&GSM_OTA,			GSM_OTA_size);	
	}
	
	//如果密码格式有误（出现在新机器第一次运行），则按默认密码123456处理	
#if (password_check==1)
	
	if(Isdigit(GSM_P.cmd_key,6) == false)
	{
		initialize_all(1);
	}
		
#endif	
	
	if((gpsx.utc.year < 19) || (gpsx.utc.year > 39) || (gpsx.utc.month > 13) || (gpsx.SPEED > 5000))
	{
		initialize_all(2);
	}
	
	SPI_Flash_Sleep();
	
	if(Isdigit(GSM_P.IMEI,15) == false)													// 检查IMEI号是否为数字，如不是重新读取
	{
		MCU_F.b.Imei_Err_Flag = true;
	}			
	
	Set_Track_Time();																	// 获取连续定位时间及每天开启GPS电源时间	
					
	//读取历史数据条数，已发送的条数
	if(GSM_P.send_history>=GSM_P.store_history)
	{
		GSM_P.send_history=0;
		GSM_P.store_history=0;
	}	

#if (OTA_F==1)			
	for(u8 i=0;i<5;i++)	//检查版本
	{
		if(!isdigit(GSM_OTA.program_ver[i]))
		{
			GSM_OTA.program_ver[i]=0;break;		
		}
	}		
#endif
}

/* FLASH重设出厂设置		*/
void initialize_all(unsigned char Ota)
{	
	unsigned char row;
	
	if(Ota == 2)
	{
		SPI_Flash_Write(gpsadd,     (u8*)&default_gpsx,    gpsx_All_Size);
	}
	else
	{
		SPI_Flash_Erase_Sector(0);	//擦除第一个扇区		
		
		if(Ota == 1)				// 重置所有参数
		{
			SPI_Flash_Write(flags_addr,		(u8*)&default_flags,	flags_size);
			SPI_Flash_Write(GSM_paraddr,	(u8*)&default_GSM_P,   	GSM_para_size);
			SPI_Flash_Write(phoneaddr,  	(u8*)&default_GSM_ph,  	GSM_ph_size);
			SPI_Flash_Write(phonenameaddr,	(u8*)&default_phname,	phonenamesize);
			SPI_Flash_Write(gpsadd,     	(u8*)&default_gpsx,    	gpsx_All_Size);
			SPI_Flash_Write(otaaddr,		(u8*)&default_GSM_OTA, 	GSM_OTA_size);
		}
		else						// OTA升级后,重置部分参数
		{
			u8 eep_write_data[10]={0,0,0,0,0,0,0,0,0,0};
			
			eep_write_data[0] = GSMA.w;																	
			eep_write_data[1] = GSMB.w;																	
			eep_write_data[2] = GSMC.w;
#if DEBUG
			eep_write_data[7] = 0x7C;
#endif
			eep_write_data[8] = GSMD.w;
			
			SPI_Flash_Write(flags_addr, 	(u8*)&eep_write_data,   flags_size		);
			SPI_Flash_Write(GSM_paraddr,	(u8*)&GSM_P,			GSM_para_size);
			SPI_Flash_Write(phoneaddr,  	(u8*)&GSM_ph,			GSM_ph_size);
			SPI_Flash_Write(phonenameaddr,	(u8*)&GSM_phname,		phonenamesize);
			SPI_Flash_Write(gpsadd,     	(u8*)&gpsx,    			gpsx_All_Size);
			SPI_Flash_Write(otaaddr,		(u8*)&GSM_OTA,			GSM_OTA_size);		
		}
			
		SPI_Flash_Write(geoaddr,    		(u8*)&default_geoc,    	GEO_c_size);	
		
		for(row=0;row<5;row++)
		{
			SPI_Flash_Write(row*40+setgeoaddr,(u8*)"",1);							//清除GEO坐标
		}
	}
	
	SPI_Flash_Sleep();NVIC_SystemReset();
}




