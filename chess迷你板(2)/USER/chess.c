/*****************************************************************************
Copyright: δ����ɣ�����ת��
File name: Chess.c
Description: ����Ĺ����߼�����
Author: guoxl
Version: �汾
Date: �������
History: �޸���ʷ��¼�б� ÿ���޸ļ�¼Ӧ�����޸����ڡ��޸��߼��޸����ݼ�����
*****************************************************************************/
#include <sys.h>
#include <chess.h>
#include "displayChess.h"
#include "touch.h"
#include "lcd.h"
#include "delay.h"
#include "check.h"

#define LED0 PBout(5)// PB5
#define LED1 PEout(5)// PE5	
#define KEY0  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)//��ȡ����0

int8_t TURN = 0;//turn�������ַ�����˼��������������һ��������
int8_t undoEn = 0;//���������־λ��ֻ�ܻ�һ����������֮��ʹ�ܣ�����֮���ֹ


ChangeBuffer changeBuffer2 = 
{
    -2,
    0,
    0,
    0,
    0,
    0
};


//���ӽṹ�������ʼ�����ܹ���32�����ӣ�ÿ��������һ���ṹ���ʾ,һ�����һ�����飬
//������±���Կ��������ӵ�һ������
//ÿһ�����Ӷ�Ӧ0~31�е�һ������
//�����õ�һ����������˼ά��ʽ
PieceTypedef piece[32] = 
{
	{'܇',0,0,blackCAMP,car,ALIVE},//0
    {'��',1,0,blackCAMP,horse,ALIVE},//1
    {'��',2,0,blackCAMP,elephant,ALIVE},//2
    {'ʿ',3,0,blackCAMP,guard,ALIVE},//3
    {'��',4,0,blackCAMP,general,ALIVE},//4
    {'ʿ',5,0,blackCAMP,guard,ALIVE},//5
    {'��',6,0,blackCAMP,elephant,ALIVE},//6
    {'��',7,0,blackCAMP,horse,ALIVE},//7
    {'܇',8,0,blackCAMP,car,ALIVE},//8
    {'��',1,2,blackCAMP,cannon,ALIVE},//9
    {'��',7,2,blackCAMP,cannon,ALIVE},//10
    {'��',0,3,blackCAMP,soldier,ALIVE},//11
    {'��',2,3,blackCAMP,soldier,ALIVE},//12
    {'��',4,3,blackCAMP,soldier,ALIVE},//13
    {'��',6,3,blackCAMP,soldier,ALIVE},//14
    {'��',8,3,blackCAMP,soldier,ALIVE},//15
    {'��',0,6,redCAMP,soldier,ALIVE},//16
    {'��',2,6,redCAMP,soldier,ALIVE},//17
    {'��',4,6,redCAMP,soldier,ALIVE},//18
    {'��',6,6,redCAMP,soldier,ALIVE},//19
    {'��',8,6,redCAMP,soldier,ALIVE},//20
    {'��',1,7,redCAMP,cannon,ALIVE},//21
    {'��',7,7,redCAMP,cannon,ALIVE},//22
    {'��',0,9,redCAMP,car,ALIVE},//23
    {'��',1,9,redCAMP,horse,ALIVE},//24
    {'��',2,9,redCAMP,elephant,ALIVE},//25
    {'ʿ',3,9,redCAMP,guard,ALIVE},//26
    {'˧',4,9,redCAMP,general,ALIVE},//27
    {'ʿ',5,9,redCAMP,guard,ALIVE},//28
    {'��',6,9,redCAMP,elephant,ALIVE},//29
    {'��',7,9,redCAMP,horse,ALIVE},//30
    {'��',8,9,redCAMP,car,ALIVE}//31
};



//�������飬ʵʱ��¼���������ӷֲ�λ��
//��СΪ9x10�ֽڣ�ÿһ��Ԫ�ض�Ӧ�����ϵ�һ�����
//λ�����˴���ʵ��Ч������֪������꣨x,y����peiceTable[x][y]��ʾ�����ϣ�x,y��λ�õ����ӵ�������������Ϊ-1��
int8_t pieceTable[9][10] =           //    pieceTable[4][3]    13   ���ñ��xy�����������ϵ�λ����90����ת������ֱ��ȡ������Ӧ���ӵ��±�
{
    {0,-1,-1,11,-1,-1,16,-1,-1,23},     //*-1����Ϊ���������ӣ���-1�������������ʾ��Ӧ���ӵ����±꣬λ����������Ľṹ���ж�Ӧ
    {1,-1,9,-1,-1,-1,-1,21,-1,24},                  
    {2,-1,-1,12,-1,-1,17,-1,-1,25},
    {3,-1,-1,-1,-1,-1,-1,-1,-1,26},
    {4,-1,-1,13,-1,-1,18,-1,-1,27},
    {5,-1,-1,-1,-1,-1,-1,-1,-1,28},
    {6,-1,-1,14,-1,-1,19,-1,-1,29},
    {7,-1,10,-1,-1,-1,-1,22,-1,30},
    {8,-1,-1,15,-1,-1,20,-1,-1,31}
};



