#ifndef __SPI_H
#define __SPI_H

#include "stm32f10x.h"
#include "sys.h"

 				  	    													  
void 		SPI1_Init(void);			 //初始化SPI口
void 		SPI1_SetSpeed(u16 SpeedSet); //设置SPI速度   
u8 			SPI1_ReadWriteByte(u8 TxData);//SPI总线读写一个字节

void 		SPI2_Init(void);			 //初始化SPI口
void 		SPI2_SetSpeed(u16 SpeedSet); //设置SPI速度   
u8 			SPI2_ReadWriteByte(u8 TxData);//SPI总线读写一个字节


void		bsp_SpiBusEnter(void);
void		bsp_SpiBusExit(void);
uint8_t bsp_SpiBusBusy(void);


#endif

