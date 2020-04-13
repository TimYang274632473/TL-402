#include "includes.h"

u8 SPI_FLASH_BUF[4096];
 	    
u8	 SPI_Flash_ReadSR(void); 
u8   SPIx_ReadWriteByte(u8 TxData);
void SPI_FLASH_Write_SR(u8 sr);  	
void SPI_FLASH_Write_Enable(void); 
void SPI_FLASH_Write_Disable(void);	
void SPI_Flash_Erase_Chip(void);    	 
void SPI_Flash_Wait_Busy(void);           	  
void SPI_Flash_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);

//4KbytesΪһ��Sector
//16������Ϊ1��Block
//W25X32
//����Ϊ4M�ֽ�,����64��Block,1024��Sector 

//SPIx ��дһ���ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPIx_ReadWriteByte(u8 TxData)
{	
	u8 RxData;
	HAL_SPI_TransmitReceive(&hspi1,&TxData,&RxData,1,1000);	
	return  RxData;  
}

//��ȡSPI FLASH  
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void SPI_Flash_Read(u32 ReadAddr,u8* pBuffer,u16 NumByteToRead)   
{ 
//	u16 i;    												    
	SPI_CS_LOW;  
	SPIx_ReadWriteByte(W25X_ReadData);         //���Ͷ�ȡ����   
	SPIx_ReadWriteByte((u8)((ReadAddr)>>16));  //����24bit��ַ    
	SPIx_ReadWriteByte((u8)((ReadAddr)>>8));   
	SPIx_ReadWriteByte((u8)ReadAddr);   
	HAL_SPI_TransmitReceive(&hspi1,pBuffer,pBuffer,NumByteToRead,1000);	
	SPI_CS_HIGH;  
}  

//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	 
void SPI_Flash_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
//	u16 i;  
	SPI_FLASH_Write_Enable();                  
	SPI_CS_LOW;                                
	SPIx_ReadWriteByte(W25X_PageProgram);      //����дҳ����   
	SPIx_ReadWriteByte((u8)((WriteAddr)>>16)); //����24bit��ַ    
	SPIx_ReadWriteByte((u8)((WriteAddr)>>8));   
	SPIx_ReadWriteByte((u8)WriteAddr);   
	HAL_SPI_TransmitReceive(&hspi1,pBuffer,pBuffer,NumByteToWrite,1000);	
	SPI_CS_HIGH;                            
	SPI_Flash_Wait_Busy();	 				  
} 

//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	  ;
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	
		SPI_Flash_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
		else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	}	    
} 


//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535) 

void SPI_Flash_Write(u32 WriteAddr,u8* pBuffer,u16 NumByteToWrite)  	
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;  

	secpos=WriteAddr/4096;//������ַ 0~511 for w25x32
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   
	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
	
	while(1) 
	{	
		SPI_Flash_Read(secpos*4096,SPI_FLASH_BUF,4096);//������������������
		
		for(i=0;i<secremain;i++)//У������
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			SPI_Flash_Erase_Sector(secpos);//�����������
			for(i=0;i<secremain;i++) //����
			{
				SPI_FLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);//д����������  

		}else SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		   	pBuffer+=secremain;  //ָ��ƫ��
			WriteAddr+=secremain;//д��ַƫ��	   
		   	NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;	//��һ����������д����
			else secremain=NumByteToWrite;			//��һ����������д����
		}	 
	}
  //INT_Enable();	
}


//��ȡSPI_FLASH��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
u8 SPI_Flash_ReadSR(void)   
{  
	u8 byte=0;   
	SPI_CS_LOW;                           
	SPIx_ReadWriteByte(W25X_ReadStatusReg); 	//���Ͷ�ȡ״̬�Ĵ�������    
	byte=SPIx_ReadWriteByte(0Xff);          	//��ȡһ���ֽ�  
	SPI_CS_HIGH;                             
	return byte;   
} 