//����7�������ֱ��Ӧ��7�����ӵĵ�·���ж�
//�췽�ͺڷ����ã����������ӽṹ��
//����x1��y1���ϵ������ƶ�����x2��y2����CampΪ���ӵ���Ӫ��������ݹ�������ƶ��򷵻�1�����򷵻�0
//ע���������ж϶�����ʵ�ʵ��ƶ�
int8_t car(int8_t x1,int8_t y1,int8_t x2,int8_t y2,int8_t Camp)//
{
    int8_t index,i,temp;
	
    if(x1 != x2 && y1 != y2)  //����ͬһֱ���ϣ�������
    return 0;
    if(x1 == x2 && y1 == y2)
    return 0;
	 
    index = pieceTable[x2][y2];
    if(index!=-1 && piece[index].camp == Camp)//Ŀ�ĵ����Ѿ����ҷ����ӣ�������
    return 0;
	 
	 if(x2<0 || y2<0 || x2>8 || y2>9)
    return 0;
	 
    i = 1;
    if(x1 == x2)         //�����ƶ�
    {
        if(y1>y2)        //���y1>y2,����ֵ��������֤y2�Ǹ�����Ǹ�
        {
            temp = y1;
            y1 = y2;
            y2 = temp;
        }
        while(pieceTable[x1][y1+i] == -1)//�ж�y1��y2֮�������������ӣ��еĻ�������
        {
            if(y1+i == y2)
            break;
            i++;
        }
        if(y1+i != y2)
        return 0;
    }
    else              //�����ƶ���ԭ��ͬ��
    {
        if(x1>x2) 
        {
            temp = x1;
            x1 = x2;
            x2 = temp;
        }
        while(pieceTable[x1+i][y1] == -1)
        {
            if(x1+i == x2)
            break;
            i++;
        }
        if(x1+i != x2)
        return 0;        
    }
    return 1;
}

int8_t horse(int8_t x1,int8_t y1,int8_t x2,int8_t y2,int8_t Camp)//(x1,y1):Դ����(x2,y2):Ŀ������
{
    int8_t dx,dy,index,x3,y3;
    dx = x2-x1;              
    dy = y2-y1;

    if(dx*dy!=2 && dx*dy!=-2)       //�����գ�x��y����һ������Ϊ1��һ������Ϊ2�����Գ˻�һ��Ϊ2����-2
    return 0;

    index = pieceTable[x2][y2];
    if( index!=-1 && piece[index].camp == Camp)
    return 0;

    x3 = x1+dx/2;                    //(x3,y3)���ƶ�ʱ����ŵ�λ��
    y3 = y1+dy/2;
    if(pieceTable[x3][y3] != -1)
    return 0;

    if(x2<0 || y2<0 || x2>8 || y2>9)
    return 0;

    return 1;
}

int8_t elephant(int8_t x1,int8_t y1,int8_t x2,int8_t y2,int8_t Camp)
{
	int8_t index,dx,dy;   
    dx = x2-x1;
    dy = y2-y1; 

    if((dx!=2 && dx!=-2) || (dy!=2 && dy!=-2))    //x,y�����ƶ��ľ���һ����2
    return 0;

    index = pieceTable[x1+dx/2][y1+dy/2];//����鿴��Ŵ��Ƿ������ӣ��еĻ������ƶ�
    if(index != -1)
    return 0; 

	 if(x2<0 || y2<0 || x2>8 || y2>9)
    return 0;

    if(Camp)                    //���ܹ���
    {
        if(y2>4)
        return 0;
    }
    else
    {
        if(y2<5)
        return 0;
    }
    
    index = pieceTable[x2][y2];
    if(index!=-1 && piece[index].camp == Camp)//Ŀ�ĵ��м������ӣ�������
    return 0; 
	 
    return 1;   
}

//ʿ
int8_t guard(int8_t x1,int8_t y1,int8_t x2,int8_t y2,int8_t Camp)
{
	 int8_t index,dx,dy;   
    dx = x2-x1;
    dy = y2-y1; 

    if(x2<3 || x2>5)
    return 0;

    if(dx*dx+dy*dy != 2)     //ʿx,y�����ƶ��ľ���һ����1
    return 0;

    if(Camp)
    {
        if(y2>2 || y2<0)
        return 0;
    }
    else
    {
        if(y2<7 || y2>9)
        return 0;
    }
    	
    index = pieceTable[x2][y2];
    if(index!=-1 && piece[index].camp == Camp)
    return 0; 
	 
    return 1;
}

//��(˧)
int8_t general(int8_t x1,int8_t y1,int8_t x2,int8_t y2,int8_t Camp)
{
	int8_t index,dx,dy;   
    dx = x2-x1;
    dy = y2-y1;

    index = pieceTable[x2][y2];
    if(index!=-1 && piece[index].camp == Camp)
    return 0;

    if(x2<3 || x2>5)
    return 0;

    if(dx*dx+dy*dy != 1)   //���ƶ���x,y�����ϵľ���һ����0һ����1��
    return 0;

    if(Camp)
    {
        if(y2>2 || y2<0)
        return 0;
    }
    else
    {
        if(y2<7 || y2>9)
        return 0;
    }
    return 1;    
}

