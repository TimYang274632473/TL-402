#include <includes.h>
oil_def oil_para[4];
vehicle_def veh1;
vehicle1_def vehicle;
vehicle2_def veh2;
vehicle3_def veh3;
u8 rotate_sta=0;
u8 acc_count=0;
u8 door_count=0;
u8 pwr_count=0;
u8 inv_count=0;
u8 inv1_count=0;
u8 fwd1_count=0;
u8 fwd_count=0;
u8 rotate_val=0;
u8 rly_op_time;
u8 tim5_inc=0;

u16 acc_inval_count=0;
u16 base_v=0;
u16 oil[10],over[10];

u16 vot_d=0;
short int ds_temperature=0; 
u32 spk_count=0;
u32 spk_count1=18;
u32 _10us=0;

void oilcar_alarm_handle(void)
{
 u8 i=0;
 u8 j=0;
	
		if(veh2.b.acc_off_flag)
		{
			if(accpower_chk()==0)
			{
			 ACC_off();
			 veh2.b.acc_off_flag=0;
			}	
		}
		
	 //door报警
		if(door_chk()==veh1.b.DOR_ACT_flag)
		{
				veh2.b.door_flag=1;
				if(door_count>14)
				{
					door_count=30;
					if((veh1.b.DOR_SMS_flag==0)&&(veh1.b.ALM_ALW_flag==1))
					{						
						veh1.b.DOR_SMS_flag=1;					
						SPI_Flash_Write(2, &veh1.w,1);					
						send_info("","ALM-DOOR!",",ALM-G",GPRS_NEWGPS); 	
					}
        }				
		}		
		else 
		{
				if(veh2.b.door_flag)
				{
					door_count++;
					if(door_count>34)
					{
						door_count=0;
						veh2.b.door_flag=0;
				    veh1.b.DOR_SMS_flag=0;				    
				    SPI_Flash_Write(2, &veh1.w,1);					
          }
				}	
		}
		
		//ACC报警
		if(accpower_chk())
		{
				veh2.b.acc_flag=1;
				if(acc_count>14)
				{
					acc_count=30;
						if((veh1.b.ACC_SMS_flag==0)&&(veh1.b.ALM_ALW_flag==1))
						{							
							veh1.b.ACC_SMS_flag=1;						
							SPI_Flash_Write(2, &veh1.w,1);							
							send_info("","ALM-ACC!",",ALM-F",GPRS_NEWGPS); 	
						}
        }				
		}		
		else 
		{
				if(veh2.b.acc_flag)
				{
					acc_count++;
					if(acc_count>34)
					{
						acc_count=0;
						veh2.b.acc_flag=0;
						veh1.b.ACC_SMS_flag=0;				  
						SPI_Flash_Write(2, &veh1.w,1);	
					}						
      }
		}
		
		//PWRER报警
		if(PWRIN_chk()==0)
		{
				veh2.b.pwr_flag=1;
				if(pwr_count>14)
				{
					  pwr_count=30;
						if((veh1.b.PWR_SMS_flag==0)&&(veh1.b.ALM_ALW_flag==1))
						{							
							veh1.b.PWR_SMS_flag=1;							
							SPI_Flash_Write(2, &veh1.w,1);							
							send_info("","External Power Lost!",",ALM-J",GPRS_NEWGPS); 	
						}
        }				
		}		
		else 
		{
			if(veh2.b.pwr_flag)
			{
				pwr_count++;
				if(pwr_count>34)
				{
					pwr_count=0;
					veh2.b.pwr_flag=0;				
					veh1.b.PWR_SMS_flag=0;					
					SPI_Flash_Write(2, &veh1.w,1);
				}						
			 }
		}
		
		//正转检测
			if(foreward_chk())
			{
				if(veh2.b.fwd_flag==0)
				{
					fwd1_count++;
					if(fwd1_count>4){veh2.b.fwd_flag=1;veh2.b.fwd_c_flag=0;fwd_count=0;}
				}
			}
			else
			{
				fwd1_count=0;
				if(veh2.b.fwd_flag)
				{
					veh2.b.fwd_c_flag=1;
					if(fwd_count>vehicle.rotate_time){veh2.b.fwd_flag=0;veh2.b.fwd_c_flag=0;fwd_count=0;}
				}
			}

			//反转检测
			if(inversion_chk())
			{
				if(veh2.b.inv_flag==0)
				{
					inv1_count++;
					if(inv1_count>4){veh2.b.inv_flag=1;veh2.b.inv_c_flag=0;inv_count=0;}
				}
			}
			else
			{
				inv1_count=0;
				if(veh2.b.inv_flag)
				{
					veh2.b.inv_c_flag=1;
					if(inv_count>vehicle.rotate_time){veh2.b.inv_flag=0;veh2.b.inv_c_flag=0;inv_count=0;}
				}
			}
			
			if(veh2.b.fwd_flag)i|=0x08;
			if(foreward_chk()) i|=0x04;
			if(veh2.b.inv_flag)i|=0x02;
			if(inversion_chk())i|=0x01;		
			j=rotate_val&0x0f;
			if(i!=j)	
			{	
				j*=16;
				j+=i;
				rotate_val=j;
			}			
			switch(rotate_val&0x0f)
			{
				case 0x0a:	
				if(rotate_val==0xea) 
					rotate_sta=1;break;	
				case 0x02:
				case 0x03:
				case 0x0b:
					rotate_sta=2;break;	
				case 0x08:
				case 0x0c:
				case 0x0e:
					rotate_sta=1;break;		
				case 0x00:
					rotate_sta=0;break;	
				default:
					rotate_sta=3;		
			}
}	

