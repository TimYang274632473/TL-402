#include <includes.h>

ECHO_Para_Def 	ECHO_F;

unsigned char Track_Time_Unit=10;				// ÿ��������λ����GPSʱ��			

unsigned char InValid_GPS_Count=1;				// ������ЧGPS����
	
unsigned char Bat_Val[3]={0,0,0};	
unsigned char Bat_Cnt=0;

/*���ò�����ش���*/

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

	"fffffffffffffff\0",   	// IMEI��
	"3gnet","TCP",
	"","",         			// APN
	"","",                 	// IP��ַ,// IP�˿�            	
	"","",					// ����				
	5,                   	// ������λ���ʱ�䣨����ʱ�䵥λ��
	'M',                 	// ������λʱ�䵥λ
	0x88,                   // ʱ��
	80,                  	// ������ֵ
	30,                  	// �͵���ֵ	
	180,                  	// ��������ʱ��
	100,                 	// Բ�ĵ���Χ���뾶
	22.7613506,          	// Բ�ĵ���Χ������γ��
	114.3610382,         	// Բ�ĵ���Χ�����ľ���
	0,                  	// �ѷ�����ʷ��������	
	0,                   	// �Ѵ�����ʷ��������
	9,                   	// ͨ������
	50,						// ���䷧ֵ
	0,						// ��ײ��ֵ
	0,
	12,0,					// ����ʱ,��
	"123456\0"            	// ָ�����룬��ʼֵ��123456
	
#else

	"fffffffffffffff\0",   	// IMEI��
	"3gnet","TCP",
	"","",         			// APN
	"","",                 	// IP��ַ,// IP�˿�            	
	"","",					// ����				
	5,                   	// ������λ���ʱ�䣨����ʱ�䵥λ��
	'M',                 	// ������λʱ�䵥λ
	0x88,                   // ʱ��
	80,                  	// ������ֵ
	30,                  	// �͵���ֵ	
	180,                  	// ��������ʱ��
	100,                 	// Բ�ĵ���Χ���뾶
	22.7613506,          	// Բ�ĵ���Χ������γ��
	114.3610382,         	// Բ�ĵ���Χ�����ľ���
	0,                  	// �ѷ�����ʷ��������	
	0,                   	// �Ѵ�����ʷ��������
	9,                   	// ͨ������
	0,						// ���䷧ֵ
	0,						// ��ײ��ֵ
	0,
	12,0,					// ����ʱ,��
	"123456\0"            	// ָ�����룬��ʼֵ��123456
	
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
	0x00,     				// OTA����״̬
	"",       				// OTA�����汾��
	0,        				// OTAʧ�ܴ���
	0 ,        				// MCU���ݵ�SPI״̬
	"",
	0
}; 

/*0x9BĬ�Ͽ�����GPRS,�������٣��͵籨����ʱ��+�����в���SOS*/

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
	{0,0,0,0},				// ��һ��GEO���㾭γ������
	{0,0,0,0},				// �ڶ���GEO���㾭γ������
	{0,0,0,0},				// ������GEO���㾭γ������
	{0,0,0,0},				// ������GEO���㾭γ������
	{0,0,0,0},				// ������GEO���㾭γ������
};	

cnt_1ms_def cnt_1ms={0,0,0,0,0,0,0,0};
cnt_1s_def  cnt_1s ={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	
Def_Flag_100mS	Tim_F;
MCU_F_def 		MCU_F;
	
/*	�ض���putc��������������ʹ��printf�����Ӵ��ڴ�ӡ���	*/
int fputc(int ch, FILE *f)
{	
	USART1_SendData((char)ch);
	return ch;
}

/* �Դ���1���͵����ַ�`									*/
void USART1_SendData(char dat)
{
	while(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TXE) == RESET);
	huart1.Instance->TDR=dat;
}
	
/* �Դ���2���͵����ַ�									*/
void USART2_SendData(char dat)
{
	while(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE) == RESET);
	huart2.Instance->TDR=dat;
}

/* �Ե͹��Ĵ���1�͵����ַ�								*/
void LUSART1_SendData(char dat)
{
	while(__HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_TXE) == RESET);
	hlpuart1.Instance->TDR=dat;
}

