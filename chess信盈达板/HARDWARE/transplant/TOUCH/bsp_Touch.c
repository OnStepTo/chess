#include "bsp_Touch.h"

//触屏按下标志 1按下，通过外中断修改
volatile uint8_t ucTouchFlag = 0;

//默认为touchtype=0的数据.
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

/******************************* 声明触屏相关的静态函数 *******************************/
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

//SPI写数据
//向触摸屏IC写入1byte数据
//num:要写入的数据
static void TP_Write_Byte(u8 num)
{  
	u8 count=0;   
	for(count=0;count<8;count++)  
	{
		if(num&0x80)TDIN=1;
		else TDIN=0;
		num<<=1;
		TCLK=0;
		TCLK=1;		//上升沿有效	        
	}
}

//SPI读数据 
//从触摸屏IC读取adc值
//CMD:指令
//返回值:读到的数据	   
static u16 TP_Read_AD(u8 CMD)
{
	u8 count=0; 	  
	u16 Num=0; 
	TCLK=0;		//先拉低时钟 	 
	TDIN=0; 	//拉低数据线
	TCS=0; 		//选中触摸屏IC
	TP_Write_Byte(CMD);//发送命令字
	delay_us(6);//ADS7846的转换时间最长为6us
	TCLK=0;
	delay_us(1);
	TCLK=1;		//给1个时钟，清除BUSY	    	    
	TCLK=0;
	for(count=0;count<16;count++)//读出16位数据,只有高12位有效 
	{
		Num<<=1;
		TCLK=0;	//下降沿有效  	    	   
		TCLK=1;
		if(DOUT)Num++;
	}  	
	Num>>=4;   	//只有高12位有效.
	TCS=1;		//释放片选	 
	return(Num);
}


