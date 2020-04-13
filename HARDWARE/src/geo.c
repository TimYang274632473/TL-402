#include <includes.h>

GEO_A_def GEOA;
GEO_B_def GEOB;
GEO_C_def GEOC;
GEO_D_def GEOD;
GEO_coordinate GEO_c[5];										// 5组矩形GEO

float lat_new;
float lot_new;

float lat_saving;
float lot_saving;

void GPS_Alarm_Handle(void)
{		
	if(((GEOA.w&0x1f) == 0) && !GSMA.b.ALM_Circle_GEO_Flag && !GSMA.b.ALM_OverSpeed_Flag && !GSMB.b.GPRS_SaveMode_Flag)	// 如果没有设置报警	
	{
		return;
	}
	
	lot_new=(float)gpsx.longitude/10000000;
	if(gpsx.ewhemi=='W') {lot_new=-lot_new;}

	lat_new=(float)gpsx.latitude/10000000;
	if(gpsx.nshemi=='S') {lat_new=-lat_new;}
			
#if GEO_FUN1					
	//监控有无设置矩形GEO报警，组1-组5		
	if(GPS_F.b.Send_Geo1_ALM_flag)
	{
		if(GEO_alarm_analysis(GEOC.b.GEO1_IN_flag,GEOB.b.GEO1_OUT_flag,GEO_c[0].lat1,GEO_c[0].lot1,GEO_c[0].lat2,GEO_c[0].lot2,1))		// 组1
		{
			cnt_1s.Chk_GEO1_Cnt = 0;GPS_F.b.Send_Geo1_ALM_flag = false;
		}
	}
	if(GPS_F.b.Send_Geo2_ALM_flag)
	{
		if(GEO_alarm_analysis(GEOC.b.GEO2_IN_flag,GEOB.b.GEO2_OUT_flag,GEO_c[1].lat1,GEO_c[1].lot1,GEO_c[1].lat2,GEO_c[1].lot2,2))		// 组2	
		{
			cnt_1s.Chk_GEO2_Cnt = 0;GPS_F.b.Send_Geo2_ALM_flag = false;
		}
	}
	if(GPS_F.b.Send_Geo3_ALM_flag)
	{
		if(GEO_alarm_analysis(GEOC.b.GEO3_IN_flag,GEOB.b.GEO3_OUT_flag,GEO_c[2].lat1,GEO_c[2].lot1,GEO_c[2].lat2,GEO_c[2].lot2,3))		// 组3	
		{
			cnt_1s.Chk_GEO3_Cnt = 0;GPS_F.b.Send_Geo3_ALM_flag = false;
		}
	}
	if(GPS_F.b.Send_Geo4_ALM_flag)
	{
		if(GEO_alarm_analysis(GEOC.b.GEO4_IN_flag,GEOB.b.GEO4_OUT_flag,GEO_c[3].lat1,GEO_c[3].lot1,GEO_c[3].lat2,GEO_c[3].lot2,4))		// 组4	
		{
			cnt_1s.Chk_GEO4_Cnt = 0;GPS_F.b.Send_Geo4_ALM_flag = false;
		}
	}
	if(GPS_F.b.Send_Geo5_ALM_flag)
	{ 
		if(GEO_alarm_analysis(GEOC.b.GEO5_IN_flag,GEOB.b.GEO5_OUT_flag,GEO_c[4].lat1,GEO_c[4].lot1,GEO_c[4].lat2,GEO_c[4].lot2,5))		// 组5
		{
			cnt_1s.Chk_GEO5_Cnt = 0;GPS_F.b.Send_Geo5_ALM_flag = false;
		}
	}
#endif	
	
#if GEO_FUN2			 
	//监控有无设置圆形GEO报警，组6			 
	if(GPS_F.b.Send_Geo6_ALM_flag)
	{
		if((get_length(lot_new,lat_new,GSM_P.center_lot, GSM_P.center_lat)/10)>GSM_P.geo_radius)									// 如果实际距离超过了设置的距离，则发送报警信息到手机和平台
		{
			send_alarm(0x01,"ALM-Movement!",ALM_H,ALMGPS);	

			cnt_1s.Chk_GEO6_Cnt = 0;GPS_F.b.Send_Geo6_ALM_flag = false;			
		}
	}	
#endif
				
	//监控有无超速报警 ALM-C	
	if(GSMA.b.ALM_OverSpeed_Flag)
	{
		if(!GPS_F.b.Send_overspd_ALM_flag)
		{	
			if((gpsx.SPEED/10) > GSM_P.set_speed)																						// 如果实际行驶速度超过了设置的速度，则发送报警信息到手机和平台
			{								
				GPS_F.b.Send_overspd_ALM_flag = true;

				send_alarm(0x01,"Over Speed!",ALM_C,ALMGPS);
			}				
		}
		else
		{
			if((gpsx.SPEED/10)<(GSM_P.set_speed-20)) 																					// 只有速度小于设置速度20Km/H，标志位才取消
			{
				GPS_F.b.Send_overspd_ALM_flag = false;
			}
		}
	}

	if(GSMB.b.GPRS_SaveMode_Flag)
	{
		if((get_length(lot_new,lat_new,lot_saving, lat_saving)) > 50)	
		{
			lot_saving = lot_new;
			lat_saving = lat_new;
			cnt_1s.Track_Save_Cnt = 0;	
			if(GPRS.b.GPRS_Save_now_Flag)	
			{
				GPRS.b.GPRS_Save_now_Flag = false;Set_Track_Time();
			}
		}
		else if((gpsx.SPEED/10) > 5)
		{
			cnt_1s.Track_Save_Cnt = 0;	
			if(GPRS.b.GPRS_Save_now_Flag)	
			{
				GPRS.b.GPRS_Save_now_Flag = false;Set_Track_Time();
			}
		}				
	}		
}

