#include "includes.h"

bool delay_us_flag=1;
bool ds18b_err_flag=0;
u32 _1us=0;


void delay_us(u32 nus)
{
	#if(MCU_FREQUENCE==72000000)
	delay_us_flag=1;
	_1us=0;
	while(_1us<nus);
	delay_us_flag=0;
	#elif(MCU_FREQUENCE==40000000)	
	// CPU_IntDis();	
	u32 i=0,j=2;
	for(i=0;i<nus;i++)
	{
	  j++;j--;j++;j--;j++;j--;j++;j--;j++;j--;j++;j--;j++;
		j++;j--;j++;j--;j++;j--;j++;j--;j++;j--;j++;j--;j++;
		j++;j--;j++;j--;j++;j--;j++;j--;j++;j--;j++;j--;j++;j--;j++;
	}
	//CPU_IntEn();
	#elif(MCU_FREQUENCE==16000000)	
	u32 i=0,j=2;
	for(i=0;i<nus;i++)
	{
		 CPU_IntDis();	
		j++;j--;j++;j--;j++;j--;j++;j--;j++;j--;j++;j--;j++;
	 //for(;j<12;j++); 
		CPU_IntEn();
	}
	#endif
}				 		   

void DS18B20_Rst(void)	   
{                 
	  DS18B20_IO_OUT(); //SET PB11 OUTPUT
    DS18B20_DQ_OUT=0; //DQ
    delay_us(750);    //750us
    DS18B20_DQ_OUT=1; //DQ=1 
	  delay_us(15);     //15US
}

u8 DS18B20_Check(void) 	   
{   
	u8 retry=0;
	DS18B20_IO_IN();//SET PB11 INPUT	 
    while (DS18B20_DQ_IN&&retry<200)
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=200)return 1;
	else retry=0;
    while (!DS18B20_DQ_IN&&retry<240)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)return 1;	  
	return 0;
}


u8 DS18B20_Read_Bit(void) 			 // read one bit
{
    u8 data;
	DS18B20_IO_OUT();//SET PB11 OUTPUT
    DS18B20_DQ_OUT=0; 
	delay_us(2);
    DS18B20_DQ_OUT=1; 
	DS18B20_IO_IN();//SET PB11 INPUT
	delay_us(12);
	if(DS18B20_DQ_IN)data=1;
    else data=0;	 
    delay_us(50);           
    return data;
}


u8 DS18B20_Read_Byte(void)   
{        
    u8 i,j,dat;
    dat=0;
	for (i=1;i<=8;i++) 
	{
        j=DS18B20_Read_Bit();
        dat=(j<<7)|(dat>>1);
    }	
    return dat;
}

void DS18B20_Write_Byte(u8 dat)     
 {             
    u8 j;
    u8 testb;
	DS18B20_IO_OUT();//SET PB11 OUTPUT;
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) 
        {
            DS18B20_DQ_OUT=0;// Write 1
            delay_us(2);                            
            DS18B20_DQ_OUT=1;
            delay_us(60);             
        }
        else 
        {
            DS18B20_DQ_OUT=0;// Write 0
            delay_us(60);             
            DS18B20_DQ_OUT=1;
            delay_us(2);                          
        }
    }
}


void DS18B20_Start(void)// ds1820 start convert
{   
	
    DS18B20_Rst();	   
	DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0x44);// convert
} 


u8 DS18B20_Init(void)
{
	RCC->APB2ENR|=1<<3;    //PB11
	GPIOB->CRH&=0XFFFF0FFF;    
	GPIOB->CRH|=0X00003000;
	GPIOB->ODR|=1<<11;      
	DS18B20_Rst();
	return DS18B20_Check();
}  

//¾«¶È:0.1C
//·¶Î§(-550~1250) 
short DS18B20_Get_Temp(void)
{
    u8 temp;
    u8 TL,TH;
	  short tem;
    DS18B20_Start (); // ds1820 start convert
	
	  DS18B20_IO_OUT();
	  DS18B20_DQ_OUT=1;
	  delay_us(600000);
	
    DS18B20_Rst();
    DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0xbe);// convert	    
    TL=DS18B20_Read_Byte(); // LSB   
    TH=DS18B20_Read_Byte(); // MSB   
    DS18B20_IO_OUT();
	  DS18B20_DQ_OUT=1;	
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;
    }else temp=1; 	  
    tem=TH;
    tem<<=8;    
    tem+=TL;
    tem=(float)tem*0.625;  
	if(temp)return tem; 
	else return -tem;    
} 
 