//ADC获取油量传感器电压
u32 adc_get_oil(void)
{	
	u8 i;
	u32 adc_totalval=0;
  u32 adc_val=0; 	

	for(i=0;i<10;i++)
	{
		if(HAL_OK==HAL_ADC_PollForConversion(&hadc1,200))
		adc_totalval+=HAL_ADC_GetValue(&hadc1);
		else
		{i+=1;break;}
	}	
	adc_val=adc_totalval/i;	
  
  return (adc_val*3300*6/4096);//乘6是因为电阻分压了
}	


//除去一个最大值和最小值，再求平均
u16 aveoil_p(void)	
{
	u8 tempn=0;
  u16 maxv=0;	//maxv是最大值，minv是最小值	
	u16 minv=65535;	
	u16 tempv=0;

	for(tempn=0;tempn<7;tempn++)
	{		
	  if(oil[tempn]>maxv)maxv=oil[tempn];
	  if(oil[tempn]<minv)minv=oil[tempn];
	  tempv+=oil[tempn];	 
	}
	tempv-=minv;
	tempv-=maxv;
	tempv/=5;
	return tempv;
}

//计算油量值
//输入参数：电压
u16 chuli_oil(u32 oilvolt)
{
	u8 b,it;
  u16 oil_val1;	
	signed int zh,zb=0;
	
#if 0
	bool oil_f=0;
	u8 over_d=0;
	u8 oil_d=0;
	u16 tempvol;

  //base_v是基值，oilvolt是本次测量值，oil[]是测量数组，over[]是超值数组
	if(oilvolt>base_v)tempvol=oilvolt-base_v;	   
  else tempvol=base_v-oilvolt;
  if(tempvol>4)
  {
    //veh2.b.read_oil_again_flag=1;
		if(over_d>6)over_d=0;
		over[over_d]=oilvolt;
		over_d++;
		if(over_d>6)
		{
			for(over_d=0;over_d<7;over_d++)oil[over_d]=over[over_d];
			base_v=aveoil_p();
			oil_f=1;
		}
  }
  else 
  {over_d=0;veh2.b.read_oil_again_flag=0;}

  if(oil_d>6)oil_d=0;
	oil[oil_d]=oilvolt;
  oil_d++;
	if(oil_f)oil_val1=aveoil_p();
	else oil_val1=oilvolt;		
#endif

	vot_d=oilvolt;	
  zh=oilvolt;
	b=it=0;
	
next_it:	
	if(oil_para[it].oil_kk<300000)
	{
		switch(it)
		{
		 	case 0:	b|=0x01;break;
			case 1:	b|=0x02;break;
			case 2:	b|=0x04;break;
			case 3:	b|=0x08;
		}
		
		if((vot_d>=oil_para[it].oil_v1)&&(oil_para[it].oil_v2>=vot_d)) 
		{
jisuan_oil:
			zb=oil_para[it].oil_tt;
			zb*=zh;
			zb/=10000;
			zh=oil_para[it].oil_kk;
			zh+=zb;
			if(zh>0)oil_val1=zh;
			else oil_val1=0;
			return oil_val1;
		}
	}
	it++;
	if(it>3)	
	{
		if(b&0x01){it=0;goto last_oil1;}
		if(b&0x02){it=1;goto last_oil2;}
		if(b&0x04){it=2;goto last_oil3;}
		if(b&0x08){it=3;goto last_oil;}
		goto not_set;
last_oil1:
		if(b&0x02)b|=0x10;
last_oil2:
		if(b&0x04)b|=0x20;
last_oil3:
		if(b&0x08)b|=0x30;
last_oil:
		b/=16;
		if(b==0)goto jisuan_oil;
		if((vot_d>oil_para[it].oil_v2)&&(vot_d>oil_para[b].oil_v2))
		{
			if(oil_para[b].oil_v2>oil_para[it].oil_v2)it=b;
			goto jisuan_oil;
		}
		if((vot_d<oil_para[it].oil_v1)&&(vot_d<oil_para[b].oil_v1))
		{
		 	if(oil_para[b].oil_v1<oil_para[it].oil_v1)it=b;
			goto jisuan_oil;
		}
not_set:		
		oil_val1=vot_d;return oil_val1;
	}
	else goto next_it;
}	


	



