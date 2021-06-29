 
#include "bsp_spiflash.h" 
#include "spi.h"
#include "delay.h"
//#include "bsp_systick.h"
 
u16 SPI_FLASH_TYPE=W25Q64;//Ĭ�Ͼ���25Q64

//4KbytesΪһ��Sector
//16������Ϊ1��Block
//W25Q64
//����Ϊ8M�ֽ�,����128��Block,2048��Sector 



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
	SPI_FLASH_CS=0;                            //ʹ������   
	SPI2_ReadWriteByte(W25X_ReadStatusReg);    //���Ͷ�ȡ״̬�Ĵ�������    
	byte=SPI2_ReadWriteByte(0Xff);             //��ȡһ���ֽ�  
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     
	return byte;   
} 
//дSPI_FLASH״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
void SPI_FLASH_Write_SR(u8 sr)   
{   
	SPI_FLASH_CS=0;                            //ʹ������   
	SPI2_ReadWriteByte(W25X_WriteStatusReg);   //����дȡ״̬�Ĵ�������    
	SPI2_ReadWriteByte(sr);               //д��һ���ֽ�  
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     	      
}   
//SPI_FLASHдʹ��	
//��WEL��λ   
void SPI_FLASH_Write_Enable(void)   
{
	SPI_FLASH_CS=0;                            //ʹ������   
  SPI2_ReadWriteByte(W25X_WriteEnable);      //����дʹ��  
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     	      
} 
//SPI_FLASHд��ֹ	
//��WEL����
void SPI_FLASH_Write_Disable(void)   
{  
	SPI_FLASH_CS=0;                            //ʹ������   
  SPI2_ReadWriteByte(W25X_WriteDisable);     //����д��ָֹ��    
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     	      
}


//�ȴ�����
void SPI_Flash_Wait_Busy(void)   
{   
	while ((SPI_Flash_ReadSR()&0x01)==0x01);   // �ȴ�BUSYλ���
}  

//��ȡоƬID W25X16��ID:0XEF14
u16 SPI_Flash_ReadID(void)
{
	u16 Temp = 0;	  
	SPI_FLASH_CS=0;
	SPI2_ReadWriteByte(0x90);//���Ͷ�ȡID����
	SPI2_ReadWriteByte(0x00);
	SPI2_ReadWriteByte(0x00);
	SPI2_ReadWriteByte(0x00);
	Temp|=SPI2_ReadWriteByte(0xFF)<<8;
	Temp|=SPI2_ReadWriteByte(0xFF);
	SPI_FLASH_CS=1;				    
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
	SPI_FLASH_Write_Enable();                  //SET WEL 
	SPI_Flash_Wait_Busy();   
	SPI_FLASH_CS=0;                            //ʹ������   
	SPI2_ReadWriteByte(W25X_ChipErase);        //����Ƭ��������  
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     	      
	SPI_Flash_Wait_Busy();   				   //�ȴ�оƬ��������
}   
//����һ������
//Dst_Addr:������ַ 0~2047 for W25Q64
//����һ������������ʱ��:150ms
void SPI_Flash_Erase_Sector(u32 Dst_Addr)   
{   
	Dst_Addr*=4096;
	SPI_FLASH_Write_Enable();                  //SET WEL 	 
	SPI_Flash_Wait_Busy();   
	SPI_FLASH_CS=0;                            //ʹ������   
	SPI2_ReadWriteByte(W25X_SectorErase);      //������������ָ�� 
	SPI2_ReadWriteByte((u8)((Dst_Addr)>>16));  //����24bit��ַ    
	SPI2_ReadWriteByte((u8)((Dst_Addr)>>8));   
	SPI2_ReadWriteByte((u8)Dst_Addr);  
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     	      
  SPI_Flash_Wait_Busy();   				   				 //�ȴ��������
}  
//�������ģʽ
void SPI_Flash_PowerDown(void)   
{ 
	SPI_FLASH_CS=0;                            //ʹ������   
	SPI2_ReadWriteByte(W25X_PowerDown);        //���͵�������  
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     	      
   delay_us(3);                               //�ȴ�TPD  
}   
//����
void SPI_Flash_WAKEUP(void)   
{  
  	SPI_FLASH_CS=0;                            //ʹ������   
    SPI2_ReadWriteByte(W25X_ReleasePowerDown); //  send W25X_PowerDown command 0xAB    
		SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     	      
    delay_us(3);                            //�ȴ�TRES1
}   




