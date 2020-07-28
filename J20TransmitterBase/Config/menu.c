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
	//��ǰ, ��, ��, ȷ��, home
    {0, 0, 0, 1, 0,(*mainWindow)},//һ������
	{1, 2, 4, 5, 0,(*menu1)},//���������һ��
	{2, 3, 1, 9, 0,(*menu2)},//��������ڶ���
	{3, 4, 2, 13, 0,(*menu3)},//�������������
	{4, 1, 3, 0, 0,(*menu4)},//�˳�
	
	{5, 6, 8, 5, 0,(*subMenu1_1)},//��������,menu1ѡ�е�1��
	{6, 7, 5, 6, 0,(*subMenu1_2)},//menu1���������2��
	{7, 8, 6, 7, 0,(*subMenu1_3)},//menu1���������3��
	{8, 5, 7, 1, 0,(*subMenu1_4)},//menu1���������4��
	
	{9 , 10, 12,  9, 0,(*subMenu2_1)},//menu2���������1��
	{10, 11,  9, 10, 0,(*subMenu2_2)},//menu2���������2��
	{11, 12, 10, 11, 0,(*subMenu2_3)},//menu2���������3��
	{12,  9, 11, 2, 0,(*subMenu2_4)},//menu2���������4��
	
	{13, 14, 16, 13, 0,(*subMenu3_1)},//menu3���������1��
	{14, 15, 13, 14, 0,(*subMenu3_2)},//menu3���������2��
	{15, 16, 14, 15, 0,(*subMenu3_3)},//menu3���������3��
	{16, 13, 15, 3, 0,(*subMenu3_4)},//menu3���������4��
};

u8 nowIndex = 0,lastIndex = 0;
extern unsigned char logo[];
extern unsigned char logoR[];

void OLED_display(void){
	switch(encoderEvent[1]){
		case BM_down: //��ʱ����ת������
			nowIndex=Key_table[nowIndex].down_index;
		break;
		case BM_up: //˳ʱ����ת,����
			nowIndex=Key_table[nowIndex].up_index;
		break;
		case KEY_DOWN://�̰� ȷ������
			nowIndex=Key_table[nowIndex].enter_index;
		break;
		case KEY_LONG://���� ����������
			nowIndex=Key_table[nowIndex].esc_index;
		break;
	}
	if(nowIndex==0 && lastIndex>0) OLED_Fill(0,0,127,63,0);//���
	if(nowIndex>=1 && nowIndex<=4 && (lastIndex>4 | lastIndex==0)) OLED_Fill(0,0,127,63,0);//���
	if(nowIndex!=lastIndex)
	{
		Key_table[nowIndex].operate();
		OLED_Refresh_Gram();//ˢ���Դ�
	}
	lastIndex = nowIndex;
}


