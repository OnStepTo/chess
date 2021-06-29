#include "bsp_Touch.h"

//�������±�־ 1���£�ͨ�����ж��޸�
volatile uint8_t ucTouchFlag = 0;

//Ĭ��Ϊtouchtype=0������.
//u8 CMD_RDX=0XD0;
//u8 CMD_RDY=0X90;



_m_tp_dev tp_dev=
{
	{0},					//x5
	{0},					//y5 
	0,						//sta
	0,						//touchtype
	
};


_TouchPara	TouchPara={
	0,						//xfac
	0,						//yfac
	0,						//xoff
	0,						//yoff
};

/******************************* ����������صľ�̬���� *******************************/
static void 			TP_Write_Byte(u8 num);
static u16 				TP_Read_AD(u8 CMD);
static u16 				TP_Read_XOY(u8 xy);
static u8 				TP_Read_XY(u16 *x,u16 *y);
static u8 				TP_Read_XY2(u16 *x,u16 *y);
static void 			TP_Drow_Touch_Point(u16 x,u16 y,u16 color);
static void 			TP_Adj_Info_Show(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u16 x3,u16 y3,u16 fac);
static u8 				TP_ADJ( u8 cnt,u16 *x,u16 *y );
static uint8_t  	TP_Adjust(void);
static void 			Flash_TouchCalibrate ( void );
/***************************************************************************************/

//SPIд����
//������ICд��1byte����
//num:Ҫд�������
static void TP_Write_Byte(u8 num)
{  
	u8 count=0;   
	for(count=0;count<8;count++)  
	{
		if(num&0x80)TDIN=1;
		else TDIN=0;
		num<<=1;
		TCLK=0;
		TCLK=1;		//��������Ч	        
	}
}

//SPI������ 
//�Ӵ�����IC��ȡadcֵ
//CMD:ָ��
//����ֵ:����������	   
static u16 TP_Read_AD(u8 CMD)
{
	u8 count=0; 	  
	u16 Num=0; 
	TCLK=0;		//������ʱ�� 	 
	TDIN=0; 	//����������
	TCS=0; 		//ѡ�д�����IC
	TP_Write_Byte(CMD);//����������
	delay_us(6);//ADS7846��ת��ʱ���Ϊ6us
	TCLK=0;
	delay_us(1);
	TCLK=1;		//��1��ʱ�ӣ����BUSY	    	    
	TCLK=0;
	for(count=0;count<16;count++)//����16λ����,ֻ�и�12λ��Ч 
	{
		Num<<=1;
		TCLK=0;	//�½�����Ч  	    	   
		TCLK=1;
		if(DOUT)Num++;
	}  	
	Num>>=4;   	//ֻ�и�12λ��Ч.
	TCS=1;		//�ͷ�Ƭѡ	 
	return(Num);
}


//��ȡһ������ֵ(x����y)
//������ȡREAD_TIMES������,����Щ������������,
//Ȼ��ȥ����ͺ����LOST_VAL����,ȡƽ��ֵ 
//xy:ָ�CMD_RDX/CMD_RDY��
//����ֵ:����������
#define READ_TIMES 5 		//��ȡ����
#define LOST_VAL 1	  	//����ֵ
static u16 TP_Read_XOY(u8 xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=TP_Read_AD(xy);
	for(i=0;i<READ_TIMES-1; i++)//����
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//��������
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
}


//��ȡx,y����
//��Сֵ��������100.
//x,y:��ȡ��������ֵ
//����ֵ:0,ʧ��;1,�ɹ���
static u8 TP_Read_XY(u16 *x,u16 *y)
{
	u16 xtemp,ytemp;
	xtemp=TP_Read_XOY( tp_dev.CMD_RDX );
	ytemp=TP_Read_XOY( tp_dev.CMD_RDY );
	//if(xtemp<100||ytemp<100)return 0;//����ʧ��
	*x=xtemp;
	*y=ytemp;
	return 1;//�����ɹ�
}


