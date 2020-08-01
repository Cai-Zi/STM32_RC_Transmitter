#include "oled.h"
#include "stdlib.h" 	 
#include "delay.h"
#include "menu.h"
#include "nrf24l01.h"
#include "led.h"
#include "key.h"
#include "flash.h"
#include "adc.h"
#include "delay.h"
#include "usart.h"

/*https://blog.csdn.net/embedded_guzi/article/details/35835755
https://blog.csdn.net/calmuse/article/details/79346742
*/

Key_index_struct const Key_table[17]=
{
	//��ǰ, ��, ��, ȷ��, ����, home
    {0, 0, 0, 1, 0, 0,(*mainWindow)},//һ������
	{1, 2, 4, 5, 0, 0,(*menu1)},//���������һ�� ͨ��΢��
	{2, 3, 1, 9, 0, 0,(*menu2)},//��������ڶ��� ͨ������
	{3, 4, 2, 13,0, 0,(*menu3)},//������������� ͨ��У׼
	{4, 1, 3, 0, 0, 0,(*menu4)},//�˳�
	
	{5, 6, 8, 5, 1, 0,(*subMenu1_1)},//��������,menu1ѡ�е�1�� ͨ��΢��
	{6, 7, 5, 6, 1, 0,(*subMenu1_2)},//��������,menu1ѡ�е�2��
	{7, 8, 6, 7, 1, 0,(*subMenu1_3)},//��������,menu1ѡ�е�3��
	{8, 5, 7, 1, 1, 0,(*subMenu1_4)},//��������,menu1ѡ�е�4��
	
	{9 , 10, 12, 9,  2, 0,(*subMenu2_1)},//��������,menu2ѡ�е�1�� ͨ������
	{10, 11,  9, 10, 2, 0,(*subMenu2_2)},//��������,menu2ѡ�е�2��
	{11, 12, 10, 11, 2, 0,(*subMenu2_3)},//��������,menu2ѡ�е�3��
	{12,  9, 11, 2,  2, 0,(*subMenu2_4)},//��������,menu2ѡ�е�4��
	
	{13, 13, 13, 13, 3, 0,(*subMenu3_1)},//��������,menu3ѡ�е�1�� ͨ��У׼
};

u8 nowIndex = 0,lastIndex = 0;
extern unsigned char logo[];
extern unsigned char logoR[];

void OLED_display(void){
	switch(menuEvent[1]){
		case BM_down: //��ʱ����ת������
			nowIndex=Key_table[nowIndex].down_index;
			break;
		case BM_up: //˳ʱ����ת,����
			nowIndex=Key_table[nowIndex].up_index;
			break;
		case KEY_enter://ȷ��
			nowIndex=Key_table[nowIndex].enter_index;
			break;
		case KEY_esc://����
			nowIndex=Key_table[nowIndex].esc_index;
			break;
		case KEY_home://���� ����������
			nowIndex=Key_table[nowIndex].home_index;
			break;
		case NUM_down:
			break;
		case NUM_up:
			break;
	}
	if(nowIndex==0 && lastIndex>0) OLED_Fill(0,0,127,63,0);//���
	if(nowIndex>=1 && nowIndex<=4 && (lastIndex>4 | lastIndex<1)) OLED_Fill(0,0,127,63,0);//���
	if(nowIndex>=5 && nowIndex<=8 && (lastIndex>8 | lastIndex<5)) OLED_Fill(0,0,127,63,0);//���
	if(nowIndex>=9 && nowIndex<=12 && (lastIndex>12 | lastIndex<9)) OLED_Fill(0,0,127,63,0);//���
	if(nowIndex>=13 && nowIndex<=13 && (lastIndex<13)) OLED_Fill(0,0,127,63,0);//���
	if(nowIndex!=lastIndex)
	{
		Key_table[nowIndex].operate();
		OLED_Refresh_Gram();//ˢ���Դ�
	}
	printf("now:%d, last:%d, event:%d\n",nowIndex,lastIndex,menuEvent[1]);
	lastIndex = nowIndex;
}


