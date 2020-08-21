#ifndef __MENU_H
#define __MENU_H			  	 
#include "sys.h"
#include "stdlib.h"

typedef struct{
	u8 current_index;	//��ŵ�ǰ����������ţ�
	u8 down_index;		//��Ű��¡�down�����£�����ʱ��Ҫ��ת���������ţ�
	u8 up_index;		//��Ű��¡�up�����ϣ�����ʱ��Ҫ��ת���������ţ�
	u8 enter_index;		//��Ű��¡�enter�����룩����ʱ��Ҫ��ת�������ţ�
	u8 esc_index;		//��Ű��¡�esc���˳�������ʱ��Ҫ��ת�������ţ�
	u8 home_index;		//��Ű��¡�home�������棩����ʱ��Ҫ��ת�������ţ�
	void (*operate)();	//����ָ���������ŵ�ǰ����������Ҫִ�еĺ�������ڵ�ַ��
}Key_index_struct;

typedef struct Lcd_menu_content{
	u8 *lpIcon; //��ʾͼ��
	u8 *lpText; //��ʾ�ı���Ϣ
	u8 nTextCount; //�˵���Ӧ���ı���Ϣ�ĸ���
}Lcd_menu_content;

typedef struct Lcd_menu{
	struct Lcd_menu *lpfather; //����
	struct Lcd_menu *lpson; //�����
	u8 nSonCount; //�����Ķ��Ӹ���
	Lcd_menu_content lpIconAndText;
	u8 Flag_return; //���ر�־
	void  (*operate)(); //��������ڵ�ַ
}Lcd_menu;
extern volatile u8 nowMenuIndex;
extern volatile u8 lastMenuIndex;

void OLED_display(void);
void mainWindow(void);//������
void menu1(void);//�˵��б�
void menu2(void);//�˵��б�
void menu3(void);//�˵��б�
void menu4(void);//�˵��б�

void subMenu1_1(void);//�Ӳ˵�
void subMenu1_2(void);//�Ӳ˵�
void subMenu1_3(void);//�Ӳ˵�
void subMenu1_4(void);//�Ӳ˵�

void subMenu2_1(void);//�Ӳ˵�
void subMenu2_2(void);//�Ӳ˵�
void subMenu2_3(void);//�Ӳ˵�
void subMenu2_4(void);//�Ӳ˵�

void subMenu3_1(void);//�Ӳ˵�
void subMenu3_2(void);//�Ӳ˵�
void subMenu3_3(void);//�Ӳ˵�
void subMenu3_4(void);//�Ӳ˵�

void subMenu4_1(void);//�Ӳ˵�
void subMenu4_2(void);//�Ӳ˵�
void subMenu4_3(void);//�Ӳ˵�
void subMenu4_4(void);//�Ӳ˵�
#endif