/* �Դ���4���͵����ַ�									*/
void USART4_SendData(char dat)
{
	while(__HAL_UART_GET_FLAG(&huart4, UART_FLAG_TXE) == RESET);
	huart4.Instance->TDR=dat;
}

//��ȡEEPROM���������
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
	SPI_Flash_Read(geoaddr,		(u8*)&GEO_c,	GEO_c_size		);						//��ȡ5�����GEO�洢���� 
#endif
	
	SPI_Flash_Read(otaaddr, 	(u8*)&GSM_OTA,	GSM_OTA_size	);
	
	GSMA.w=eep_data[0];																	// ��ȡGPRS���ӱ�־
	GSMB.w=eep_data[1];																	// ����ͨ����������GPS����ģʽ�Ȳ���	
	GSMC.w=eep_data[2];	
#if (GEO_FUN1==1)																		// 5�����Χ������	
	GEOA.w=eep_data[3];																	// ����GEO�򿪱�־
	GEOB.w=eep_data[4];																	// ����GEO��ȥ��־
	GEOC.w=eep_data[5];																	// ����GEO�����־
	GEOD.w=eep_data[6];																	// ����GEO������Ч��־  
#endif	
	ECHO_F.w=eep_data[7];	
	GSMD.w=eep_data[8];
	
	if((GSM_OTA.OTA_state&0x11) == 0x10)												// ���APP�����ɹ�,��Ҫ���������Ĭ������
	{
		GSM_OTA.OTA_state = GSM_OTA.OTA_state & 0xEF;
		initialize_all(0);																// ��OTA��,��������Ĭ�ϲ���
	}
	
	if(GSM_OTA.OTA_ph > 7)
	{
		GSM_OTA.OTA_ph = 0;
		SPI_Flash_Write(otaaddr,		(u8*)&GSM_OTA,			GSM_OTA_size);	
	}
	
	//��������ʽ���󣨳������»�����һ�����У�����Ĭ������123456����	
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
	
	if(Isdigit(GSM_P.IMEI,15) == false)													// ���IMEI���Ƿ�Ϊ���֣��粻�����¶�ȡ
	{
		MCU_F.b.Imei_Err_Flag = true;
	}			
	
	Set_Track_Time();																	// ��ȡ������λʱ�估ÿ�쿪��GPS��Դʱ��	
					
	//��ȡ��ʷ�����������ѷ��͵�����
	if(GSM_P.send_history>=GSM_P.store_history)
	{
		GSM_P.send_history=0;
		GSM_P.store_history=0;
	}	

#if (OTA_F==1)			
	for(u8 i=0;i<5;i++)	//���汾
	{
		if(!isdigit(GSM_OTA.program_ver[i]))
		{
			GSM_OTA.program_ver[i]=0;break;		
		}
	}		
#endif
}

/* FLASH�����������		*/
void initialize_all(unsigned char Ota)
{	
	unsigned char row;
	
	if(Ota == 2)
	{
		SPI_Flash_Write(gpsadd,     (u8*)&default_gpsx,    gpsx_All_Size);
	}
	else
	{
		SPI_Flash_Erase_Sector(0);	//������һ������		
		
		if(Ota == 1)				// �������в���
		{
			SPI_Flash_Write(flags_addr,		(u8*)&default_flags,	flags_size);
			SPI_Flash_Write(GSM_paraddr,	(u8*)&default_GSM_P,   	GSM_para_size);
			SPI_Flash_Write(phoneaddr,  	(u8*)&default_GSM_ph,  	GSM_ph_size);
			SPI_Flash_Write(phonenameaddr,	(u8*)&default_phname,	phonenamesize);
			SPI_Flash_Write(gpsadd,     	(u8*)&default_gpsx,    	gpsx_All_Size);
			SPI_Flash_Write(otaaddr,		(u8*)&default_GSM_OTA, 	GSM_OTA_size);
		}
		else						// OTA������,���ò��ֲ���
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
			SPI_Flash_Write(row*40+setgeoaddr,(u8*)"",1);							//���GEO����
		}
	}
	
	SPI_Flash_Sleep();NVIC_SystemReset();
}