void mainWindow(void)
{
	u16 thrNum;
	int loca;
	//u8 txt[16]={0};//����ַ����ı�
	//�м�ͼ��
	OLED_DrawPointBMP(9,0,logo,110,56,1);//��ʾlogo
	
	//��1ͨ��΢����ʶ-���-�Һ���
	OLED_Fill(66,59,124,62,0);//д0�����ԭ���ı�־
	loca = (int)95+setData.PWMadjustValue[0]/4;
	OLED_Fill(loca,59,loca,62,1);//д1
	OLED_DrawPlusSign(95,61);//���ı�ʶ
	OLED_Fill(66,63,124,63,1);//д1
	
	//��2ͨ��΢����ʶ-������-������
	OLED_Fill(123,1,126,63,0);//д0
	loca = (int)32-setData.PWMadjustValue[1]/4;
	OLED_Fill(123,loca,126,loca,1);//д1
	OLED_DrawPlusSign(125,32);//���ı�ʶ
	OLED_Fill(127,1,127,63,1);//д1
	
	//��3ͨ�������г���-������
	thrNum = (int)(PWMvalue[2]-1000)/16;
	OLED_Fill(0,63-thrNum,0,63,1);//�²���д1
	OLED_Fill(0,0,0,63-thrNum,0);//�ϲ���д0
	
	//��4ͨ��΢����ʶ-�����-�����
	OLED_Fill(4,59,62,62,0);//д0�����ԭ���ı�־
	loca = (int)33+setData.PWMadjustValue[3]/4;
	OLED_Fill(loca,59,loca,62,1);//д1
	OLED_DrawPlusSign(33,61);//���ı�ʶ
	OLED_Fill(4,63,62,63,1);//д1
}
void menu1(void)//�˵��б�
{
	OLED_ShowChinese(0, 0,0,16,0);//ͨ
	OLED_ShowChinese(16,0,1,16,0);//��
	OLED_ShowChinese(32,0,2,16,0);//΢
	OLED_ShowChinese(48,0,3,16,0);//��
	
	OLED_ShowChinese(0, 16,0,16,1);//ͨ
	OLED_ShowChinese(16,16,1,16,1);//��
	OLED_ShowChinese(32,16,4,16,1);//��
	OLED_ShowChinese(48,16,5,16,1);//��
	
	OLED_ShowChinese(0, 32,6,16,1);//��
	OLED_ShowChinese(16,32,7,16,1);//��
	OLED_ShowChinese(32,32,8,16,1);//У
	OLED_ShowChinese(48,32,9,16,1);//׼
	
	OLED_ShowChinese(0, 48,10,16,1);//��
	OLED_ShowChinese(16,48,11,16,1);//��
}
void menu2(void)//�˵��б�
{
	OLED_ShowChinese(0, 0,0,16,1);//ͨ
	OLED_ShowChinese(16,0,1,16,1);//��
	OLED_ShowChinese(32,0,2,16,1);//΢
	OLED_ShowChinese(48,0,3,16,1);//��
	
	OLED_ShowChinese(0, 16,0,16,0);//ͨ
	OLED_ShowChinese(16,16,1,16,0);//��
	OLED_ShowChinese(32,16,4,16,0);//��
	OLED_ShowChinese(48,16,5,16,0);//��
	
	OLED_ShowChinese(0, 32,6,16,1);//��
	OLED_ShowChinese(16,32,7,16,1);//��
	OLED_ShowChinese(32,32,8,16,1);//У
	OLED_ShowChinese(48,32,9,16,1);//׼
	
	OLED_ShowChinese(0, 48,10,16,1);//��
	OLED_ShowChinese(16,48,11,16,1);//��
}
void menu3(void)//�˵��б�
{
	OLED_ShowChinese(0, 0,0,16,1);//ͨ
	OLED_ShowChinese(16,0,1,16,1);//��
	OLED_ShowChinese(32,0,2,16,1);//΢
	OLED_ShowChinese(48,0,3,16,1);//��
	
	OLED_ShowChinese(0, 16,0,16,1);//ͨ
	OLED_ShowChinese(16,16,1,16,1);//��
	OLED_ShowChinese(32,16,4,16,1);//��
	OLED_ShowChinese(48,16,5,16,1);//��
	
	OLED_ShowChinese(0, 32,6,16,0);//��
	OLED_ShowChinese(16,32,7,16,0);//��
	OLED_ShowChinese(32,32,8,16,0);//У
	OLED_ShowChinese(48,32,9,16,0);//׼
	
	OLED_ShowChinese(0, 48,10,16,1);//��
	OLED_ShowChinese(16,48,11,16,1);//��
}
void menu4(void)//�˵��б�-�˳�
{
	OLED_ShowChinese(0, 0,0,16,1);//ͨ
	OLED_ShowChinese(16,0,1,16,1);//��
	OLED_ShowChinese(32,0,2,16,1);//΢
	OLED_ShowChinese(48,0,3,16,1);//��
	
	OLED_ShowChinese(0, 16,0,16,1);//ͨ
	OLED_ShowChinese(16,16,1,16,1);//��
	OLED_ShowChinese(32,16,4,16,1);//��
	OLED_ShowChinese(48,16,5,16,1);//��
	
	OLED_ShowChinese(0, 32,6,16,1);//��
	OLED_ShowChinese(16,32,7,16,1);//��
	OLED_ShowChinese(32,32,8,16,1);//У
	OLED_ShowChinese(48,32,9,16,1);//׼
	
	OLED_ShowChinese(0, 48,10,16,0);//��
	OLED_ShowChinese(16,48,11,16,0);//��
}
//=======================�Ӳ˵�1==========================
void subMenu1_1(void)//�Ӳ˵��б�
{
	OLED_ShowChinese(0, 0,0,16,0);//ͨ
	OLED_ShowChinese(16,0,1,16,0);//��
	OLED_ShowString(32,0, (u8 *)"1",16,0);
	OLED_ShowNum(48,0,abs(setData.PWMadjustValue[0]),3,16);
	if(setData.PWMadjustValue[0]<0) OLED_ShowString(74,0, (u8 *)"-",16,1);
	else OLED_ShowString(74,0, (u8 *)"+",16,1);
	
	OLED_ShowChinese(0, 16,0,16,1);//ͨ
	OLED_ShowChinese(16,16,1,16,1);//��
	OLED_ShowString(32,16, (u8 *)"2",16,1);
	OLED_ShowNum(48,16,abs(setData.PWMadjustValue[1]),3,16);
	if(setData.PWMadjustValue[1]<0) OLED_ShowString(74,16, (u8 *)"-",16,1);
	else OLED_ShowString(74,16, (u8 *)"+",16,1);
	
	OLED_ShowChinese(0, 32,0,16,1);//ͨ
	OLED_ShowChinese(16,32,1,16,1);//��
	OLED_ShowString(32,32, (u8 *)"3",16,1);
	OLED_ShowNum(48,32,abs(setData.PWMadjustValue[2]),3,16);
	if(setData.PWMadjustValue[2]<0) OLED_ShowString(74,32, (u8 *)"-",16,1);
	else OLED_ShowString(74,32, (u8 *)"+",16,1);
	
	OLED_ShowChinese(0, 48,0,16,1);//ͨ
	OLED_ShowChinese(16,48,1,16,1);//��
	OLED_ShowString(32,48, (u8 *)"4",16,1);
	OLED_ShowNum(48,48,abs(setData.PWMadjustValue[3]),3,16);
	if(setData.PWMadjustValue[3]<0) OLED_ShowString(74,48, (u8 *)"-",16,1);
	else OLED_ShowString(74,48, (u8 *)"+",16,1);
}
void subMenu1_2(void)//�Ӳ˵��б�
{
	OLED_ShowChinese(0, 0,0,16,1);//ͨ
	OLED_ShowChinese(16,0,1,16,1);//��
	OLED_ShowString(32,0, (u8 *)"1",16,1);
	OLED_ShowNum(48,0,abs(setData.PWMadjustValue[0]),3,16);
	if(setData.PWMadjustValue[0]<0) OLED_ShowString(74,0, (u8 *)"-",16,1);
	else OLED_ShowString(74,0, (u8 *)"+",16,1);
	
	OLED_ShowChinese(0, 16,0,16,0);//ͨ
	OLED_ShowChinese(16,16,1,16,0);//��
	OLED_ShowString(32,16, (u8 *)"2",16,0);
	OLED_ShowNum(48,16,abs(setData.PWMadjustValue[1]),3,16);
	if(setData.PWMadjustValue[1]<0) OLED_ShowString(74,16, (u8 *)"-",16,1);
	else OLED_ShowString(74,16, (u8 *)"+",16,1);
	
	OLED_ShowChinese(0, 32,0,16,1);//ͨ
	OLED_ShowChinese(16,32,1,16,1);//��
	OLED_ShowString(32,32, (u8 *)"3",16,1);
	OLED_ShowNum(48,32,abs(setData.PWMadjustValue[2]),3,16);
	if(setData.PWMadjustValue[2]<0) OLED_ShowString(74,32, (u8 *)"-",16,1);
	else OLED_ShowString(74,32, (u8 *)"+",16,1);
	
	OLED_ShowChinese(0, 48,0,16,1);//ͨ
	OLED_ShowChinese(16,48,1,16,1);//��
	OLED_ShowString(32,48, (u8 *)"4",16,1);
	OLED_ShowNum(48,48,abs(setData.PWMadjustValue[3]),3,16);
	if(setData.PWMadjustValue[3]<0) OLED_ShowString(74,48, (u8 *)"-",16,1);
	else OLED_ShowString(74,48, (u8 *)"+",16,1);
}
void subMenu1_3(void)//�Ӳ˵��б�
{
	OLED_ShowChinese(0, 0,0,16,1);//ͨ
	OLED_ShowChinese(16,0,1,16,1);//��
	OLED_ShowString(32,0, (u8 *)"1",16,1);
	OLED_ShowNum(48,0,abs(setData.PWMadjustValue[0]),3,16);
	if(setData.PWMadjustValue[0]<0) OLED_ShowString(74,0, (u8 *)"-",16,1);
	else OLED_ShowString(74,0, (u8 *)"+",16,1);
	
	OLED_ShowChinese(0, 16,0,16,1);//ͨ
	OLED_ShowChinese(16,16,1,16,1);//��
	OLED_ShowString(32,16, (u8 *)"2",16,1);
	OLED_ShowNum(48,16,abs(setData.PWMadjustValue[1]),3,16);
	if(setData.PWMadjustValue[1]<0) OLED_ShowString(74,16, (u8 *)"-",16,1);
	else OLED_ShowString(74,16, (u8 *)"+",16,1);
	
	OLED_ShowChinese(0, 32,0,16,0);//ͨ
	OLED_ShowChinese(16,32,1,16,0);//��
	OLED_ShowString(32,32, (u8 *)"3",16,0);
	OLED_ShowNum(48,32,abs(setData.PWMadjustValue[2]),3,16);
	if(setData.PWMadjustValue[2]<0) OLED_ShowString(74,32, (u8 *)"-",16,1);
	else OLED_ShowString(74,32, (u8 *)"+",16,1);
	
	OLED_ShowChinese(0, 48,0,16,1);//ͨ
	OLED_ShowChinese(16,48,1,16,1);//��
	OLED_ShowString(32,48, (u8 *)"4",16,1);
	OLED_ShowNum(48,48,abs(setData.PWMadjustValue[3]),3,16);
	if(setData.PWMadjustValue[3]<0) OLED_ShowString(74,48, (u8 *)"-",16,1);
	else OLED_ShowString(74,48, (u8 *)"+",16,1);
}
void subMenu1_4(void)//�Ӳ˵��б�
{
	OLED_ShowChinese(0, 0,0,16,1);//ͨ
	OLED_ShowChinese(16,0,1,16,1);//��
	OLED_ShowString(32,0, (u8 *)"1",16,1);
	OLED_ShowNum(48,0,abs(setData.PWMadjustValue[0]),3,16);
	if(setData.PWMadjustValue[0]<0) OLED_ShowString(74,0, (u8 *)"-",16,1);
	else OLED_ShowString(74,0, (u8 *)"+",16,1);
	
	OLED_ShowChinese(0, 16,0,16,1);//ͨ
	OLED_ShowChinese(16,16,1,16,1);//��
	OLED_ShowString(32,16, (u8 *)"2",16,1);
	OLED_ShowNum(48,16,abs(setData.PWMadjustValue[1]),3,16);
	if(setData.PWMadjustValue[1]<0) OLED_ShowString(74,16, (u8 *)"-",16,1);
	else OLED_ShowString(74,16, (u8 *)"+",16,1);
	
	OLED_ShowChinese(0, 32,0,16,1);//ͨ
	OLED_ShowChinese(16,32,1,16,1);//��
	OLED_ShowString(32,32, (u8 *)"3",16,1);
	OLED_ShowNum(48,32,abs(setData.PWMadjustValue[2]),3,16);
	if(setData.PWMadjustValue[2]<0) OLED_ShowString(74,32, (u8 *)"-",16,1);
	else OLED_ShowString(74,32, (u8 *)"+",16,1);
	
	OLED_ShowChinese(0, 48,0,16,0);//ͨ
	OLED_ShowChinese(16,48,1,16,0);//��
	OLED_ShowString(32,48, (u8 *)"4",16,0);
	OLED_ShowNum(48,48,abs(setData.PWMadjustValue[3]),3,16);
	if(setData.PWMadjustValue[3]<0) OLED_ShowString(74,48, (u8 *)"-",16,1);
	else OLED_ShowString(74,48, (u8 *)"+",16,1);
}
//======================�Ӳ˵�2 ͨ������===========================
void subMenu2_1(void)//�Ӳ˵��б�
{
	OLED_ShowChinese(0, 0,0,16,0);//ͨ
	OLED_ShowChinese(16,0,1,16,0);//��
	OLED_ShowString(32,0, (u8 *)"1",16,0);
	if(setData.chReverse[0])OLED_ShowChinese(48,0,4,16,1);//��
	else OLED_ShowChinese(48,0,5,16,1);//��
	
	OLED_ShowChinese(0, 16,0,16,1);//ͨ
	OLED_ShowChinese(16,16,1,16,1);//��
	OLED_ShowString(32,16, (u8 *)"2",16,1);
	if(setData.chReverse[1])OLED_ShowChinese(48,16,4,16,1);//��
	else OLED_ShowChinese(48,16,5,16,1);//��
	
	OLED_ShowChinese(0, 32,0,16,1);//ͨ
	OLED_ShowChinese(16,32,1,16,1);//��
	OLED_ShowString(32,32, (u8 *)"3",16,1);
	if(setData.chReverse[2])OLED_ShowChinese(48,32,4,16,1);//��
	else OLED_ShowChinese(48,32,5,16,1);//��
	
	OLED_ShowChinese(0, 48,0,16,1);//ͨ
	OLED_ShowChinese(16,48,1,16,1);//��
	OLED_ShowString(32,48, (u8 *)"4",16,1);
	if(setData.chReverse[3])OLED_ShowChinese(48,48,4,16,1);//��
	else OLED_ShowChinese(48,48,5,16,1);//��
}
void subMenu2_2(void)//�Ӳ˵��б�
{
	OLED_ShowChinese(0, 0,0,16,1);//ͨ
	OLED_ShowChinese(16,0,1,16,1);//��
	OLED_ShowString(32,0, (u8 *)"1",16,1);
	if(setData.chReverse[0])OLED_ShowChinese(48,0,4,16,1);//��
	else OLED_ShowChinese(48,0,5,16,1);//��
	
	OLED_ShowChinese(0, 16,0,16,0);//ͨ
	OLED_ShowChinese(16,16,1,16,0);//��
	OLED_ShowString(32,16, (u8 *)"2",16,0);
	if(setData.chReverse[1])OLED_ShowChinese(48,16,4,16,1);//��
	else OLED_ShowChinese(48,16,5,16,1);//��
	
	OLED_ShowChinese(0, 32,0,16,1);//ͨ
	OLED_ShowChinese(16,32,1,16,1);//��
	OLED_ShowString(32,32, (u8 *)"3",16,1);
	if(setData.chReverse[2])OLED_ShowChinese(48,32,4,16,1);//��
	else OLED_ShowChinese(48,32,5,16,1);//��
	
	OLED_ShowChinese(0, 48,0,16,1);//ͨ
	OLED_ShowChinese(16,48,1,16,1);//��
	OLED_ShowString(32,48, (u8 *)"4",16,1);
	if(setData.chReverse[3])OLED_ShowChinese(48,48,4,16,1);//��
	else OLED_ShowChinese(48,48,5,16,1);//��
}
void subMenu2_3(void)//�Ӳ˵��б�
{
	OLED_ShowChinese(0, 0,0,16,1);//ͨ
	OLED_ShowChinese(16,0,1,16,1);//��
	OLED_ShowString(32,0, (u8 *)"1",16,1);
	if(setData.chReverse[0])OLED_ShowChinese(48,0,4,16,1);//��
	else OLED_ShowChinese(48,0,5,16,1);//��
	
	OLED_ShowChinese(0, 16,0,16,1);//ͨ
	OLED_ShowChinese(16,16,1,16,1);//��
	OLED_ShowString(32,16, (u8 *)"2",16,1);
	if(setData.chReverse[1])OLED_ShowChinese(48,16,4,16,1);//��
	else OLED_ShowChinese(48,16,5,16,1);//��
	
	OLED_ShowChinese(0, 32,0,16,0);//ͨ
	OLED_ShowChinese(16,32,1,16,0);//��
	OLED_ShowString(32,32, (u8 *)"3",16,0);
	if(setData.chReverse[2])OLED_ShowChinese(48,32,4,16,1);//��
	else OLED_ShowChinese(48,32,5,16,1);//��
	
	OLED_ShowChinese(0, 48,0,16,1);//ͨ
	OLED_ShowChinese(16,48,1,16,1);//��
	OLED_ShowString(32,48, (u8 *)"4",16,1);
	if(setData.chReverse[3])OLED_ShowChinese(48,48,4,16,1);//��
	else OLED_ShowChinese(48,48,5,16,1);//��
}
void subMenu2_4(void)//�Ӳ˵��б�
{
	OLED_ShowChinese(0, 0,0,16,1);//ͨ
	OLED_ShowChinese(16,0,1,16,1);//��
	OLED_ShowString(32,0, (u8 *)"1",16,1);
	if(setData.chReverse[0])OLED_ShowChinese(48,0,4,16,1);//��
	else OLED_ShowChinese(48,0,5,16,1);//��
	
	OLED_ShowChinese(0, 16,0,16,1);//ͨ
	OLED_ShowChinese(16,16,1,16,1);//��
	OLED_ShowString(32,16, (u8 *)"2",16,1);
	if(setData.chReverse[1])OLED_ShowChinese(48,16,4,16,1);//��
	else OLED_ShowChinese(48,16,5,16,1);//��
	
	OLED_ShowChinese(0, 32,0,16,1);//ͨ
	OLED_ShowChinese(16,32,1,16,1);//��
	OLED_ShowString(32,32, (u8 *)"3",16,1);
	if(setData.chReverse[2])OLED_ShowChinese(48,32,4,16,1);//��
	else OLED_ShowChinese(48,32,5,16,1);//��
	
	OLED_ShowChinese(0, 48,0,16,0);//ͨ
	OLED_ShowChinese(16,48,1,16,0);//��
	OLED_ShowString(32,48, (u8 *)"4",16,0);
	if(setData.chReverse[3])OLED_ShowChinese(48,48,4,16,1);//��
	else OLED_ShowChinese(48,48,5,16,1);//��
}
//======================�Ӳ˵�3===========================
void subMenu3_1(void)//�Ӳ˵��б�
{
	OLED_ShowChinese(0, 0,0,16,1);//ͨ
	OLED_ShowChinese(16,0,1,16,1);//��
	OLED_ShowString(32,0, (u8 *)"1",16,1);
	OLED_ShowNum(40,0,setData.chLower[0],4,16);
	OLED_ShowNum(80,0,setData.chUpper[0],4,16);
	
	OLED_ShowChinese(0, 16,0,16,1);//ͨ
	OLED_ShowChinese(16,16,1,16,1);//��
	OLED_ShowString(32,16, (u8 *)"2",16,1);
	OLED_ShowNum(40,16,setData.chLower[1],4,16);
	OLED_ShowNum(80,16,setData.chUpper[1],4,16);
	
	OLED_ShowChinese(0, 32,0,16,1);//ͨ
	OLED_ShowChinese(16,32,1,16,1);//��
	OLED_ShowString(32,32, (u8 *)"3",16,1);
	OLED_ShowNum(40,32,setData.chLower[2],4,16);
	OLED_ShowNum(80,32,setData.chUpper[2],4,16);
	
	OLED_ShowChinese(0, 48,0,16,1);//ͨ
	OLED_ShowChinese(16,48,1,16,1);//��
	OLED_ShowString(32,48, (u8 *)"4",16,1);
	OLED_ShowNum(40,48,setData.chLower[3],4,16);
	OLED_ShowNum(80,48,setData.chUpper[3],4,16);
}
