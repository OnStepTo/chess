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


#define TP_PRES_DOWN 0x80  //����������	  
#define TP_CATH_PRES 0x40  //�а���������

//������������
typedef struct
{
	u16 x[5]; 							//��ǰ����
	u16 y[5];								//��x[0],y[0]�洢:�˴�ɨ��ʱ,����������,
													//��x[4],y[4]�洢��һ�ΰ���ʱ������. 
	u8  sta;								//�ʵ�״̬ 
													//b7:����1/�ɿ�0; 
	                        //b6:0,û�а�������;1,�а�������. 
													//b5~b0:����
	u8  touchtype;					//�����Ĳ���,��������������������ȫ�ߵ�ʱ��Ҫ�õ�.
													//b0:0,����(�ʺ�����ΪX����,����ΪY�����TP)
													//   1,����(�ʺ�����ΪY����,����ΪX�����TP) 
													//b1~6:����.
													//b7:0,������
													//   1,������
	u8	CMD_RDX;						//��X��������
	u8	CMD_RDY;						//��Y��������
}_m_tp_dev;

//У׼ϵ���ṹ��
typedef struct
{
	float xfac;		//������У׼����
	float yfac;
	short xoff;
	short yoff;
}_TouchPara;


extern	volatile uint8_t ucTouchFlag;
extern 	_m_tp_dev 	tp_dev;	 	//����������,��touch.c���涨��
extern	_TouchPara	TouchPara;//У׼ϵ��,��touch.c���涨��

//������оƬ��������	   
#define PEN  PFin(10)   //PF10 INT
#define DOUT PFin(8)   	//PF8  MISO
#define TDIN PFout(9)  	//PF9  MOSI
#define TCLK PBout(1)  	//PB1  SCLK
#define TCS  PBout(2) 	//PB2  CS

u8			TP_Scan(u8 tp);							//ɨ��
void		TP_Init(void);							//��ʼ��





#endif

