void mainWindow(void)
{
	u16 thrNum;
	int loca;
	//u8 txt[16]={0};//����ַ����ı�
	//�м���ĸ
	OLED_ShowString(35,2,(u8 *)"J-20",24,1);//������
	OLED_ShowString(20,35,(u8 *)"RADIO CONTROL",12,1);//������
	
	//��1ͨ��΢����ʶ-���-�Һ���
	OLED_Fill(66,59,124,62,0);//д0�����ԭ���ı�־
	loca = (int)95+setData.PWMadjustValue[0]/4;
	OLED_Fill(loca,59,loca,62,1);//д1
	OLED_DrawPlusSign(95,61);//���ı�ʶ
	OLED_Fill(66,63,124,63,1);//д1
	
	//��2ͨ��΢����ʶ-������-������
	OLED_Fill(123,1,126,63,0);//д0
	loca = (int)32+setData.PWMadjustValue[1]/4;
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
	OLED_ShowString(0,0, (u8 *)"subMenu1_1  ",16,0);
	OLED_ShowString(0,16,(u8 *)"subMenu1_2  ",16,1);
	OLED_ShowString(0,32,(u8 *)"subMenu1_3  ",16,1);
	OLED_ShowString(0,48,(u8 *)"exit        ",16,1);
	OLED_Refresh_Gram();//ˢ���Դ�
}
void subMenu1_2(void)//�Ӳ˵��б�
{
	OLED_ShowString(0,0, (u8 *)"subMenu1_1  ",16,1);
	OLED_ShowString(0,16,(u8 *)"subMenu1_2  ",16,0);
	OLED_ShowString(0,32,(u8 *)"subMenu1_3  ",16,1);
	OLED_ShowString(0,48,(u8 *)"exit        ",16,1);
	OLED_Refresh_Gram();//ˢ���Դ�
}
void subMenu1_3(void)//�Ӳ˵��б�
{
	OLED_ShowString(0,0, (u8 *)"subMenu1_1  ",16,1);
	OLED_ShowString(0,16,(u8 *)"subMenu1_2  ",16,1);
	OLED_ShowString(0,32,(u8 *)"subMenu1_3  ",16,0);
	OLED_ShowString(0,48,(u8 *)"exit        ",16,1);
	OLED_Refresh_Gram();//ˢ���Դ�
}
void subMenu1_4(void)//�Ӳ˵��б�
{
	OLED_ShowString(0,0, (u8 *)"subMenu1_1  ",16,1);
	OLED_ShowString(0,16,(u8 *)"subMenu1_2  ",16,1);
	OLED_ShowString(0,32,(u8 *)"subMenu1_3  ",16,1);
	OLED_ShowString(0,48,(u8 *)"exit        ",16,0);
	OLED_Refresh_Gram();//ˢ���Դ�
}
//======================�Ӳ˵�2===========================
void subMenu2_1(void)//�Ӳ˵��б�
{
	OLED_ShowString(0,0, (u8 *)"subMenu2_1  ",16,0);
	OLED_ShowString(0,16,(u8 *)"subMenu2_2  ",16,1);
	OLED_ShowString(0,32,(u8 *)"subMenu2_3  ",16,1);
	OLED_ShowString(0,48,(u8 *)"exit        ",16,1);
}
void subMenu2_2(void)//�Ӳ˵��б�
{
	OLED_ShowString(0,0, (u8 *)"subMenu2_1  ",16,1);
	OLED_ShowString(0,16,(u8 *)"subMenu2_2  ",16,0);
	OLED_ShowString(0,32,(u8 *)"subMenu2_3  ",16,1);
	OLED_ShowString(0,48,(u8 *)"exit        ",16,1);
}
void subMenu2_3(void)//�Ӳ˵��б�
{
	OLED_ShowString(0,0, (u8 *)"subMenu2_1  ",16,1);
	OLED_ShowString(0,16,(u8 *)"subMenu2_2  ",16,1);
	OLED_ShowString(0,32,(u8 *)"subMenu2_3  ",16,0);
	OLED_ShowString(0,48,(u8 *)"exit        ",16,1);
}
void subMenu2_4(void)//�Ӳ˵��б�
{
	OLED_ShowString(0,0, (u8 *)"subMenu2_1  ",16,1);
	OLED_ShowString(0,16,(u8 *)"subMenu2_2  ",16,1);
	OLED_ShowString(0,32,(u8 *)"subMenu2_3  ",16,1);
	OLED_ShowString(0,48,(u8 *)"exit        ",16,0);
}
//======================�Ӳ˵�3===========================
void subMenu3_1(void)//�Ӳ˵��б�
{
	OLED_ShowString(0,0, (u8 *)"subMenu3_1  ",16,0);
	OLED_ShowString(0,16,(u8 *)"subMenu3_2  ",16,1);
	OLED_ShowString(0,32,(u8 *)"subMenu3_3  ",16,1);
	OLED_ShowString(0,48,(u8 *)"exit        ",16,1);
}
void subMenu3_2(void)//�Ӳ˵��б�
{
	OLED_ShowString(0,0, (u8 *)"subMenu3_1  ",16,1);
	OLED_ShowString(0,16,(u8 *)"subMenu3_2  ",16,0);
	OLED_ShowString(0,32,(u8 *)"subMenu3_3  ",16,1);
	OLED_ShowString(0,48,(u8 *)"exit        ",16,1);
}
void subMenu3_3(void)//�Ӳ˵��б�
{
	OLED_ShowString(0,0, (u8 *)"subMenu3_1  ",16,1);
	OLED_ShowString(0,16,(u8 *)"subMenu3_2  ",16,1);
	OLED_ShowString(0,32,(u8 *)"subMenu3_3  ",16,0);
	OLED_ShowString(0,48,(u8 *)"exit        ",16,1);
}
void subMenu3_4(void)//�Ӳ˵��б�
{
	OLED_ShowString(0,0, (u8 *)"subMenu3_1  ",16,1);
	OLED_ShowString(0,16,(u8 *)"subMenu3_2  ",16,1);
	OLED_ShowString(0,32,(u8 *)"subMenu3_3  ",16,1);
	OLED_ShowString(0,48,(u8 *)"exit        ",16,0);
}


