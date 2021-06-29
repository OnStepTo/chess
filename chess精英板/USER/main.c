/*****************************************************************************
Copyright: δ����ɣ�����ת��
File name: main.c
Description: ��������������������
Author: guoxl
Version: �汾
Date: �������
History: �޸���ʷ��¼�б� ÿ���޸ļ�¼Ӧ�����޸����ڡ��޸��߼��޸����ݼ�����
*****************************************************************************/
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "displayChess.h"
#include "chess.h"
#include "touch.h"
#include "exti.h"
#include "beep.h"
#include "check.h"

int8_t selectIndex = -2;
int8_t gameRunFlag;


 int main(void)
 {
	PieceTypedef tempPiece;
	Path path;
	ChangeBuffer changeBuffer;
	int8_t clearFlag;
	int8_t assassin;//�̿ͣ����ĸ����ӽ�����

	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(115200);
	KEY_Init();
	BEEP_Init();
	extiInit();
	LCD_Init();
	tp_dev.init();
	POINT_COLOR = RED;	 
	initUI();          //����ĳ�ʼ����

	while (1)
	{
		getPath(&path);
		tempPiece = piece[path.indexS];
		
		/*�жϵõ���path�Ƿ���Ϲ��򣬷����򷵻�1*/
		if(tempPiece.judge(tempPiece.x,tempPiece.y,path.x,path.y,TURN))
		{
			change(path.indexS,path.x,path.y,&changeBuffer);//�Ȱ����ӵ����ݺ�������ݸ��ģ����沢û�иı䣬��Ϊ��û�ж��������Ƿ񱻽���

			/*�ж�ִ��change()�����Ƿ񱻽��������������ƶ���Ч����ҪbackChange()*/
			if(check(!tempPiece.camp,&assassin) || generalLine())
			{
				backChange(&changeBuffer);//�����߲��У��ᱻ���������Գ����ƶ�
				deSelectPiece(path.indexS);//����Ļ��ѡ�е�����ȡ��ѡ��
				
				write_Chinese(sX,sY,'��',DARKBLUE,Libbuttom,lengthCH2);
				write_Chinese(sX+lengthCH2,sY,'Ч',DARKBLUE,Libbuttom,lengthCH2);
				clearFlag = 1;
			}
			else//�����߿���
			{
				clearPiece(changeBuffer.AX,changeBuffer.AY);//�Ѿ����Ӳ���,ע�⣬������ŸĶ�����
				makePiece(path.indexS);              //���ƶ�·����Ŀ�ĵػ�һ��������
				changeBuffer2 = changeBuffer;     //�ṹ�帳ֵ��changeBuffer2ר�������壬���洢�����������ӹ��̵�����
				undoEn = 1;                      //���������־λ��ÿ������֮��ʹ�ܣ�����֮���ֹ��

				if(check(tempPiece.camp,&assassin))//ÿ�����Ӻ��ж��Ƿ񽫾�
				{
					PBout(8) = 1;     //��������
					if(judgeWin(assassin))//��������˽��Ž�һ���ж��Ƿ���
					{
						POINT_COLOR = CYAN;
						LCD_Fill(sX,sY,sX+2*lengthCH2,sY+lengthCH2,BACK_COLOR);
						LCD_ShowString(sX+3*lengthCH2,sY,sizeENG*6,sizeENG,sizeENG," game over!");

						if(TURN)
						write_Chinese(sX,sY,'��',BLACK,Libbuttom,lengthCH2);
						else
						write_Chinese(sX,sY,'��',RED,Libbuttom,lengthCH2);

						write_Chinese(sX+lengthCH2,sY,'��',CYAN,Libbuttom,lengthCH2);
						write_Chinese(sX+lengthCH2*2,sY,'ʤ',CYAN,Libbuttom,lengthCH2);

						PBout(8) = 0;
						changeBuffer2.indexSrc = -2;//�ж���������ֵ����⵽��return
						while(1);
					}
					write_Chinese(sX,sY,'��',TURN?BLACK:RED,Libbuttom,lengthCH2);
					write_Chinese(sX+lengthCH2,sY,'��',TURN?BLACK:RED,Libbuttom,lengthCH2);
					clearFlag = 1;    //��һ�ο��Բ�����ʾ
					PBout(8) = 0;     //�رշ�����
				}
				else if(clearFlag)    //�Ƿ�����ʾ��ϢҪ����
				{
					LCD_Fill(sX,sY,sX+2*lengthCH2,sY+lengthCH2,BACK_COLOR);
					clearFlag = 0;
				}

				TURN = !TURN;
			}
		}
		else//�õ���path·�������Ϲ���
		{
			deSelectPiece(path.indexS);       //ȡ��ѡ��״̬
			write_Chinese(sX,sY,'��',DARKBLUE,Libbuttom,lengthCH2);
			write_Chinese(sX+lengthCH2,sY,'Ч',DARKBLUE,Libbuttom,lengthCH2);
			clearFlag = 1;
		}
	}
}
 
void EXTI4_IRQHandler(void)     //�����жϣ����崦����
{
    int8_t index2 = changeBuffer2.indexDes;
    int8_t index = changeBuffer2.indexSrc;
    int8_t tmpX,tmpY;
    int8_t noUse;
    tmpX = piece[index].x;
    tmpY = piece[index].y;

	
    if(changeBuffer2.indexSrc == -2)//�ո�λ��״̬�������ǽ���״̬����ʱ��������return����
    {
		  EXTI_ClearITPendingBit(EXTI_Line4);    
        return;
    }
	 
    delay_ms(10);  //��ʱ����

    if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)==0 && undoEn==1)
    {

        if(selectIndex != -2)//������ʱ������Ѿ�ѡ�������ӣ���ȡ��ѡ��
        {
            deSelectPiece(selectIndex);
        }
        
        backChange(&changeBuffer2);//�������ӹ��̵����ݸ���
		  
        if(index2 != -1)//���ԭ������Ŀ�ĵ������ӵĻ���Ҫ����������
        {
            makePiece(index2);
        }
        else//û�еĻ����������Ӽ���
			  
        clearPiece(tmpX,tmpY);
        makePiece(index);//�����ӵ�Դλ�ð����ӻ�����

        if(check(TURN,&noUse) || generalLine())//����Ƿ񱻽���
        {
            write_Chinese(sX,sY,'��',TURN?BLACK:RED,Libbuttom,lengthCH2);
            write_Chinese(sX+lengthCH2,sY,'��',TURN?BLACK:RED,Libbuttom,lengthCH2);
        }
        else
        {
            LCD_Fill(sX,sY,sX+lengthCH2*2,sY+lengthCH2,BACK_COLOR);
        }

        TURN = !TURN;//��һ������

        undoEn = 0;//����֮�������ٻڣ�����֮����ʹ�ܻ���
    }
	 
    while(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4));//�ȴ�����̧��
    EXTI_ClearITPendingBit(EXTI_Line4);    
}

/*******end of file*****************/