//����2�ζ�ȡ������IC,�������ε�ƫ��ܳ���
//ERR_RANGE,��������,����Ϊ������ȷ,�����������.	   
//�ú����ܴ�����׼ȷ��
//x,y:��ȡ��������ֵ
//����ֵ:0,ʧ��;1,�ɹ���
#define ERR_RANGE 50 //��Χ 
static u8 TP_Read_XY2(u16 *x,u16 *y) 
{
	u16 x1,y1,x2,y2,dx,dy;
	
	TP_Read_XY(&x1,&y1);
	TP_Read_XY(&x2,&y2);
	dx = abs( (int)x1-(int)x2 );
	dy = abs( (int)y1-(int)y2 );
	
//	if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//ǰ�����β�����+-50��
//	&&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
	if( dx<ERR_RANGE && dy<ERR_RANGE )
	{
			*x=(x1+x2)/2;
			*y=(y1+y2)/2;
			return 1;
	}
	else return 0;	  
}


//////////////////////////////////////////////////////////////////////////////////		  
//��LCD�����йصĺ���  
//��һ��������
//����У׼�õ�
//x,y:����
//color:��ɫ
static void TP_Drow_Touch_Point(u16 x,u16 y,u16 color)
{
	u16 tempColor = POINT_COLOR;
	POINT_COLOR = color;
	LCD_DrawLine(x-12,y,x+13,y);//����
	LCD_DrawLine(x,y-12,x,y+13);//����
	LCD_DrawPoint(x+1,y+1);
	LCD_DrawPoint(x-1,y+1);
	LCD_DrawPoint(x+1,y-1);
	LCD_DrawPoint(x-1,y-1);
	LCD_Draw_Circle(x,y,6);//������Ȧ
	POINT_COLOR = tempColor;
}


//У׼�������(0,1)��(2,3)��(0,2)��(1,3),(1,2)��(0,3)���������ľ���
const u8 TP_ADJDIS_TBL[3][4]={{0,1,2,3},{0,2,1,3},{1,2,0,3}};//У׼��������
//��ʾ�ַ���
const char* TP_REMIND_MSG_TBL="Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";
//��ʾУ׼���(��������)
static void TP_Adj_Info_Show(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u16 x3,u16 y3,u16 fac)
{
	char cStr[100];
	
	sprintf(cStr,"x1:% 5d",x0);
	LCD_ShowString(40,160,80,20,16,(u8*)cStr);
	sprintf(cStr,"y1:% 5d",y0);
	LCD_ShowString(120,160,80,20,16,(u8*)cStr);
	
	sprintf(cStr,"x2:% 5d",x1);
	LCD_ShowString(40,180,80,20,16,(u8*)cStr);
	sprintf(cStr,"y2:% 5d",y1);
	LCD_ShowString(120,180,80,20,16,(u8*)cStr);
	
	sprintf(cStr,"x3:% 5d",x2);
	LCD_ShowString(40,200,80,20,16,(u8*)cStr);
	sprintf(cStr,"y3:% 5d",y2);
	LCD_ShowString(120,200,80,20,16,(u8*)cStr);
	
	sprintf(cStr,"x4:% 5d",x3);
	LCD_ShowString(40,220,80,20,16,(u8*)cStr);
	sprintf(cStr,"y4:% 5d",y3);
	LCD_ShowString(120,220,80,20,16,(u8*)cStr);
	
	sprintf(cStr,"fac:% 4d",fac);
	LCD_ShowString(40,240,80,20,16,(u8*)cStr);//��ʾ��ֵ,����ֵ������95~105��Χ֮��.
}

/***********************************************************************
* ����У׼�㣬��ȡ��������������
* ���� 	cnt 0~3 У׼�����
* 			*x,*y 	���ز�������������
* ����ֵ 1,������ȷ 0,��ʱ�˳�
***********************************************************************/
static u8 TP_ADJ( u8 cnt,u16 *x,u16 *y )
{	
	u16 outtime=0;
	u8	ret=0;
	
	//���ƴ�����
	switch( cnt )
	{
		case 0:	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//�����4
						TP_Drow_Touch_Point(20,20,RED);									//����1
						break;
		case 1:	TP_Drow_Touch_Point(20,20,WHITE);								//�����1 
						TP_Drow_Touch_Point(lcddev.width-20,20,RED);		//����2
						break;
		case 2:	TP_Drow_Touch_Point(lcddev.width-20,20,WHITE);	//�����2
						TP_Drow_Touch_Point(20,lcddev.height-20,RED);		//����3
						break;
		case 3:	TP_Drow_Touch_Point(20,lcddev.height-20,WHITE);			//�����3
						TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,RED);	//����4
		default:break;
	}
	
	while(1)
	{
		TP_Scan(1);//ɨ����������
		
		if((tp_dev.sta&0xc0)==TP_CATH_PRES)//����������һ��(��ʱ�����ɿ���.)
		{
			tp_dev.sta &= ~TP_CATH_PRES;	//��ǰ����Ѿ����������. 
			*x=tp_dev.x[0];
			*y=tp_dev.y[0];
			ret = 1;
			break;
		}
		
		delay_ms(10);
		if(++outtime>1000)
		{
			ret = 0;
			break;
		}
	}
	
	return ret;
}