//дSPI_FLASH״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
void SPI_FLASH_Write_SR(u8 sr)   
{   
	SPI_CS_LOW;                           
	SPIx_ReadWriteByte(W25X_WriteStatusReg); 	//����дȡ״̬�Ĵ�������    
	SPIx_ReadWriteByte(sr);                  	//д��һ���ֽ�  
	SPI_CS_HIGH;                               
}   

//SPI_FLASHдʹ��	
//��WEL��λ   
void SPI_FLASH_Write_Enable(void)   
{
	SPI_CS_LOW;                          
	SPIx_ReadWriteByte(W25X_WriteEnable);			//����дʹ��  
	SPI_CS_HIGH;                              
} 

//SPI_FLASHд��ֹ	
//��WEL����  
void SPI_FLASH_Write_Disable(void)   
{  
	SPI_CS_LOW;                            
	SPIx_ReadWriteByte(W25X_WriteDisable);		//����д��ָֹ��    
	SPI_CS_HIGH;                            	      
} 		

//��ȡоƬID W25X16��ID:0XEF14
u16 SPI_Flash_ReadID(void)
{
	u16 Temp = 0;	  
	SPI_CS_LOW;				    
	SPIx_ReadWriteByte(0x90);					//���Ͷ�ȡID����	    
	SPIx_ReadWriteByte(0x00); 	    
	SPIx_ReadWriteByte(0x00); 	    
	SPIx_ReadWriteByte(0x00); 	 			   
	Temp|=SPIx_ReadWriteByte(0xFF)<<8;  
	Temp|=SPIx_ReadWriteByte(0xFF);	 
	SPI_CS_HIGH;			    
	return Temp;
}

//��������оƬ
//��Ƭ����ʱ��:
//W25X16:25s 
//W25X32:40s 
//W25X64:40s 
//�ȴ�ʱ�䳬��...
void SPI_Flash_Erase_Chip(void)   
{                                             
	SPI_FLASH_Write_Enable();                  
	SPI_Flash_Wait_Busy();   
	SPI_CS_LOW;                           
	SPIx_ReadWriteByte(W25X_ChipErase);			//����Ƭ��������  
	SPI_CS_HIGH;                           	      
	SPI_Flash_Wait_Busy();						//�ȴ�оƬ��������
} 

//����һ������
//Dst_Addr:������ַ 0~1023 for w25x32
void SPI_Flash_Erase_Sector(u32 Dst_Addr)   
{   
	Dst_Addr*=4096;
	SPI_FLASH_Write_Enable();                  //дʹ�� 
	SPI_Flash_Wait_Busy();   
	SPI_CS_LOW;                           
	SPIx_ReadWriteByte(W25X_SectorErase);      //������������ָ�� 
	SPIx_ReadWriteByte((u8)((Dst_Addr)>>16));  //����24bit��ַ    
	SPIx_ReadWriteByte((u8)((Dst_Addr)>>8));   
	SPIx_ReadWriteByte((u8)Dst_Addr);  
	SPI_CS_HIGH;                           	      
	SPI_Flash_Wait_Busy(); 						//�ȴ��������
} 

// �ȴ�����
void SPI_Flash_Wait_Busy(void)   
{   
	while ((SPI_Flash_ReadSR()&0x01)==0x01);  
}  

// ����
void SPI_Flash_Sleep(void)   
{
	if(MCU_F.b.W25_wakeup_Flag == true)
	{
		SPI_CS_LOW;                           
		SPIx_ReadWriteByte(W25X_PowerDown);      
		SPI_CS_HIGH;	 
		MCU_F.b.W25_wakeup_Flag = false;
		vTaskDelay(1);
	}
} 

// ����
void SPI_Flash_Wakeup(void)   
{  
	if(MCU_F.b.W25_wakeup_Flag == false)
	{
		SPI_CS_LOW;                          
		SPIx_ReadWriteByte(W25X_ReleasePowerDown); 
		SPI_CS_HIGH;                              
		MCU_F.b.W25_wakeup_Flag = true;
		for(u16 i=0;i<300;i++);
	}		
}   