//��
int8_t cannon(int8_t x1,int8_t y1,int8_t x2,int8_t y2,int8_t Camp)
{
    int8_t index,i,temp,pieceN,temp1;
    temp1 = pieceTable[x2][y2];
    pieceN = 0;
	
    if(x1 != x2 && y1 != y2)//�������ͬһ��ֱ���ϣ�������
    return 0;
    if(x1 == x2 && y1 == y2)//ԭ���ƶ�û������
    return 0;
	 if(x2<0 || y2<0 || x2>8 || y2>9)
    return 0;
	 
    index = pieceTable[x2][y2];
    i = 1;
	 
    if(x1 == x2)         //�����ƶ�
    {
        if(y1>y2)        //���y1>y2������ֵ��������֤y2�Ǹ�����Ǹ�
        {
            temp = y1;
            y1 = y2;
            y2 = temp;
        }
        for(i=1;y1+i<y2;i++)//y1��y2֮�����е�λ�ã�������y1��y2������ѯһ��
        {
            if(pieceTable[x1][y1+i] != -1)//��������pieceN++���Դ�ͳ����λ��֮���������
            pieceN++;  //�ж��������ӵı�־λ
        }
    }
    else                //�����ƶ���ͬ��
    {
        if(x1>x2) 
        {
            temp = x1;
            x1 = x2;
            x2 = temp;
        }
        for(i=1;x1+i<x2;i++)
        {
            if(pieceTable[x1+i][y1] != -1)
            pieceN++;
        }
    }
	 
    if(pieceN > 1)//��ʱpieceN��ʾԴλ����Ŀ��λ��֮���ж��ٸ�����
    return 0;
	 
	 
	 //���м�λ�������ӣ�Ŀ��λ�������Ӳ����ƣ��м�λ�������ӣ�Ŀ��λ�������Ӳ�����
    if((pieceN==1 && temp1==-1) || (pieceN==0 && temp1!=-1))
    return 0;
	 
    if(index!=-1 && piece[index].camp == Camp)   //Ŀ��λ�ò�Ϊ�յ����Ǽ�������
    return 0;
	 
    return 1;
}

//��
int8_t soldier(int8_t x1,int8_t y1,int8_t x2,int8_t y2,int8_t Camp)
{
	int8_t index,dx,dy;
    dx = x2-x1;
    dy = y2-y1;

    if(dy*dy+dx*dx != 1)
    return 0;
	 
	 if(x2<0 || y2<0 || x2>8 || y2>9)
    return 0;

    if(Camp)//����Ӫ�����
    {
        if(dy < 0)//�����ܺ���
        return 0;
        if(y1<=4 && dx!=0)//��û�й����ǲ��������ƶ���
        return 0;
    }
    else
    {      
        if(dy>0)
        return 0;
        if(y1>=5 && dx!=0)
        return 0;
    }

    index = pieceTable[x2][y2];
    if(index!=-1 && piece[index].camp == Camp)
    return 0;

    return 1;
}

//�ƶ����Ӻ��ʱ��Ҫ�����Ƶ����ӵ���Ϣ���и���
//ͬʱ�������pieceTableҲҪ����
void change(int8_t index,int8_t x,int8_t y,ChangeBuffer *p)   //index�ж����ĸ����� x,yΪ����λ��
{
    int8_t index2;

    p->indexSrc = index;             //���ƶ����̵����ݱ��浽buffer��
    p->AX = piece[index].x;
    p->AY = piece[index].y;
    p->BX = x;
    p->BY = y;
    index2 = pieceTable[x][y];
    p->indexDes = index2;

    pieceTable[piece[index].x][piece[index].y] = -1;//���������ߺ�û������������1��ʾ
    if(index2 != -1)                  //Ŀ�ĵ������ӵĻ���ɱ����������״̬��ΪDEATH(ǰ��ִ����·���жϺ�����������Բ��Ǽ�������)
    {
        piece[index2].status = DEATH;
    }
    pieceTable[x][y] = index;        //peiceTable��(x,y)λ�õõ����ƹ�����index
    piece[index].x = x;              //����λ�øı䣬�ṹ���ԱҪ�ı�
    piece[index].y = y;
}

//��Ӧ����һ��change��������change������Ч��undo��
//��ִ����change������ִ��backChange��������
//��change���ĸ�����ȫ��ԭ
void backChange(ChangeBuffer *p)
{
    int8_t index,index2;
    index = p->indexSrc;       //��ȡbuffer�������
    index2 = p->indexDes;

    piece[index].x = p->AX;    //���ӵ�λ�ø�ԭ
    piece[index].y = p->AY;
	
    if(p->indexDes != -1)
    {
        piece[p->indexDes].status = ALIVE;//������ӱ�ɱ������Ҫ��������
    }

    pieceTable[p->AX][p->AY] = index;    //�������Ҫ�Ĺ���
    pieceTable[p->BX][p->BY] = index2;
}


/*******end of file*****************/
