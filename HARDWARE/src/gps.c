#include <includes.h>

Def_Flag_GPS	GPS_F;
nmea_msg		gpsx;
nmea_msg		gpsx_his_read;
nmea_msg		gpsx_his_write;
	
char GPS_RMC_Buf[GPS_Buf_Len]={0};	// GPSRMC�洢����
char GPS_GGA_Buf[GPS_Buf_Len]={0};	// GPSGGA�洢����
char GPS_Str_Buf[6]={0};			// GPS����ͷ����
unsigned char GPS_Rx_Cnt=0; 		// GPS�����������

unsigned char open_gps_count=1;				// ��ǰ��GPSʱ�����������������Чʱ��������open_gps_count
unsigned char gps_a_count=0;
unsigned char gps_v_count=0;				// ÿ�ζ�GPS��Ч���������������Ч��InValid_GPS_Count��ʼ����

char dtbuf[dt_len]={0};						// ���淢��������������Ч����

const char MTK_gpscfgstr[]="$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n";		// ����GGA RMC
const char MTK_gpscfgall[]="$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0*29\r\n";		// ������������

const unsigned char UBLOX_gpsclsvtg[]={0x24,0x45,0x49,0x47,0x50,0x51,0x2c,0x56,0x54,0x47,0x2a,0x32,0x33,0x0d,0x0a,0xb5,0x62,0x06,0x01,0x03,0x00,0xf0,0x05,0x00,0xff,0x19};		// �ر�VTG
const unsigned char UBLOX_gpsclsgsa[]={0x24,0x45,0x49,0x47,0x50,0x51,0x2c,0x47,0x53,0x41,0x2a,0x33,0x33,0x0d,0x0a,0xb5,0x62,0x06,0x01,0x03,0x00,0xf0,0x02,0x00,0xfc,0x13};		// �ر�GSA
const unsigned char UBLOX_gpsclsgsv[]={0x24,0x45,0x49,0x47,0x50,0x51,0x2c,0x47,0x53,0x56,0x2a,0x32,0x34,0x0d,0x0a,0xb5,0x62,0x06,0x01,0x03,0x00,0xf0,0x03,0x00,0xfd,0x15};		// �ر�GSV
const unsigned char UBLOX_gpsclsgll[]={0x24,0x45,0x49,0x47,0x50,0x51,0x2c,0x47,0x4c,0x4c,0x2a,0x32,0x31,0x0d,0x0a,0xb5,0x62,0x06,0x01,0x03,0x00,0xf0,0x01,0x00,0xfb,0x11};		// ��GLL

//const unsigned char UBLOX_char gpsrst[]   ={0xB5,0x62,0x06,0x04,0x04,0x00,0xFF,0x87,0x01,0x00,0x95,0xF7};																				// ��λ

const unsigned char UBLOX_gpsopenvtg[] = {0x24,0x45,0x49,0x47,0x50,0x51,0x2c,0x56,0x54,0x47,0x2a,0x32,0x33,0x0d,0x0a,0xb5,0x62,0x06,0x01,0x03,0x00,0xf0,0x05,0x01,0x00,0x1a};		// ����VTG					
const unsigned char UBLOX_gpsopengsa[] = {0x24,0x45,0x49,0x47,0x50,0x51,0x2c,0x47,0x53,0x41,0x2a,0x33,0x33,0x0d,0x0a,0xb5,0x62,0x06,0x01,0x03,0x00,0xf0,0x02,0x01,0xfd,0x14};		// ����GSA
const unsigned char UBLOX_gpsopengsv[] = {0x24,0x45,0x49,0x47,0x50,0x51,0x2c,0x47,0x53,0x56,0x2a,0x32,0x34,0x0d,0x0a,0xb5,0x62,0x06,0x01,0x03,0x00,0xf0,0x03,0x01,0xfe,0x16};		// ����GSV
const unsigned char UBLOX_gpsopengll[] = {0x24,0x45,0x49,0x47,0x50,0x51,0x2c,0x47,0x4c,0x4c,0x2a,0x32,0x31,0x0d,0x0a,0xb5,0x62,0x06,0x01,0x03,0x00,0xf0,0x01,0x01,0xfc,0x12};		// ����GGA
	