//��ȡSPI FLASH  
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
 	u16 i;    												    
	SPI_FLASH_CS=0;                            //ʹ������   
	SPI2_ReadWriteByte(W25X_ReadData);         //���Ͷ�ȡ����   
	SPI2_ReadWriteByte((u8)((ReadAddr)>>16));  //����24bit��ַ    
	SPI2_ReadWriteByte((u8)((ReadAddr)>>8));   
	SPI2_ReadWriteByte((u8)ReadAddr);
	for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=SPI2_ReadWriteByte(0XFF);   //ѭ������
    }
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     	      
}  
//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	 
void SPI_Flash_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
    SPI_FLASH_Write_Enable();                  //SET WEL 
	SPI_FLASH_CS=0;                            //ʹ������   
    SPI2_ReadWriteByte(W25X_PageProgram);      //����дҳ����   
    SPI2_ReadWriteByte((u8)((WriteAddr)>>16)); //����24bit��ַ    
    SPI2_ReadWriteByte((u8)((WriteAddr)>>8));   
    SPI2_ReadWriteByte((u8)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)SPI2_ReadWriteByte(pBuffer[i]);//ѭ��д��  
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ 
	SPI_Flash_Wait_Busy();					   //�ȴ�д�����
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
	u16 pageremain;	   
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
	};	    
} 
//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)  		   
#ifndef MEM_ALLOC_TABLE_SIZE
u8 SPI_FLASH_BUFFER[4096];
#endif 
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i; 
	u8 * SPI_FLASH_BUF;	  
#ifdef MEM_ALLOC_TABLE_SIZE			
	SPI_FLASH_BUF=mymalloc(4096);	//ʹ���ڴ���� 
	if(SPI_FLASH_BUF==NULL)return;	//����ʧ��
#else
	SPI_FLASH_BUF=SPI_FLASH_BUFFER;	//��ʹ���ڴ����
#endif	 
	secpos=WriteAddr/4096;//������ַ 0~511 for w25x16
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С
	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
	while(1) 
	{	
		SPI_Flash_Read(SPI_FLASH_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			SPI_Flash_Erase_Sector(secpos);//�����������
			for(i=0;i<secremain;i++)	   //����
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
	};	 
#ifdef MEM_ALLOC_TABLE_SIZE			
	myfree(SPI_FLASH_BUF);		//�ͷ��ڴ�	 	 
#endif
}









//��ʼ��SPI FLASH��IO��
void SPI_Flash_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	while( bsp_SpiBusBusy() );	//��æ
	bsp_SpiBusEnter();	//��æ��־
	
	SPI2_Init();
	
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  //SPI CS
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_12);

	SPI_FLASH_TYPE=SPI_Flash_ReadID();//��ȡFLASH ID.
	
	bsp_SpiBusExit();		//��æ��־
}  



////��ʼ��SPI FLASH��IO�� 
//void SPI_Flash_Init(void)
//{
//	while( bsp_SpiBusBusy() );	//��æ
//	bsp_SpiBusEnter();	//��æ��־
//	
//	GPIO_InitTypeDef GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;  //SPI CS
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
//	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
// 	GPIO_Init(GPIOA, &GPIO_InitStructure);
// 	GPIO_SetBits(GPIOA,GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4);

//	SPI_FLASH_TYPE=SPI_Flash_ReadID();//��ȡFLASH ID.
//	
//	bsp_SpiBusExit();		//��æ��־
//}  




void SPI_Flash_Read_TDMA(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)
{
	while( bsp_SpiBusBusy() );	//��æ
	bsp_SpiBusEnter();	//��æ��־
	
	SPI_Flash_Read( pBuffer, ReadAddr, NumByteToRead);
	
	bsp_SpiBusExit();		//��æ��־
}


void SPI_Flash_Write_TDMA(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
	while( bsp_SpiBusBusy() );	//��æ
	bsp_SpiBusEnter();	//��æ��־
	
	SPI_Flash_Write( pBuffer, WriteAddr, NumByteToWrite);
	
	bsp_SpiBusExit();		//��æ��־
}


void SPI_Flash_Erase_Chip_TDMA(void)
{
	while( bsp_SpiBusBusy() );	//��æ
	bsp_SpiBusEnter();	//��æ��־
	
	SPI_Flash_Erase_Chip();
	
	bsp_SpiBusExit();		//��æ��־
}


void SPI_Flash_Erase_Sector_TDMA(u32 Dst_Addr)
{
	while( bsp_SpiBusBusy() );	//��æ
	bsp_SpiBusEnter();	//��æ��־
	
	SPI_Flash_Erase_Sector( Dst_Addr);
	
	bsp_SpiBusExit();		//��æ��־
}








