#ifndef __FONT_H
#define __FONT_H 	   
//����ASCII��
//ƫ����32 
//ASCII�ַ���: !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
//PC2LCD2002ȡģ��ʽ���ã�����+����ʽ+˳��+C51��ʽ
//�ܹ���3���ַ�����12*12��16*16��24*24�����û������������������ֱ��ʵ��ַ�����
//ÿ���ַ���ռ�õ��ֽ���Ϊ:(size/8+((size%8)?1:0))*(size/2),����size:���ֿ�����ʱ�ĵ����С(12/16/24...)
 
//12*12 ASCII�ַ�������
extern const unsigned int Chinese32[];
extern const unsigned int Chinese64[];
extern const unsigned char asc2_1206[95][12];
extern const unsigned char asc2_1608[95][16];
extern const unsigned char asc2_2412[95][36];
extern const unsigned char GB_3232[][128];
extern const unsigned char GB_6464[][512]; 
extern const unsigned char GB_1616[][32];
extern const unsigned char GB_5656[][392];


#endif


/*******end of file*****************/
