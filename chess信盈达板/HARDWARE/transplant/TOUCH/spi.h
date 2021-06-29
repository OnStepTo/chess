#ifndef __SPI_H
#define __SPI_H

#include "stm32f10x.h"
#include "sys.h"

 				  	    													  
void 		SPI1_Init(void);			 //��ʼ��SPI��
void 		SPI1_SetSpeed(u16 SpeedSet); //����SPI�ٶ�   
u8 			SPI1_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�

void 		SPI2_Init(void);			 //��ʼ��SPI��
void 		SPI2_SetSpeed(u16 SpeedSet); //����SPI�ٶ�   
u8 			SPI2_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�


void		bsp_SpiBusEnter(void);
void		bsp_SpiBusExit(void);
uint8_t bsp_SpiBusBusy(void);


#endif