//GEO监控处理程序
#if (GEO_FUN1==1)
bool GEO_alarm_analysis(u8  _GEO_IN_flag,u8 _GEO_OUT_flag, float a,float b,float c,float d,unsigned char GEO_NUM)
{	
	bool geo_in_send_f=0;	
	bool geo_out_send_f=0;
	
	if(_GEO_IN_flag==1) 			//进入报警
	{   
		if((a>c)&&(b<d))
		{					 
			if((lat_new>c)&&(lat_new<a)&&(lot_new>b)&&(lot_new<d))	{geo_in_send_f=1;		}			
		}
		else if((a<c)&&(b>d))
		{
			if((lat_new>a)&&(lat_new<c)&&(lot_new>d)&&(lot_new<b))	{geo_in_send_f=1;		}
		}
		else if((a<c)&&(b<d))
		{
			if((lat_new>a)&&(lat_new<c)&&(lot_new>b)&&(lot_new<d))	{geo_in_send_f=1;		}
		}
		else if((a>c)&&(b>d))
		{
			if((lat_new>c)&&(lat_new<a)&&(lot_new>d)&&(lot_new<b))	{geo_in_send_f=1;		}
		}
		else{;}				 
	}
	else if(_GEO_OUT_flag==1) 		//出去报警
	{   			
		if(a>c)
		{	 
			if((lat_new>a)||(lat_new<c))	{geo_out_send_f=1;	}
		}
		else
		{
			if((lat_new>c)||(lat_new<a))	{geo_out_send_f=1;	}
		}
		if(b>d)
		{	 
			if((lot_new>b)||(lot_new<d))	{geo_out_send_f=1;	}
		}
		else
		{
			if((lot_new>d)||(lot_new<b))	{geo_out_send_f=1;	}
		}        				 
	}
	else{;}
				
	if(geo_in_send_f || geo_out_send_f)
	{
		switch(GEO_NUM)
		{
			case 1:send_alarm(0x01,"Stockade1!",ALM_B1,ALMGPS);break;
			case 2:send_alarm(0x01,"Stockade2!",ALM_B2,ALMGPS);break;
			case 3:send_alarm(0x01,"Stockade3!",ALM_B3,ALMGPS);break;
			case 4:send_alarm(0x01,"Stockade4!",ALM_B4,ALMGPS);break;
			case 5:send_alarm(0x01,"Stockade5!",ALM_B5,ALMGPS);break;
		}
		return true;
	}
	else
	{
		return false;
	}
}
#endif