const unsigned char UBLOX_gpssave[]  ={0xB5,0x62,0x06,0x09,0x0D,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x17,0x31,0xBF};

/* GPS���������� */
void GPS_Mode_Setting(void)
{
	GPS_Force_On(); vTaskDelay(1000);
	
	if(GSMC.b.GPS_Tran_Mode_Flag)
	{
		HAL_UART_Transmit(&huart2,(uint8_t*)MTK_gpscfgall,strlen((char*)MTK_gpscfgall),1000);
		
		HAL_UART_Transmit(&huart2,(uint8_t*)UBLOX_gpsopenvtg,26,1000);vTaskDelay(10);
		HAL_UART_Transmit(&huart2,(uint8_t*)UBLOX_gpsopengsa,26,1000);vTaskDelay(10);
		HAL_UART_Transmit(&huart2,(uint8_t*)UBLOX_gpsopengsv,26,1000);vTaskDelay(10);
		HAL_UART_Transmit(&huart2,(uint8_t*)UBLOX_gpsopengll,26,1000);vTaskDelay(10);
		HAL_UART_Transmit(&huart2,(uint8_t*)UBLOX_gpssave,21,1000);   vTaskDelay(10);
	}
	else
	{
		HAL_UART_Transmit(&huart2,(uint8_t*)MTK_gpscfgstr,strlen((char*)MTK_gpscfgstr),1000);
		
		HAL_UART_Transmit(&huart2,(uint8_t*)UBLOX_gpsclsvtg,26,1000);vTaskDelay(10);
		HAL_UART_Transmit(&huart2,(uint8_t*)UBLOX_gpsclsgsa,26,1000);vTaskDelay(10);
		HAL_UART_Transmit(&huart2,(uint8_t*)UBLOX_gpsclsgsv,26,1000);vTaskDelay(10);
		HAL_UART_Transmit(&huart2,(uint8_t*)UBLOX_gpsclsgll,26,1000);vTaskDelay(10);
		HAL_UART_Transmit(&huart2,(uint8_t*)UBLOX_gpssave,21,1000);  vTaskDelay(10);
	}
	
	GPS_Power_Off();
}

