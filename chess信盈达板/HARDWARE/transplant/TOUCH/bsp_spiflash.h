#ifndef __BSP_SPIFLASH_H
#define __BSP_SPIFLASH_H

#include "stm32f10x.h"
#include "sys.h" 

//W25Xϵ��/Qϵ��оƬ�б�	   
//W25Q80 ID  0XEF13
//W25Q16 ID  0XEF14
//W25Q32 ID  0XEF15
//W25Q64 ID  0XEF16	
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16

extern u16 	SPI_FLASH_TYPE;			//��������ʹ�õ�flashоƬ�ͺ�		   
#define			SPI_FLASH_CS PBout(12)  	//ѡ��FLASH	

//#define FONTINFOADDR 			(4916+100)*1024 				//MiniSTM32�ֿ��4.8M+100K��ַ��ʼ��
//#define	GBK12ADDR					0x00510929							//GBK12�ֿ��ַ
//#define	GBK16ADDR					0x0059CD89							//GBK16�ֿ��ַ
//#define	GBK24ADDR					0x00657E09							//GBK24�ֿ��ַ

#define	GBK12ADDR					0x00180000							//GBK12�ֿ��ַ
#define	GBK16ADDR					0x0020D000							//GBK16�ֿ��ַ
#define	GBK24ADDR					0x002C9000							//GBK24�ֿ��ַ
#define	GBK32ADDR					0x0046E000							//GBK32�ֿ��ַ
#define	TP_AdjSaveADDR		0x007FF000							//����У׼������ַ
#define	IDLEADDR					0x00760000							//�ֿ����õĵ�ַ



 
//ָ���
#define W25X_WriteEnable			0x06 
#define W25X_WriteDisable			0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData					0x03 
#define W25X_FastReadData			0x0B 
#define W25X_FastReadDual			0x3B 
#define W25X_PageProgram			0x02 
#define W25X_BlockErase				0xD8 
#define W25X_SectorErase			0x20 
#define W25X_ChipErase				0xC7 
#define W25X_PowerDown				0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID					0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 

//�г����Ե��õ�5�������������˲�æ����,���Ժͱ��ģ�鸴��SPI����,����������Ҫ������ע���æ

void SPI_Flash_Init(void);

void SPI_Flash_Read_TDMA(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   //��ȡflash
void SPI_Flash_Write_TDMA(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);//д��flash
void SPI_Flash_Erase_Chip_TDMA(void);    	  														//��Ƭ����
void SPI_Flash_Erase_Sector_TDMA(u32 Dst_Addr);													//��������

//u16  SPI_Flash_ReadID(void);  	    			//��ȡFLASH ID
//u8	 SPI_Flash_ReadSR(void);        			//��ȡ״̬�Ĵ��� 
//void SPI_FLASH_Write_SR(u8 sr);  					//д״̬�Ĵ���
//void SPI_FLASH_Write_Enable(void);  			//дʹ�� 
//void SPI_FLASH_Write_Disable(void);				//д����
//void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
//void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   //��ȡflash
//void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);//д��flash
//void SPI_Flash_Erase_Chip(void);    	  	//��Ƭ����
//void SPI_Flash_Erase_Sector(u32 Dst_Addr);//��������
//void SPI_Flash_Wait_Busy(void);           //�ȴ�����
//void SPI_Flash_PowerDown(void);           //�������ģʽ
//void SPI_Flash_WAKEUP(void);			  			//����
#endif
