#if (GEO_FUN2==1)			
//计算两点间距离，假设两点坐标经纬度，当前位置(x1,y1),历史位置(x2,y2)，X-经度，Y-纬度
float get_length(float x1,float y1, float x2, float y2) 
{
	float a,b,c,d;

	a=(float)((x1-x2)*cos((y1+y2)/2));
	b=(float)(y1-y2);
	c=(float)(pow(a,2)+pow(b,2));//平方和
	arm_sqrt_f32(c,&d);
	d*=111111.1111f;
	return d;	        
}
#endif

//把设置的电子围栏经纬度数据格式度分秒转成度，返回转换结果
float dms_to_d(char *str,u8 flag )
{
	float latlong=0;
	if(flag==1)//latitude
	{
		latlong=(float)(*str-0x30)*10+(*(str+1)-0x30);//度
		latlong+=(float)((*(str+2)-0x30)*10+(*(str+3)-0x30))/60;//分转成度
		latlong+=(float)((*(str+4)-0x30)*1000+(*(str+5)-0x30)*100+(*(str+6)-0x30)*10+(*(str+7)-0x30))/100/3600;//秒转成度
	}
	else //longitude
	{
		latlong=(float)((*(str+0)-0x30)*100+(*(str+1)-0x30)*10+(*(str+2)-0x30));//度
		latlong+=(float)((*(str+3)-0x30)*10+(*(str+4)-0x30))/60;//分转成度
		latlong+=(float)((*(str+5)-0x30)*1000+(*(str+6)-0x30)*100+(*(str+7)-0x30)*10+(*(str+8)-0x30))/100/3600;//秒转成度		 
	}
	return latlong;	 
}

/*发送报警信息str1-发送到手机内容 str2-发送到GPRS内容 flag获取GPS类型*/
void send_alarm(unsigned char Ph_Num,char *SMS_CMD,u8 GPRS_CMD,u8 GPS_Type)
{
#if GPRS_ENABLE	
	
	if(GSMA.b.GPRS_flag)		{Task_AT_GSM(Gprs_Send_Msg,0,0,GPRS_CMD,GPS_Type,MSG_GPRS,1);				}
	
#endif
	
#if SMS_ENABLE
	
	if((Ph_Num&0x01) && (*GSM_ph.temp_ph1!=0)) 	{Task_AT_GSM(Gsm_Send_SMS,GSM_ph.temp_ph1,SMS_CMD,0,GPS_Type,MSG_GOOGLE,0);	}	
	if((Ph_Num&0x02) && (*GSM_ph.temp_ph2!=0))	{Task_AT_GSM(Gsm_Send_SMS,GSM_ph.temp_ph2,SMS_CMD,0,GPS_Type,MSG_GOOGLE,0);	}
	if((Ph_Num&0x04) && (*GSM_ph.temp_ph3!=0)) 	{Task_AT_GSM(Gsm_Send_SMS,GSM_ph.temp_ph3,SMS_CMD,0,GPS_Type,MSG_GOOGLE,0);	}
	if((Ph_Num&0x08) && (*GSM_ph.temp_ph4!=0)) 	{Task_AT_GSM(Gsm_Send_SMS,GSM_ph.temp_ph4,SMS_CMD,0,GPS_Type,MSG_GOOGLE,0);	}
	if((Ph_Num&0x10) && (*GSM_ph.temp_ph5!=0)) 	{Task_AT_GSM(Gsm_Send_SMS,GSM_ph.temp_ph5,SMS_CMD,0,GPS_Type,MSG_GOOGLE,0);	}
	if((Ph_Num&0x20) && (*GSM_ph.temp_ph6!=0)) 	{Task_AT_GSM(Gsm_Send_SMS,GSM_ph.temp_ph6,SMS_CMD,0,GPS_Type,MSG_GOOGLE,0);	}
	if((Ph_Num&0x40) && (*GSM_ph.temp_ph7!=0)) 	{Task_AT_GSM(Gsm_Send_SMS,GSM_ph.temp_ph7,SMS_CMD,0,GPS_Type,MSG_GOOGLE,0);	}
	if((Ph_Num&0x80) && (*GSM_ph.temp_ph8!=0)) 	{Task_AT_GSM(Gsm_Send_SMS,GSM_ph.temp_ph8,SMS_CMD,0,GPS_Type,MSG_GOOGLE,0);	}
		
#endif
}