//���ҵ�cx�����ţ����ظö��ŵĵ�ַ
u8 NMEA_Comma_Pos(u8 *buf,u8 cx)   
{	 		    
	u8 *p=buf;
	while(cx)
	{		 
		if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;//����'*'���߷Ƿ��ַ�,�򲻴��ڵ�cx������
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;	 
}

//����m��n�η�
u32 NMEA_Pow(u8 m,u8 n) 
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

//�ַ���ת����
int NMEA_Str2num(u8 *buf,u8*dx)
{
	u8 *p=buf;
	u32 ires=0,fres=0;
	u8 ilen=0,flen=0,i;
	u8 mask=0;
	int res;
	while(1) 						// �õ�������С���ĳ���
	{
		if(*p=='-'){mask|=0X02;p++;}// �Ǹ���
		if(*p==','||(*p=='*'))break;// ����������
		if(*p=='.'){mask|=0X01;p++;}// ����С������
		else if(*p>'9'||(*p<'0')) 	// �зǷ��ַ�
		{	
			ilen=0;
			flen=0;
			break;
		}	
		if(mask&0X01)flen++;
		else ilen++;
		p++;
	}
	if(mask&0X02)buf++;				// ȥ������
	for(i=0;i<ilen;i++)				// �õ�������������
	{  
		ires+=NMEA_Pow(10,ilen-1-i)*(buf[i]-'0');
	}
	if(flen>5)flen=5;	  			// ���ȡ5λС��
	*dx=flen;	 		      		// С����λ��
	for(i=0;i<flen;i++)				// �õ�С����������
	{  
		fres+=NMEA_Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
	} 
	res=ires*NMEA_Pow(10,flen)+fres;
	if(mask&0X02)res=-res;		   
	return res;
}


//����GPGGA��Ϣ ʾ����$GPGGA,062320,3537.8333,N,13944.6667,E,0,00,99.9,0100,M,,M,000,0000*7D
void NMEA_GPGGA_Analysis(void)
{
	u8 *p1,dx,posx;   
		
	p1=(u8*)strstr(GPS_GGA_Buf,"GGA");
	
	if(p1 == NULL)	return;
	
	posx=NMEA_Comma_Pos(p1,7);						// �õ����ڶ�λ��������						
	if(posx!=0XFF)
	{
		gpsx.posslnum=NMEA_Str2num(p1+posx,&dx);
		if(gpsx.posslnum > 12) {gpsx.posslnum = 12;		}
		if(gpsx.posslnum == 0) {return;					}
	}
		
	posx=NMEA_Comma_Pos(p1,8);						// �õ����ڶ�λ��ˮƽ����						
	if(posx!=0XFF)
	{
		float level_tmp=0;
		level_tmp=NMEA_Str2num(p1+posx,&dx);
		level_tmp=level_tmp/NMEA_Pow(10,dx);
		gpsx.level=(u16)level_tmp*100;
		if(gpsx.level > 10000){gpsx.level = 0;return;	}
	}
	
#if 0		
	posx=NMEA_Comma_Pos(p1,9);						// �õ����θ߶�				
	if(posx!=0XFF)
	gpsx.altitude=NMEA_Str2num(p1+posx,&dx); 
#endif
} 

//����GPRMC��Ϣ ʾ����$GPRMC,062320,V,3537.8333,N,13944.6667,E,000.0,000.0,030222,,*0D
u8 NMEA_GPRMC_Analysis(void)
{
	u8 *p1,*p2,dx,posx;	  
	u32 temp=0,Lat_Lon=0,Lat_Lon_xx;
	float rs=0;  
		
	p1=(u8*)strstr(GPS_RMC_Buf,"RMC");	
	if(p1==0) 		{return 0;				}
	
	posx=NMEA_Comma_Pos(p1,2);									// �õ������Ƿ���Ч��־									
	if(posx!=0XFF)
	{	
		gpsx.AV_indicate=*(p1+posx);							// ���ʹ�GPS��־
		if(gpsx.AV_indicate == 'A')
		{
			gps_v_count=0;
		}
		else
		{
			gps_a_count = 0; gpsx.AV_indicate = 'V';
			
			if(gps_v_count>250)
			{				
				gps_v_count=0;									// ����ģ��
				
				if(GSMB.b.GPS_StrongMode_Flag)
				{
					GPS_Force_Off();vTaskDelay(500);GPS_Force_On();
				}
			}	

			if((gpsx.posslnum == 0) || ((GSMB.b.RTC_Update_Flag == true) && (GPS_F.b.Gps_time_update_Flag == true)))
			{
				return 0;
			}
		}
	}
	else 
	{
		return 0;
	}
		
	if((GSMB.b.RTC_Update_Flag) && (GPS_F.b.Gps_time_update_Flag) && (gpsx.posslnum <= 4))	{return 0;	}

	posx=NMEA_Comma_Pos(p1,1);									//�õ�UTCʱ��								
	if(posx!=0XFF)
	{
		p2=(u8*)strstr((const char *)p1,",");						 		
		posx=(p2[1]-0x30)*10+(p2[2]-0x30);
		if(posx>23) {return 0;				}
		else		{gpsx.utc.hour=posx;	}
		
		posx= (p2[3]-0x30)*10+(p2[4]-0x30);
		if(posx>59) {return 0;				}
		else		{gpsx.utc.min=posx;		}
		
		posx= (p2[5]-0x30)*10+(p2[6]-0x30); 
		if(posx>59)	{return 0;				}
		else		{gpsx.utc.sec=posx;		}
	}
	else 
	{
		return 0;	
	}
	
	posx=NMEA_Comma_Pos(p1,9);									//�õ�UTC����							
	if(posx!=0xFF)
	{		
		temp=NMEA_Str2num(p1+posx,&dx);	
		
		posx=temp/10000;
		if(posx<1 || posx>31)	{return 0;					}
		else 					{gpsx.utc.date = posx;		}
		
		posx=(temp/100)%100;
		if(posx<1 || posx>12 )	{return 0;					}
		else					{gpsx.utc.month = posx;		}
		
		posx=temp%100;	 
		if(posx<18|| posx>38) 	{return 0;  				}
		else					{gpsx.utc.year = posx;		}
	}
	else 
	{
		return 0;
	}
		
	if((!GSMB.b.RTC_Update_Flag) || (!GPS_F.b.Gps_time_update_Flag))		// ������յ�GPS��Ч���ݲ���δ���¹�RTCʱ��
	{
		if(gpsx.utc.year >= 18)
		{
			cnt_1s.GPS_Time_Update_Cnt = 0;GPS_F.b.Gps_time_update_Flag = true;
			
			if(GSMB.b.RTC_Update_Flag  == false)
			{
				GSMB.b.RTC_Update_Flag = true;
				Task_SPI_Flash(Flash_Write,1, (u8*)&GSMB, 1);		// ��������
			}
			
			RTC_Write_Year_Time(gpsx.utc.year, gpsx.utc.month ,gpsx.utc.date, gpsx.utc.hour, gpsx.utc.min,gpsx.utc.sec);	// ˢ��RTCʱ��
		}
	}
	    
	if((gpsx.AV_indicate != 'A') || (gpsx.posslnum <= 4))	{return 0;	}
	
	posx=NMEA_Comma_Pos(p1,3);									// �õ�γ��								
	if(posx!=0XFF)
	{				
		temp = NMEA_Str2num(p1+posx,&dx);							// �õ���		
		Lat_Lon = temp / NMEA_Pow(10,dx+2);							// �õ�'		
		Lat_Lon_xx = temp % NMEA_Pow(10,dx+2);								// ת��Ϊ��		
		Lat_Lon = Lat_Lon * NMEA_Pow(10,7) + (Lat_Lon_xx * NMEA_Pow(10,7-dx))/60; 
		
		if(Lat_Lon < 900000000)						
		{
			gpsx.latitude=Lat_Lon;
		}
		else
		{
			return 0;
		}
	}
	else 
	{
		return 0;
	}
	 
	posx=NMEA_Comma_Pos(p1,4);									// ��γ���Ǳ�γ						
	if(posx!=0XFF){gpsx.nshemi=*(p1+posx); }
	if((gpsx.nshemi != 'N')&&(gpsx.nshemi != 'S')){return 0;}
	
	posx=NMEA_Comma_Pos(p1,5);									// �õ�����								
	if(posx!=0XFF)
	{		
		temp = NMEA_Str2num(p1+posx,&dx);							// �õ���			
		Lat_Lon = temp/NMEA_Pow(10,dx+2);							// �õ�'			
		Lat_Lon_xx = temp % NMEA_Pow(10,dx+2);								// ת��Ϊ�� 		
		Lat_Lon = Lat_Lon * NMEA_Pow(10,7) + (Lat_Lon_xx * NMEA_Pow(10,7-dx))/60;
		if(Lat_Lon < 1800000000)							    
		{
			gpsx.longitude=Lat_Lon;
		}
		else
		{
			return 0;
		}
	}
	else 
	{
		return 0;
	}
	
	posx=NMEA_Comma_Pos(p1,6);									// ������������								
	if(posx!=0XFF) gpsx.ewhemi=*(p1+posx);
	if((gpsx.ewhemi!='W')&&(gpsx.ewhemi!='E')){return 0;}
	
	posx=NMEA_Comma_Pos(p1,7);									// ���λ���ٶ�,1.852�Ǻ����빫��Ļ���ϵ��,�ѷŴ�ʮ��              
	if(posx!=0XFF)
	{
		rs=NMEA_Str2num(p1+posx,&dx)*1852/1000;	
		rs=rs/NMEA_Pow(10,dx);			 
		if(rs>500)			 	{return 0;	 				}
		else					{gpsx.SPEED=(u16)(rs*10);	}
	}
	else 
	{
		return 0;
	}
	
	posx=NMEA_Comma_Pos(p1,8);									// ���λ�Ʒ���             
	if(posx!=0XFF) 
	{		
		temp = (u16)((NMEA_Str2num(p1+posx,&dx)/NMEA_Pow(10,dx)));	

		if(temp <= 360)
		{
			if((gpsx.SPEED >= 20) && (*(p1+posx) != ','))
			{
				gpsx.course=(u16)temp;
			}
			
			if(gpsx.course >360)	{gpsx.course = 0;}
		}
		else
		{
			return 0;
		}
	}
	else 
	{
		return 0;
	}
				
	return 1;													// ��������1
}

/* ����GPS����			*/
void New_GPS_Receive(void)
{
	GPS_Rx_Cnt=0;
	GPS_GGA_Buf[0]=0;GPS_RMC_Buf[0]=0;							// �����ʱ����־
	GPS_F.b.GPGGA_Flag=0;GPS_F.b.GPGGA_END_Flag=0;				
	GPS_F.b.GPRMC_Flag=0;GPS_F.b.GPRMC_END_Flag=0;
		
	xEventGroupClearBits(EventGroupHandler_OTH,Gps_GPRMC_GGA_OK_Flag);
	
	NVIC_EnableIRQ(USART2_IRQn);
}

/* GPS���ݽ���			*/
bool New_GPS_Handle(void)
{
	unsigned char i=0;
	unsigned char readgprmc_ok = 6;
	unsigned char readgprmc_count = 10;
	bool GPS_Now_Flag=false;												// ��ǰGPSĬ����Ч״̬

	GPS_Power_On();
		
	if((GPS_F.b.Track_For_gprs_flag || GPS_F.b.Track_For_sms_Flag))
	{
		if(cnt_1s.Track_Time_Set <= 20)
		{
			readgprmc_count = 1;readgprmc_ok = 1;
		}
		else
		{
			readgprmc_count = 10;readgprmc_ok = 2;
		}
	}
	
	for(i=0;i<readgprmc_count;i++)
	{	
		if(((!GPS_F.b.Track_For_gprs_flag) && (!GPS_F.b.Track_For_sms_Flag)) || ((GPS_F.b.Track_For_gprs_flag || GPS_F.b.Track_For_sms_Flag) && (cnt_1s.Track_Time_Set >= 5)))
		{
			New_GPS_Receive();xEventGroupWaitBits(EventGroupHandler_OTH, Gps_GPRMC_GGA_OK_Flag, pdFALSE,pdFALSE,1500);	// ��������	
		}
		
		NMEA_GPGGA_Analysis();											// GPGGA���ݽ���
		
		if(NMEA_GPRMC_Analysis())										// GPRMC���ݽ���,ͳ����Ч����	
		{
			if(++gps_a_count >= readgprmc_ok)								
			{		
				gps_a_count = 0; GPS_Now_Flag=true; GPS_F.b.AV_Flag = true;
												
				if(open_gps_count > 1){open_gps_count--;	}
				
				InValid_GPS_Count=1;cnt_1s.Chk_GPS_Ten_Cnt = 0;			// ��ȥʮ����һ�εĶ�ȡGPS
				
				cnt_1s.Track_wifi_Cnt = 0;
				
				break;													// ���������򱾴ζ�ȡ��һ����ЧGPS���ݣ����Դ����GPS�����йصı�����GEO\SPEED��								
			}
		}
	}
	
	if(GSMA.b.Track_Flag && (cnt_1s.Track_Time_Set < 5)) {New_GPS_Receive();}
		
	GPS_Power_Off();
	
	if((GPS_Now_Flag == false)||(gpsx.SPEED > 5000))
	{		
		GPS_Now_Flag = false;gpsx.AV_indicate = 'V';
		
		if(++open_gps_count > 4){open_gps_count=4; GPS_F.b.AV_Flag = false;	}					
	}
	
	return GPS_Now_Flag;
}

//��ȡ����GPS���ݣ������Ч������һ�����ݳ�ȥ��
//��Ч��־��V,flag1��־�Ƿ����GPS,0������
//GPS_Format���ݸ�ʽ: 1:GPRS��ʽ; 2:GOOGLE��ʽ
void Read_GPS_data(unsigned char New_Old_GPS,unsigned char MSG_Type)
{ 
	nmea_utc_time gpsx_t;
	
	if(New_Old_GPS == HISGPS)												// ��ȡ��ʷ����
	{
		char gsmbase2[9]={0};												// ���վ2
		
		if(gpsx.gsmbase2 <= 0xFFFF)
		{
			snprintf(gsmbase2,5,"%04X",gpsx_his_read.gsmbase2);
		}
		else
		{
			snprintf(gsmbase2,9,"%08X",gpsx_his_read.gsmbase2);
		}

		snprintf(dtbuf,dt_len,"%1c,DATE:%02d%02d%02d,TIME:%02d%02d%02d,LAT:%02d.%07d%1c,LOT:%03d.%07d%1c,Speed:%d.%1d,%d-%d-%d-0-%d-%02d,%03d,%05d-%04X-%s,%02d,%d.%02d,%02d.0",\
		gpsx_his_read.AV_indicate,gpsx_his_read.utc.year%100,gpsx_his_read.utc.month,gpsx_his_read.utc.date,gpsx_his_read.utc.hour,gpsx_his_read.utc.min,gpsx_his_read.utc.sec,\
		gpsx_his_read.latitude/10000000,gpsx_his_read.latitude%10000000,gpsx_his_read.nshemi,gpsx_his_read.longitude/10000000,gpsx_his_read.longitude%10000000,gpsx_his_read.ewhemi,\
		gpsx_his_read.SPEED/10,gpsx_his_read.SPEED%10,MCU_F.b.Power_Connected_Flag,!GSMB.b.GPS_PowerOffMode_Flag,MCU_F.b.Charging_Flag,gpsx_his_read.soc,gpsx_his_read.sig_strength,gpsx_his_read.course,gpsx_his_read.operators,gpsx_his_read.gsmbase1,gsmbase2,\
		gpsx_his_read.posslnum,gpsx_his_read.level/100,gpsx_his_read.level%100,temperature);
			
		return;
	}
	
	if((GSMB.b.GPS_PowerOffMode_Flag) || (New_Old_GPS == OLDGPS))			// δ������GPS��Դ	
	{
		gpsx.AV_indicate='V';
	}							
	
	memcpy(&gpsx_t,&gpsx.utc,5);
		
	UTC_RTC(&gpsx_t.year,&gpsx_t.month,&gpsx_t.date,&gpsx_t.hour,&gpsx_t.min,GSM_P.TimeZone);
	
	if(MSG_Type == MSG_GOOGLE)												// ����GOOGLE��ʽ
	{
		char JD[1]="";
		char WD[1]="";
		
		if(gpsx.nshemi == 'S')	{WD[0]='-';	}
		
		if(gpsx.ewhemi == 'W')	{JD[0]='-';	}

		snprintf(dtbuf,120,"%1c,DATE:%02d/%02d/%02d\nTIME:%02d:%02d:%02d\nSPEED:%d.%1d\nBAT:%d\nCSQ:%02d \nhttp://maps.google.com/maps?f=q&hl=en&q=%s%d.%07d,%s%d.%07d",\
		gpsx.AV_indicate,gpsx_t.year,gpsx_t.month,gpsx_t.date,gpsx_t.hour,gpsx_t.min,gpsx.utc.sec,gpsx.SPEED/10,gpsx.SPEED%10,\
		gpsx.soc,gpsx.sig_strength,WD,gpsx.latitude/10000000,gpsx.latitude%10000000,JD,gpsx.longitude/10000000,gpsx.longitude%10000000);
	}
	else if(MSG_Type != MSG_NO)										// ��Ҫ��ӡ����
	{
		char gsmbase2[9]={0};										// ���վ2
		
		if(gpsx.gsmbase2 <= 0xFFFF)
		{
			snprintf(gsmbase2,5,"%04X",gpsx.gsmbase2);
		}
		else
		{
			snprintf(gsmbase2,9,"%08X",gpsx.gsmbase2);
		}
		
		snprintf(dtbuf,dt_len,"%1c,DATE:%02d%02d%02d,TIME:%02d%02d%02d,LAT:%02d.%07d%1c,LOT:%03d.%07d%1c,Speed:%d.%1d,%d-%d-%d-0-%d-%02d,%03d,%05d-%04X-%s,%02d,%d.%02d,%02d.0",\
		gpsx.AV_indicate,gpsx_t.year,gpsx_t.month,gpsx_t.date,gpsx_t.hour,gpsx_t.min,gpsx.utc.sec,gpsx.latitude/10000000,gpsx.latitude%10000000,\
		gpsx.nshemi,gpsx.longitude/10000000,gpsx.longitude%10000000,gpsx.ewhemi,gpsx.SPEED/10,gpsx.SPEED%10,MCU_F.b.Power_Connected_Flag,!GSMB.b.GPS_PowerOffMode_Flag,MCU_F.b.Charging_Flag,gpsx.soc,gpsx.sig_strength,gpsx.course,gpsx.operators,gpsx.gsmbase1,gsmbase2,gpsx.posslnum,gpsx.level/100,gpsx.level%100,temperature);
	} 
}

/* UTCʱ��ת��Ϊ��ʱ��ʱ��*/
void UTC_RTC(u8 *year,u8 *month,u8 *day,u8 *hour,u8 *min,u8 zone)
{
	unsigned char MinZone=0;
	bool time_add=0,time_sub=0;
	
	switch(zone&0x60)
	{
		case 0x20: MinZone=15;break;
		case 0x40: MinZone=30;break;
		case 0x60: MinZone=45;break;
		default:   MinZone=0; break;
	}
	
	if(zone & 0x80)											// ��ʱ������ʱ,��ʱ��
	{
		*min += MinZone;
		
		if(*min > 59)										
		{
			*min -= 60;
			time_add = 1;
		}
		
		*hour += (zone&0x1F) + time_add;					// ����ʱ
		time_add = 0;										// ֱ������ֽ�λ
		
		if(*hour > 23)										// ���ʱ����24Сʱ
		{
			*hour -= 24;time_add=1;							// ʱ���н�λ
		}
	}
	else													// ��ʱ��
	{
		if(*min < MinZone)
		{
			*min += 60;time_sub = 1;						// ���н�λ
		}
		
		*min -= MinZone;									// �����
					
		*hour = 24 + *hour - (zone&0x1F) - time_sub;
		
		time_sub = 0;
		
		if(*hour > 23)										// ʱ��û�н�λ
		{
			*hour -= 24;
		}
		else												// ʱ���н�λ
		{
			time_sub = 1;
		}
	}
	
	if(time_add)											// ���ʱ���н�λ
	{
		(*day)++;											// �ռ�1
		
		if((*day == 29)&&(*month == 2)&&(((*year) + 2000) % 4 != 0))							
		{
			*day=1;*month=3;								// �ո�Ϊһ�ţ��½�һλ	
		}
		else if((*day == 30)&&(*month == 2)&&(((*year) + 2000) % 4 == 0))						
		{
			*day=1;*month=3;								// �ո�Ϊһ�ţ��½�һλ	
		}
		else if((*day == 31) && ((*month == 4)||(*month == 6)||(*month == 9)||(*month == 11)))		
		{
			*day=1;(*month)++;								// �ո�Ϊһ�ţ��½�һλ
		}
		else if(*day >= 32)
		{
			*day=1;(*month)++;
			if((*month) > 12)								// ����Ƕ���,��λ����12�£����Ϊһ�£����ٽ�λ
			{
				(*month)=1;(*year)++;
			}
		}
	}
	else if(time_sub)										// ���ʱ���н�λ
	{
		(*day)--;
		
		if((*day) == 0)										// ������Ѽ���0��
		{
			(*day)=31;(*month)--;							// ������һλ
			
			if((*month) == 0)								// ���ԭ����һ��
			{
				(*month)=12;(*year)--;						// �˻ص�12�£�������һλ
			}
			else if((*month) == 2)							// ���ԭ��������
			{
				if(((*year) +2000)%4)						// �����ƽ����
				{
					*day=28;
				}
				else										// ����������
				{
					*day=29;
				}
			}
			else if((*month == 4)||(*month == 6)||(*month == 9)||(*month == 11))
			{
				*day = 30;
			}
		}
	}
}