//读取一个坐标值(x或者y)
//连续读取READ_TIMES次数据,对这些数据升序排列,
//然后去掉最低和最高LOST_VAL个数,取平均值 
//xy:指令（CMD_RDX/CMD_RDY）
//返回值:读到的数据
#define READ_TIMES 5 		//读取次数
#define LOST_VAL 1	  	//丢弃值
static u16 TP_Read_XOY(u8 xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=TP_Read_AD(xy);
	for(i=0;i<READ_TIMES-1; i++)//排序
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//升序排列
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


//读取x,y坐标
//最小值不能少于100.
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
static u8 TP_Read_XY(u16 *x,u16 *y)
{
	u16 xtemp,ytemp;
	xtemp=TP_Read_XOY( tp_dev.CMD_RDX );
	ytemp=TP_Read_XOY( tp_dev.CMD_RDY );
	//if(xtemp<100||ytemp<100)return 0;//读数失败
	*x=xtemp;
	*y=ytemp;
	return 1;//读数成功
}


//连续2次读取触摸屏IC,且这两次的偏差不能超过
//ERR_RANGE,满足条件,则认为读数正确,否则读数错误.	   
//该函数能大大提高准确度
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
#define ERR_RANGE 50 //误差范围 
static u8 TP_Read_XY2(u16 *x,u16 *y) 
{
	u16 x1,y1,x2,y2,dx,dy;
	
	TP_Read_XY(&x1,&y1);
	TP_Read_XY(&x2,&y2);
	dx = abs( (int)x1-(int)x2 );
	dy = abs( (int)y1-(int)y2 );
	
//	if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//前后两次采样在+-50内
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
//与LCD部分有关的函数  
//画一个触摸点
//用来校准用的
//x,y:坐标
//color:颜色
static void TP_Drow_Touch_Point(u16 x,u16 y,u16 color)
{
	u16 tempColor = POINT_COLOR;
	POINT_COLOR = color;
	LCD_DrawLine(x-12,y,x+13,y);//横线
	LCD_DrawLine(x,y-12,x,y+13);//竖线
	LCD_DrawPoint(x+1,y+1);
	LCD_DrawPoint(x-1,y+1);
	LCD_DrawPoint(x+1,y-1);
	LCD_DrawPoint(x-1,y-1);
	LCD_Draw_Circle(x,y,6);//画中心圈
	POINT_COLOR = tempColor;
}


//校准点参数：(0,1)与(2,3)，(0,2)与(1,3),(1,2)与(0,3)，这三组点的距离
const u8 TP_ADJDIS_TBL[3][4]={{0,1,2,3},{0,2,1,3},{1,2,0,3}};//校准距离计算表
//提示字符串
const char* TP_REMIND_MSG_TBL="Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";
//提示校准结果(各个参数)
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
	LCD_ShowString(40,240,80,20,16,(u8*)cStr);//显示数值,该数值必须在95~105范围之内.
}

/***********************************************************************
* 绘制校准点，获取测量点物理坐标
* 参数 	cnt 0~3 校准点序号
* 			*x,*y 	返回测量的物理坐标
* 返回值 1,测量正确 0,超时退出
***********************************************************************/
static u8 TP_ADJ( u8 cnt,u16 *x,u16 *y )
{	
	u16 outtime=0;
	u8	ret=0;
	
	//绘制触摸点
	switch( cnt )
	{
		case 0:	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//清除点4
						TP_Drow_Touch_Point(20,20,RED);									//画点1
						break;
		case 1:	TP_Drow_Touch_Point(20,20,WHITE);								//清除点1 
						TP_Drow_Touch_Point(lcddev.width-20,20,RED);		//画点2
						break;
		case 2:	TP_Drow_Touch_Point(lcddev.width-20,20,WHITE);	//清除点2
						TP_Drow_Touch_Point(20,lcddev.height-20,RED);		//画点3
						break;
		case 3:	TP_Drow_Touch_Point(20,lcddev.height-20,WHITE);			//清除点3
						TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,RED);	//画点4
		default:break;
	}
	
	while(1)
	{
		TP_Scan(1);//扫描物理坐标
		
		if((tp_dev.sta&0xc0)==TP_CATH_PRES)//按键按下了一次(此时按键松开了.)
		{
			tp_dev.sta &= ~TP_CATH_PRES;	//标记按键已经被处理过了. 
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

//触摸屏校准代码
//得到四个校准参数
static uint8_t  TP_Adjust(void)
{								 
	u16 pos_temp[4][2];//坐标缓存值
	u8  cnt=0,Adjsta,ret = 0;
	u16 d1,d2;
	u32 tem1,tem2;
	float fac;
	u8	lflag=0;
	
	LCD_Clear(WHITE);	//清屏
	POINT_COLOR=BLUE;	//蓝色
	LCD_ShowString(40,40,160,100,16,(u8*)TP_REMIND_MSG_TBL);//显示提示信息
	tp_dev.sta=0;//消除触发信号
	
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
		
		//计算三组点的距离是否在允许范围内？
		lflag = 1;
		for(cnt=0;cnt<3;cnt++)
		{
			tem1=abs(pos_temp[TP_ADJDIS_TBL[cnt][0]][0]-pos_temp[TP_ADJDIS_TBL[cnt][1]][0]);//x1-x2/x1-x3/x2-x3
			tem2=abs(pos_temp[TP_ADJDIS_TBL[cnt][0]][1]-pos_temp[TP_ADJDIS_TBL[cnt][1]][1]);//y1-y2/y1-y3/y2-y3
			tem1*=tem1;
			tem2*=tem2;
			d1=sqrt(tem1+tem2);//得到两点之间的距离 
			tem1=abs(pos_temp[TP_ADJDIS_TBL[cnt][2]][0]-pos_temp[TP_ADJDIS_TBL[cnt][3]][0]);//x3-x4/x2-x4/x1-x4
			tem2=abs(pos_temp[TP_ADJDIS_TBL[cnt][2]][1]-pos_temp[TP_ADJDIS_TBL[cnt][3]][1]);//y3-y4/y2-y4/y1-y4
			tem1*=tem1;
			tem2*=tem2;
			d2=sqrt(tem1+tem2);//得到两点之间的距离
			fac=(float)d1/d2;
			if(fac<0.95||fac>1.05||d1==0||d2==0)//不合格
			{
				lflag = 0;
			}
		}
		
		//TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);
		
		//全部正确,计算结果
		if( lflag )
		{
			TouchPara.xfac=(float)(lcddev.width-40)/(pos_temp[1][0]-pos_temp[0][0]);//得到xfac
			TouchPara.xoff=(lcddev.width-TouchPara.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//得到xoff
			TouchPara.yfac=(float)(lcddev.height-40)/(pos_temp[2][1]-pos_temp[0][1]);//得到yfac
			TouchPara.yoff=(lcddev.height-TouchPara.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//得到yoff
			
//			if( abs(TouchPara.xfac)>2 || abs(TouchPara.yfac)>2 )//触屏和预设的相反了，要想办法重新开始.
//			{
//				LCD_ShowString(40,26,lcddev.width,lcddev.height,16,"TP Need readjust!");
//				tp_dev.touchtype=!tp_dev.touchtype;//修改触屏类型.
//				if(tp_dev.touchtype)//X,Y方向与屏幕相反
//				{
//					CMD_RDX=0X90;
//					CMD_RDY=0XD0;
//				}
//				else				   //X,Y方向与屏幕相同
//				{
//					CMD_RDX=0XD0;
//					CMD_RDY=0X90;
//				}
//			}

			LCD_Clear(WHITE);//清屏
			LCD_ShowString(35,110,lcddev.width,lcddev.height,16,(u8*)"Touch Screen Adjust OK!");//校正完成

			ret = 1;
			
			break;
		}
		else
		{
			//显示数据
			TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);
		}
	}
	
	return ret;
}


/**
  * @brief  LCD触摸屏校准或从外部flash读取校准系数
  * @param  无
  * @retval 无
  */
static void Flash_TouchCalibrate ( void )
{
	uint8_t ucStoreFlag,FlagWord;                   //LCD触摸屏校准系数存储标志
	
	if( lcddev.dir==0 ) FlagWord = 0x55;
	else if( lcddev.dir==1 ) FlagWord = 0x54;
	
	SPI_Flash_Read_TDMA ( & ucStoreFlag, TP_AdjSaveADDR , 1 );   //从外部flash读出LCD触摸屏校准系数存储标志
	
	if ( ucStoreFlag == FlagWord )  //已存储过LCD触摸屏校准系数
		SPI_Flash_Read_TDMA( (void*) & TouchPara, TP_AdjSaveADDR+1, sizeof(TouchPara) );	 //继续读取LCD触摸屏校准系数
		
	else                                          //尚未存储过LCD触摸屏校准系数
	{
		while( !TP_Adjust() );      //等待触摸屏校正完毕

		/* 存储LCD触摸屏校准系数和标志 */
		ucStoreFlag = FlagWord;
		SPI_Flash_Erase_Sector_TDMA( TP_AdjSaveADDR/4096 );
		SPI_Flash_Write_TDMA ( &ucStoreFlag, TP_AdjSaveADDR, 1 ); 
		SPI_Flash_Write_TDMA ( (void*) & TouchPara, TP_AdjSaveADDR+1, sizeof(TouchPara) );
	}
}


//////////////////////////////////////////////////////////////////////////////////		  
//触摸按键扫描
//tp:0,屏幕坐标;1,物理坐标(校准等特殊场合用)
//返回值:当前触屏状态.
//0,触屏无触摸;1,触屏有触摸
u8 TP_Scan(u8 tp)
{
	if(PEN==0)//有按键按下
	{
		if(tp)	TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]);//读取物理坐标
		
		else if(TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]))//读取屏幕坐标
		{
	 		tp_dev.x[0]=TouchPara.xfac*tp_dev.x[0]+TouchPara.xoff;//将结果转换为屏幕坐标
			tp_dev.y[0]=TouchPara.yfac*tp_dev.y[0]+TouchPara.yoff;  
	 	}
		
		if((tp_dev.sta&TP_PRES_DOWN)==0)//之前没有被按下
		{
			tp_dev.sta=TP_PRES_DOWN|TP_CATH_PRES;//按键按下
			tp_dev.x[4]=tp_dev.x[0];//记录第一次按下时的坐标
			tp_dev.y[4]=tp_dev.y[0];
		}
	}
	else
	{
		if(tp_dev.sta&TP_PRES_DOWN)//之前是被按下的
		{
			tp_dev.sta &= ~TP_PRES_DOWN;//标记按键松开
		}
		else//之前就没有被按下
		{
			tp_dev.x[4]=0;
			tp_dev.y[4]=0;
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
		}	    
	}
	return tp_dev.sta&TP_PRES_DOWN;//返回当前的触屏状态
}


//触摸屏初始化
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
	
	GPIO_ResetBits( GPIOB,GPIO_Pin_2 ); /* 拉低片选，选择触屏芯片 */
	
	//TP_Read_XY(&tp_dev.x[0],&tp_dev.y[0]);//第一次读取初始化
	
	if( lcddev.dir==0 )	//竖屏
	{
		tp_dev.touchtype = 0;
		tp_dev.CMD_RDX = 0xD0;
		tp_dev.CMD_RDY = 0x90;
	}
	else if( lcddev.dir==1 )	//横屏
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
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //上升、下降沿中断
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;	
//  EXTI_Init(&EXTI_InitStructure);
//  
//  /* 配置中断源 */
//  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
	
}