//������У׼����
//�õ��ĸ�У׼����
static uint8_t  TP_Adjust(void)
{								 
	u16 pos_temp[4][2];//���껺��ֵ
	u8  cnt=0,Adjsta,ret = 0;
	u16 d1,d2;
	u32 tem1,tem2;
	float fac;
	u8	lflag=0;
	
	LCD_Clear(WHITE);	//����
	POINT_COLOR=BLUE;	//��ɫ
	LCD_ShowString(40,40,160,100,16,(u8*)TP_REMIND_MSG_TBL);//��ʾ��ʾ��Ϣ
	tp_dev.sta=0;//���������ź�
	
	while(1)
	{
		for( cnt=0;cnt<4;cnt++ )
		{
			Adjsta = TP_ADJ( cnt,&pos_temp[cnt][0],&pos_temp[cnt][1] );
			if( !Adjsta ) break;
		}
		if( !Adjsta ) {
			ret = 0;
			break;
		}
		
		//���������ľ����Ƿ�������Χ�ڣ�
		lflag = 1;
		for(cnt=0;cnt<3;cnt++)
		{
			tem1=abs(pos_temp[TP_ADJDIS_TBL[cnt][0]][0]-pos_temp[TP_ADJDIS_TBL[cnt][1]][0]);//x1-x2/x1-x3/x2-x3
			tem2=abs(pos_temp[TP_ADJDIS_TBL[cnt][0]][1]-pos_temp[TP_ADJDIS_TBL[cnt][1]][1]);//y1-y2/y1-y3/y2-y3
			tem1*=tem1;
			tem2*=tem2;
			d1=sqrt(tem1+tem2);//�õ�����֮��ľ��� 
			tem1=abs(pos_temp[TP_ADJDIS_TBL[cnt][2]][0]-pos_temp[TP_ADJDIS_TBL[cnt][3]][0]);//x3-x4/x2-x4/x1-x4
			tem2=abs(pos_temp[TP_ADJDIS_TBL[cnt][2]][1]-pos_temp[TP_ADJDIS_TBL[cnt][3]][1]);//y3-y4/y2-y4/y1-y4
			tem1*=tem1;
			tem2*=tem2;
			d2=sqrt(tem1+tem2);//�õ�����֮��ľ���
			fac=(float)d1/d2;
			if(fac<0.95||fac>1.05||d1==0||d2==0)//���ϸ�
			{
				lflag = 0;
			}
		}
		
		//TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);
		
		//ȫ����ȷ,������
		if( lflag )
		{
			TouchPara.xfac=(float)(lcddev.width-40)/(pos_temp[1][0]-pos_temp[0][0]);//�õ�xfac
			TouchPara.xoff=(lcddev.width-TouchPara.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//�õ�xoff
			TouchPara.yfac=(float)(lcddev.height-40)/(pos_temp[2][1]-pos_temp[0][1]);//�õ�yfac
			TouchPara.yoff=(lcddev.height-TouchPara.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//�õ�yoff
			
//			if( abs(TouchPara.xfac)>2 || abs(TouchPara.yfac)>2 )//������Ԥ����෴�ˣ�Ҫ��취���¿�ʼ.
//			{
//				LCD_ShowString(40,26,lcddev.width,lcddev.height,16,"TP Need readjust!");
//				tp_dev.touchtype=!tp_dev.touchtype;//�޸Ĵ�������.
//				if(tp_dev.touchtype)//X,Y��������Ļ�෴
//				{
//					CMD_RDX=0X90;
//					CMD_RDY=0XD0;
//				}
//				else				   //X,Y��������Ļ��ͬ
//				{
//					CMD_RDX=0XD0;
//					CMD_RDY=0X90;
//				}
//			}

			LCD_Clear(WHITE);//����
			LCD_ShowString(35,110,lcddev.width,lcddev.height,16,(u8*)"Touch Screen Adjust OK!");//У�����

			ret = 1;
			
			break;
		}
		else
		{
			//��ʾ����
			TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);
		}
	}
	
	return ret;
}


/**
  * @brief  LCD������У׼����ⲿflash��ȡУ׼ϵ��
  * @param  ��
  * @retval ��
  */
static void Flash_TouchCalibrate ( void )
{
	uint8_t ucStoreFlag,FlagWord;                   //LCD������У׼ϵ���洢��־
	
	if( lcddev.dir==0 ) FlagWord = 0x55;
	else if( lcddev.dir==1 ) FlagWord = 0x54;
	
	SPI_Flash_Read_TDMA ( & ucStoreFlag, TP_AdjSaveADDR , 1 );   //���ⲿflash����LCD������У׼ϵ���洢��־
	
	if ( ucStoreFlag == FlagWord )  //�Ѵ洢��LCD������У׼ϵ��
		SPI_Flash_Read_TDMA( (void*) & TouchPara, TP_AdjSaveADDR+1, sizeof(TouchPara) );	 //������ȡLCD������У׼ϵ��
		
	else                                          //��δ�洢��LCD������У׼ϵ��
	{
		while( !TP_Adjust() );      //�ȴ�������У�����

		/* �洢LCD������У׼ϵ���ͱ�־ */
		ucStoreFlag = FlagWord;
		SPI_Flash_Erase_Sector_TDMA( TP_AdjSaveADDR/4096 );
		SPI_Flash_Write_TDMA ( &ucStoreFlag, TP_AdjSaveADDR, 1 ); 
		SPI_Flash_Write_TDMA ( (void*) & TouchPara, TP_AdjSaveADDR+1, sizeof(TouchPara) );
	}
}


//////////////////////////////////////////////////////////////////////////////////		  
//��������ɨ��
//tp:0,��Ļ����;1,��������(У׼�����ⳡ����)
//����ֵ:��ǰ����״̬.
//0,�����޴���;1,�����д���
u8 TP_Scan(u8 tp)
{
	if(PEN==0)//�а�������
	{
		if(tp)	TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]);//��ȡ��������
		
		else if(TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]))//��ȡ��Ļ����
		{
	 		tp_dev.x[0]=TouchPara.xfac*tp_dev.x[0]+TouchPara.xoff;//�����ת��Ϊ��Ļ����
			tp_dev.y[0]=TouchPara.yfac*tp_dev.y[0]+TouchPara.yoff;  
	 	}
		
		if((tp_dev.sta&TP_PRES_DOWN)==0)//֮ǰû�б�����
		{
			tp_dev.sta=TP_PRES_DOWN|TP_CATH_PRES;//��������
			tp_dev.x[4]=tp_dev.x[0];//��¼��һ�ΰ���ʱ������
			tp_dev.y[4]=tp_dev.y[0];
		}
	}
	else
	{
		if(tp_dev.sta&TP_PRES_DOWN)//֮ǰ�Ǳ����µ�
		{
			tp_dev.sta &= ~TP_PRES_DOWN;//��ǰ����ɿ�
		}
		else//֮ǰ��û�б�����
		{
			tp_dev.x[4]=0;
			tp_dev.y[4]=0;
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
		}	    
	}
	return tp_dev.sta&TP_PRES_DOWN;//���ص�ǰ�Ĵ���״̬
}


//��������ʼ��
void TP_Init(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure;//GPIO
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOF, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_ResetBits( GPIOB,GPIO_Pin_2 ); /* ����Ƭѡ��ѡ����оƬ */
	
	//TP_Read_XY(&tp_dev.x[0],&tp_dev.y[0]);//��һ�ζ�ȡ��ʼ��
	
	if( lcddev.dir==0 )	//����
	{
		tp_dev.touchtype = 0;
		tp_dev.CMD_RDX = 0xD0;
		tp_dev.CMD_RDY = 0x90;
	}
	else if( lcddev.dir==1 )	//����
	{
		tp_dev.touchtype = 1;
		tp_dev.CMD_RDX = 0x90;
		tp_dev.CMD_RDY = 0xD0;
	}

	Flash_TouchCalibrate();
	
	
	/* EXTI line mode config */
//  GPIO_EXTILineConfig(GPIO_PortSourceGPIOF, GPIO_PinSource10);
//  EXTI_InitStructure.EXTI_Line = EXTI_Line10;
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //�������½����ж�
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;	
//  EXTI_Init(&EXTI_InitStructure);
//  
//  /* �����ж�Դ */
//  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
	
}






