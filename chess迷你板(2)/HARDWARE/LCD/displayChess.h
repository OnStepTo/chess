#ifndef __DISPLAYCHESS_H__
#define __DISPLAYCHESS_H__

#include <sys.h>

//·���ṹ�壬ִ��getPath��һ��Path���͵Ĳ�����ָ�뽫�ᴫ�룬ִ����֮����������ᱣ�����
//���ӵ�·�������·����һ������ȷ��Ч�ģ��뾭�������жϲſ�֪
typedef struct 
{
    int8_t indexS; //���ѡ��������
    int8_t x;      //��ѡ�������ӽ����ƶ���Ŀ�ĵص����꣨x��y��
    int8_t y;
}Path;

void initUI(void);
extern void debugUI(u8 i,u8 j,u8 k);
void initPiece(void);
void drawSelection(void);
void clearSelection(void);
void selectPiece(u8 index);
void deSelectPiece(u8 index);
void makePiece(int8_t index);
void clearPiece(int8_t x,int8_t y);
void getPath(Path *p);


extern u8 lengthCH;
extern u8 lengthCH2;
extern u8 pieceR;
extern u16 sX;
extern u16 sY;
extern u8 sizeENG;
extern unsigned char * Libbuttom;
extern u16 turnPiX[9];
extern u16 turnPiY[10];


#endif

/*******end of file*****************/
