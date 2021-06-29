#ifndef __BSP_TOUCH_H__
#define __BSP_TOUCH_H__

#include "stm32f10x.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "sys.h"
#include "delay.h"
//#include "bsp_systick.h"
#include "bsp_spiflash.h"
#include "lcd.h"


#define TP_PRES_DOWN 0x80  //触屏被按下	  
#define TP_CATH_PRES 0x40  //有按键按下了

//触摸屏控制器
typedef struct
{
	u16 x[5]; 							//当前坐标
	u16 y[5];								//用x[0],y[0]存储:此次扫描时,触屏的坐标,
													//用x[4],y[4]存储第一次按下时的坐标. 
	u8  sta;								//笔的状态 
													//b7:按下1/松开0; 
	                        //b6:0,没有按键按下;1,有按键按下. 
													//b5~b0:保留
	u8  touchtype;					//新增的参数,当触摸屏的左右上下完全颠倒时需要用到.
													//b0:0,竖屏(适合左右为X坐标,上下为Y坐标的TP)
													//   1,横屏(适合左右为Y坐标,上下为X坐标的TP) 
													//b1~6:保留.
													//b7:0,电阻屏
													//   1,电容屏
	u8	CMD_RDX;						//读X坐标命令
	u8	CMD_RDY;						//读Y坐标命令
}_m_tp_dev;

//校准系数结构体
typedef struct
{
	float xfac;		//触摸屏校准参数
	float yfac;
	short xoff;
	short yoff;
}_TouchPara;


extern	volatile uint8_t ucTouchFlag;
extern 	_m_tp_dev 	tp_dev;	 	//触屏控制器,在touch.c里面定义
extern	_TouchPara	TouchPara;//校准系数,在touch.c里面定义

//电阻屏芯片连接引脚	   
#define PEN  PFin(10)   //PF10 INT
#define DOUT PFin(8)   	//PF8  MISO
#define TDIN PFout(9)  	//PF9  MOSI
#define TCLK PBout(1)  	//PB1  SCLK
#define TCS  PBout(2) 	//PB2  CS

u8			TP_Scan(u8 tp);							//扫描
void		TP_Init(void);							//初始化





#endif

















