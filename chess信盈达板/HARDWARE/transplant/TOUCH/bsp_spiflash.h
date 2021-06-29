#ifndef __BSP_SPIFLASH_H
#define __BSP_SPIFLASH_H

#include "stm32f10x.h"
#include "sys.h" 

//W25X系列/Q系列芯片列表	   
//W25Q80 ID  0XEF13
//W25Q16 ID  0XEF14
//W25Q32 ID  0XEF15
//W25Q64 ID  0XEF16	
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16

extern u16 	SPI_FLASH_TYPE;			//定义我们使用的flash芯片型号		   
#define			SPI_FLASH_CS PBout(12)  	//选中FLASH	

//#define FONTINFOADDR 			(4916+100)*1024 				//MiniSTM32字库从4.8M+100K地址开始的
//#define	GBK12ADDR					0x00510929							//GBK12字库地址
//#define	GBK16ADDR					0x0059CD89							//GBK16字库地址
//#define	GBK24ADDR					0x00657E09							//GBK24字库地址

#define	GBK12ADDR					0x00180000							//GBK12字库地址
#define	GBK16ADDR					0x0020D000							//GBK16字库地址
#define	GBK24ADDR					0x002C9000							//GBK24字库地址
#define	GBK32ADDR					0x0046E000							//GBK32字库地址
#define	TP_AdjSaveADDR		0x007FF000							//触屏校准参数地址
#define	IDLEADDR					0x00760000							//字库后可用的地址



 
//指令表
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

//列出可以调用的5个函数都加入了查忙操作,可以和别的模块复用SPI总线,其他函数如要引出需注意查忙

void SPI_Flash_Init(void);

void SPI_Flash_Read_TDMA(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   //读取flash
void SPI_Flash_Write_TDMA(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);//写入flash
void SPI_Flash_Erase_Chip_TDMA(void);    	  														//整片擦除
void SPI_Flash_Erase_Sector_TDMA(u32 Dst_Addr);													//扇区擦除

//u16  SPI_Flash_ReadID(void);  	    			//读取FLASH ID
//u8	 SPI_Flash_ReadSR(void);        			//读取状态寄存器 
//void SPI_FLASH_Write_SR(u8 sr);  					//写状态寄存器
//void SPI_FLASH_Write_Enable(void);  			//写使能 
//void SPI_FLASH_Write_Disable(void);				//写保护
//void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
//void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   //读取flash
//void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);//写入flash
//void SPI_Flash_Erase_Chip(void);    	  	//整片擦除
//void SPI_Flash_Erase_Sector(u32 Dst_Addr);//扇区擦除
//void SPI_Flash_Wait_Busy(void);           //等待空闲
//void SPI_Flash_PowerDown(void);           //进入掉电模式
//void SPI_Flash_WAKEUP(void);			  			//唤醒
#endif
















