#include "oled.h"
#include "stdlib.h" 	 
#include "delay.h"
#include "menu.h"
#include "nrf24l01.h"
#include "key.h"
#include "flash.h"
#include "adc.h"
#include "delay.h"
#include "usart.h"

/*https://blog.csdn.net/embedded_guzi/article/details/35835755
https://blog.csdn.net/calmuse/article/details/79346742
*/

Key_index_struct const Key_table[MENU_NUM]=
{
	//��ǰ, ��, ��, ȷ��, ����, home
  {home, home, home, tdwt, home, home,(*mainWindow)},//��ҳ�棺home
	
	{tdwt, tdzf, gybj, tdwt1, home, home,(*menu_tdwt)},//һ���˵���ͨ��΢��
	{tdzf, hksz, tdwt, tdzf1, home, home,(*menu_tdzf)},//һ���˵���ͨ������
	{hksz, jsgl, tdzf, sjyhk, home, home,(*menu_hksz)},//һ���˵����������
	{jsgl, scsz, hksz, xjjs, home, home,(*menu_jsgl)},//һ���˵������չ���
	{scsz, tdys, jsgl, ppmsc, home, home,(*menu_scsz)},//һ���˵����������
	{tdys, dljs, scsz, tdys1, home, home,(*menu_tdys)},//һ���˵���ͨ��ӳ��
	{dljs, xtsz, tdys, dljs18, home, home,(*menu_dljs)},//һ���˵�����������
	{xtsz, gybj, dljs, cysz, home, home,(*menu_xtsz)},//һ���˵���ϵͳ����
	{gybj, tdwt, xtsz, gybjInf, home, home,(*menu_gybj)},//һ���˵������ڱ���
	
	{tdwt1,tdwt2,tdwt8,tdwt1,tdwt,home,(*menu_tdwt1)},//�����˵���ͨ��΢��1
	{tdwt2,tdwt3,tdwt1,tdwt2,tdwt,home,(*menu_tdwt2)},//�����˵���ͨ��΢��2
	{tdwt3,tdwt4,tdwt2,tdwt3,tdwt,home,(*menu_tdwt3)},//�����˵���ͨ��΢��3
	{tdwt4,tdwt5,tdwt3,tdwt4,tdwt,home,(*menu_tdwt4)},//�����˵���ͨ��΢��4
	{tdwt5,tdwt6,tdwt4,tdwt5,tdwt,home,(*menu_tdwt5)},//�����˵���ͨ��΢��5
	{tdwt6,tdwt7,tdwt5,tdwt6,tdwt,home,(*menu_tdwt6)},//�����˵���ͨ��΢��6
	{tdwt7,tdwt8,tdwt6,tdwt7,tdwt,home,(*menu_tdwt7)},//�����˵���ͨ��΢��7
	{tdwt8,tdwt1,tdwt7,tdwt8,tdwt,home,(*menu_tdwt8)},//�����˵���ͨ��΢��8
	
	{tdzf1,tdzf2,tdzf8,tdzf1,tdzf,home,(*menu_tdzf1)},//�����˵���ͨ������1
	{tdzf2,tdzf3,tdzf1,tdzf2,tdzf,home,(*menu_tdzf2)},//�����˵���ͨ������2
	{tdzf3,tdzf4,tdzf2,tdzf3,tdzf,home,(*menu_tdzf3)},//�����˵���ͨ������3
	{tdzf4,tdzf5,tdzf3,tdzf4,tdzf,home,(*menu_tdzf4)},//�����˵���ͨ������4
	{tdzf5,tdzf6,tdzf4,tdzf5,tdzf,home,(*menu_tdzf5)},//�����˵���ͨ������5
	{tdzf6,tdzf7,tdzf5,tdzf6,tdzf,home,(*menu_tdzf6)},//�����˵���ͨ������6
	{tdzf7,tdzf8,tdzf6,tdzf7,tdzf,home,(*menu_tdzf7)},//�����˵���ͨ������7
	{tdzf8,tdzf1,tdzf7,tdzf8,tdzf,home,(*menu_tdzf8)},//�����˵���ͨ������8
	//��ǰ, ��, ��, ȷ��, ����, home
	{sjyhk,vyhk,jfyhk,sjyhk,hksz,home,(*menu_sjyhk)},//�����˵�����������
	{vyhk,cshk,sjyhk,vyhk,hksz,home,(*menu_vyhk)},//�����˵���V����
	{cshk,jfyhk,vyhk,cshk,hksz,home,(*menu_cshk)},//�����˵������ٻ��
	{jfyhk,sjyhk,cshk,jfyhk,hksz,home,(*menu_jfyhk)},//�����˵���������
	
	{xjjs,xzmx,txmm,xjjs,jsgl,home,(*menu_xjjs)},//�����˵����½�����
	{xzmx,bcmx,xjjs,xzmx,jsgl,home,(*menu_xzmx)},//�����˵���ѡ��ģ��
	{bcmx,txmm,xzmx,bcmx,jsgl,home,(*menu_bcmx)},//�����˵�������ģ��
	{txmm,xjjs,bcmx,txmm,jsgl,home,(*menu_txmm)},//�����˵���ͨ������
	
	{ppmsc,wxfs,fsgl,ppmsc,scsz,home,(*menu_ppmsc)},//�����˵���PPM���
	{wxfs,fsgl,ppmsc,wxfs,scsz,home,(*menu_wxfs)},//�����˵������߷���
	{fsgl,ppmsc,wxfs,fsgl,scsz,home,(*menu_fsgl)},//�����˵������书��
	
	{tdys1,tdys2,tdys8,tdys1,tdys,home,(*menu_tdys1)},//�����˵���ͨ��ӳ��1
	{tdys2,tdys3,tdys1,tdys2,tdys,home,(*menu_tdys2)},//�����˵���ͨ��ӳ��2
	{tdys3,tdys4,tdys2,tdys3,tdys,home,(*menu_tdys3)},//�����˵���ͨ��ӳ��3
	{tdys4,tdys5,tdys3,tdys4,tdys,home,(*menu_tdys4)},//�����˵���ͨ��ӳ��4
	{tdys5,tdys6,tdys4,tdys5,tdys,home,(*menu_tdys5)},//�����˵���ͨ��ӳ��5
	{tdys6,tdys7,tdys5,tdys6,tdys,home,(*menu_tdys6)},//�����˵���ͨ��ӳ��6
	{tdys7,tdys8,tdys6,tdys7,tdys,home,(*menu_tdys7)},//�����˵���ͨ��ӳ��7
	{tdys8,tdys1,tdys7,tdys8,tdys,home,(*menu_tdys8)},//�����˵���ͨ��ӳ��8
	
	{dljs18,dljs18,dljs18,dljs,dljs,home,(*menu_dljs18)},//�����˵�����������1-8
	
	{cysz,dysz,hfcc,wtdw,xtsz,home,(*menu_cysz)},//�����˵�����������
	{dysz,nzsz,cysz,dyjz,xtsz,home,(*menu_dysz)},//�����˵�����ѹ����
	{nzsz,ymsz,dysz,nzbj,xtsz,home,(*menu_nzsz)},//�����˵�����������
	{ymsz,xcjz,nzsz,ymph,xtsz,home,(*menu_ymsz)},//�����˵�����������
	{xcjz,hfcc,ymsz,xcjzTip,xtsz,home,(*menu_xcjz)},//�����˵����г�У׼
	{hfcc,cysz,xcjz,hfccTip,xtsz,home,(*menu_hfcc)},//�����˵����ָ���������
	
	{wtdw,ajyx,kjhm,cysz,cysz,home,(*menu_wtdw)},//�����˵���΢����λ
	{ajyx,kjhm,wtdw,cysz,cysz,home,(*menu_ajyx)},//�����˵���������Ч
	{kjhm,wtdw,ajyx,cysz,cysz,home,(*menu_kjhm)},//�����˵�����������

	{dyjz,bjdy,jsbj,dysz,dysz,home,(*menu_dyjz)},//�����˵�����ѹУ׼
	{bjdy,jsbj,dyjz,dysz,dysz,home,(*menu_bjdy)},//�����˵���������ѹ
	{jsbj,dyjz,bjdy,dysz,dysz,home,(*menu_jsbj)},//�����˵������ձ���

	{nzbj,nzsc,kjzj,nzsz,nzsz,home,(*menu_nzbj)},//�����˵������ӱ���
	{nzsc,kjzj,nzbj,nzsz,nzsz,home,(*menu_nzsc)},//�����˵�������ʱ��
	{kjzj,nzbj,nzsc,nzsz,nzsz,home,(*menu_kjzj)},//�����˵��������Լ�
	//��ǰ, ��, ��, ȷ��, ����, home
	{ymph,ymqx,skbh,ymsz,ymsz,home,(*menu_ymph)},//�����˵�������ƫ��
	{ymqx,skbh,ymph,ymqx,ymsz,home,(*menu_ymqx)},//�����˵�����������
	{skbh,ymph,ymqx,ymsz,ymsz,home,(*menu_skbh)},//�����˵���ʧ�ر���

	{xcjzTip,xcjzTip,xcjzTip,xcjz14,xcjz,home,(*menu_xcjzTip)},//�����˵����г�У׼��ʾ
	{xcjz14,xcjz14,xcjz14,xcjz,xcjz,home,(*menu_xcjz14)},//�����˵����г�У׼1-4

	{hfccTip,hfccTip,hfccTip,hfcg,hfcc,home,(*menu_hfccTip)},//�����˵����ָ�����������ʾ
	{hfcg,hfcg,hfcg,hfcc,hfcc,home,(*menu_hfcg)},//�����˵����ָ��ɹ�

	{gybjInf,gybjInf,gybjInf,gybj,gybj,home,(*menu_gybjInf)},//�����˵������ڱ�����ʾ

};

u8 volatile nowMenuIndex = 0;
u8 volatile lastMenuIndex = 0;
extern unsigned char logo[];
extern unsigned char logoR[];
char batVoltStr[10]={0};
u8 ironTop[] 	= {0X04,0X0C,0X18,0X30,0X60,0XC0,0X60,0X30,0X18,0X0C,0X04};
u8 ironBottom[]	= {0X80,0XC0,0X60,0X30,0X18,0X0C,0X18,0X30,0X60,0XC0,0X80};
u8 ironHand[] = {0X01,0X00,0X01,0X00,0X02,0X80,0X02,0XC0,0X04,0XBC,0X05,0X6C,0X05,0XE8,0X0B,0X90,
				0X0B,0X10,0X0E,0X08,0X1C,0X08,0X1C,0X18,0X18,0X60,0X33,0X80,0X3C,0X00,0X20,0X00,};
u8 ironOn[] = {0X10,0X28,0X44,0X82,0XBA,0XBA,0X82,0XC6,0XAA,0X92,0X82,0X82,0X82,0X82,0X82,0X44,0X28,0X10,};
u8 ironOff[] = {0X10,0X38,0X7C,0XFE,0XFE,0XFE,0XFE,0XFE,0XFE,0XEE,0XC6,0X82,0XBA,0XBA,0X82,0X44,0X28,0X10,};
	
void main2menuList(u8 hz1,u8 hz2,u8 hz3,u8 hz4);
void menuList2tdwt(void);
void menuList2tdzf(void);
void menuList2tdys(void);
	
void OLED_display(void){
	switch(menuEvent[1]){
		case BM_down: //��ʱ����ת������
			nowMenuIndex=Key_table[nowMenuIndex].down_index;
			break;
		case BM_up: //˳ʱ����ת,����
			nowMenuIndex=Key_table[nowMenuIndex].up_index;
			break;
		case KEY_enter://ȷ��
			nowMenuIndex=Key_table[nowMenuIndex].enter_index;
			break;
		case KEY_esc://����
			nowMenuIndex=Key_table[nowMenuIndex].esc_index;
			break;
		case KEY_home://���� ����������
			nowMenuIndex=Key_table[nowMenuIndex].home_index;
			break;
		case NUM_down:
			break;
		case NUM_up:
			break;
	}
//	printf("key:%d, last:%d, now:%d\r\n",menuEvent[1],lastMenuIndex,nowMenuIndex);
	if(nowMenuIndex==home && lastMenuIndex!=home) OLED_Fill(0,0,127,63,0);//���
	if(nowMenuIndex>=tdwt && nowMenuIndex<=gybj && (lastMenuIndex<tdwt | lastMenuIndex>gybj)) main2menuList(sheZhi,zhi,cai,dan);//�л�����
	if(nowMenuIndex>=tdwt1 && nowMenuIndex<=tdwt8 && (lastMenuIndex<tdwt1 | lastMenuIndex>tdwt8))menuList2tdwt();//�л�����
	if(nowMenuIndex>=tdzf1 && nowMenuIndex<=tdzf8 && (lastMenuIndex<tdzf1 | lastMenuIndex>tdzf8))menuList2tdzf();//�л�����
	if(nowMenuIndex>=sjyhk && nowMenuIndex<=jfyhk && (lastMenuIndex<sjyhk | lastMenuIndex>jfyhk))OLED_Fill(0,0,127,63,0);//���
	if(nowMenuIndex>=xjjs && nowMenuIndex<=txmm && (lastMenuIndex<xjjs | lastMenuIndex>txmm))OLED_Fill(0,0,127,63,0);//���
	if(nowMenuIndex>=ppmsc && nowMenuIndex<=fsgl && (lastMenuIndex<ppmsc | lastMenuIndex>fsgl))OLED_Fill(0,0,127,63,0);//���
	if(nowMenuIndex>=tdys1 && nowMenuIndex<=tdys8 && (lastMenuIndex<tdys1 | lastMenuIndex>tdys8))menuList2tdys();//�л�����
	if(nowMenuIndex==dljs18 && lastMenuIndex!=dljs18) OLED_Fill(0,0,127,63,0);//���
	if(nowMenuIndex>=cysz && nowMenuIndex<=hfcc && (lastMenuIndex<cysz | lastMenuIndex>hfcc)) main2menuList(xi,tongYi,sheZhi,zhi);//�л�����
	if(nowMenuIndex>=wtdw && nowMenuIndex<=kjhm && (lastMenuIndex<wtdw | lastMenuIndex>kjhm))OLED_Fill(0,0,127,63,0);//���
	if(nowMenuIndex>=dyjz && nowMenuIndex<=jsbj && (lastMenuIndex<dyjz | lastMenuIndex>jsbj))OLED_Fill(0,0,127,63,0);//���
	if(nowMenuIndex>=nzbj && nowMenuIndex<=kjzj && (lastMenuIndex<nzbj | lastMenuIndex>kjzj))OLED_Fill(0,0,127,63,0);//���
	if(nowMenuIndex>=ymph && nowMenuIndex<=skbh && (lastMenuIndex<ymph | lastMenuIndex>skbh))OLED_Fill(0,0,127,63,0);//���
	if(nowMenuIndex==xcjz14 && lastMenuIndex!=xcjz14) OLED_Fill(0,0,127,63,0);//���
	if(nowMenuIndex!=lastMenuIndex)
	{
		Key_table[nowMenuIndex].operate();
		OLED_Refresh_Gram();//ˢ���Դ�
	}
}


void mainWindow(void)
{
	u16 thrNum;
	int loca;
	//u8 txt[16]={0};//����ַ����ı�
	//�м�ͼ��
	OLED_DrawPointBMP(9,0,logo,110,56,1);//��ʾlogo
	
	sprintf((char *)batVoltStr,"%2.2fV",batVolt);
	OLED_ShowString(90,0, (u8 *)batVoltStr,12,1);//��ʾ��ص�ѹ
	
	
	if(setData.throttlePreference)//��������
	{
		//��1ͨ��΢����ʶ-���-�Һ���
		OLED_Fill(66,59,124,62,0);//д0�����ԭ���ı�־
		loca = (int)95+setData.PWMadjustValue[0]/8;
		OLED_Fill(loca,59,loca,62,1);//д1
		OLED_DrawPlusSign(95,61);//���ı�ʶ
		OLED_Fill(66,63,124,63,1);//д1
		
		//��2ͨ��΢����ʶ-������-������
		OLED_Fill(123,1,126,63,0);//д0
		loca = (int)32-setData.PWMadjustValue[1]/8;
		OLED_Fill(123,loca,126,loca,1);//д1
		OLED_DrawPlusSign(125,32);//���ı�ʶ
		OLED_Fill(127,1,127,63,1);//д1
		
		//��3ͨ�������г���-������
		thrNum = (int)(PWMvalue[2]-1000)/16;
		OLED_Fill(0,63-thrNum,0,63,1);//�²���д1
		OLED_Fill(0,0,0,63-thrNum,0);//�ϲ���д0
		
		//��4ͨ��΢����ʶ-�����-�����
		OLED_Fill(4,59,62,62,0);//д0�����ԭ���ı�־
		loca = (int)33+setData.PWMadjustValue[3]/8;
		OLED_Fill(loca,59,loca,62,1);//д1
		OLED_DrawPlusSign(33,61);//���ı�ʶ
		OLED_Fill(4,63,62,63,1);//д1
	}
	else{//��������
		//��1ͨ��΢����ʶ-���-�����
		OLED_Fill(4,59,62,62,0);//д0�����ԭ���ı�־
		loca = (int)33+setData.PWMadjustValue[0]/8;
		OLED_Fill(loca,59,loca,62,1);//д1
		OLED_DrawPlusSign(33,61);//���ı�ʶ
		OLED_Fill(4,63,62,63,1);//д1
		
		//��2ͨ��΢����ʶ-������-������
		OLED_Fill(1,1,4,63,0);//д0
		loca = (int)32-setData.PWMadjustValue[1]/8;
		OLED_Fill(1,loca,4,loca,1);//д1
		OLED_DrawPlusSign(2,32);//���ı�ʶ
		OLED_Fill(0,1,0,63,1);//д1
		
		//��3ͨ�������г���-������
		thrNum = (int)(PWMvalue[2]-1000)/16;
		OLED_Fill(127,63-thrNum,127,63,1);//�²���д1
		OLED_Fill(127,0,127,63-thrNum,0);//�ϲ���д0
		
		//��4ͨ��΢����ʶ-�����-�Һ���
		OLED_Fill(66,59,124,62,0);//д0�����ԭ���ı�־
		loca = (int)95+setData.PWMadjustValue[3]/8;
		OLED_Fill(loca,59,loca,62,1);//д1
		OLED_DrawPlusSign(95,61);//���ı�ʶ
		OLED_Fill(66,63,124,63,1);//д1
	}
}
//������-һ���˵� �л�
void main2menuList(u8 hz1,u8 hz2,u8 hz3,u8 hz4)
{
	OLED_Fill(0,0,127,15,1);//�������
	OLED_Fill(0,16,127,63,0);//����°벿��
	//�����˵��������ò˵�
	u8 szcdIndex[]={hz1,hz2,hz3,hz4};
	OLED_ShowChineseWords(32,0,szcdIndex,4,0);
	//�����
	OLED_Fill(115,16,115,63,1);//��������������
	OLED_Fill(127,16,127,63,1);//��������������
	OLED_DrawPointBMP(116,16,ironTop,15,6,1);//������ϼ�ͷ
	OLED_DrawPointBMP(116,58,ironBottom,15,6,1);//������¼�ͷ
}
/*
listIndex����������鶨λ,1~8
mode�����е���ʾģʽ
str1~3�������е��ַ���
hzIndex�������е�12�������������е�����
*/
void menuListWindow(u8 listIndex,u8 mode[],u8 *str1,u8 *str2,u8 *str3,u8 hzIndex[],u8 len)
{	
	//��ͷ
	OLED_Fill(0,16,16,63,0);//��ͷ���
	OLED_DrawPointBMP(0,16+16*((listIndex-1)%3),ironHand,16,16,1);//ָʾ��ͷ
	//��һ��
	OLED_ShowString(16,16,str1,16,mode[0]);
	u8 line1Index[]={hzIndex[0],hzIndex[1],hzIndex[2],hzIndex[3]};
	OLED_ShowChineseWords(32,16,line1Index,4,mode[0]);
	//�ڶ���
	OLED_ShowString(16,32,str2,16,mode[1]);
	u8 line2Index[]={hzIndex[4],hzIndex[5],hzIndex[6],hzIndex[7]};
	OLED_ShowChineseWords(32,32,line2Index,4,mode[1]);
	//������
	OLED_ShowString(16,48,str3,16,mode[2]);
	u8 line3Index[]={hzIndex[8],hzIndex[9],hzIndex[10],hzIndex[11]};
	OLED_ShowChineseWords(32,48,line3Index,4,mode[2]);
	//�����
	OLED_Fill(116,22,126,57,0);//��������
	OLED_Fill(116,(u8)19+32*listIndex/len,126,(u8)24+32*listIndex/len,1);//������������
}
void menu_tdwt(void){
	u8 modes[3]={0,1,1};
	u8 str1[]="1.";
	u8 str2[]="2.";
	u8 str3[]="3.";
	u8 hzIndex[12]={tongDao,dao,weiTiao,tiao, tongDao,dao,zheng,fan, hun,kong,sheZhi,zhi};
	menuListWindow(1,modes,str1,str2,str3,hzIndex,9);
}//һ���˵���1.ͨ��΢��

void menu_tdzf(void){
	u8 modes[3]={1,0,1};
	u8 str1[]="1.";
	u8 str2[]="2.";
	u8 str3[]="3.";
	u8 hzIndex[12]={tongDao,dao,weiTiao,tiao, tongDao,dao,zheng,fan, hun,kong,sheZhi,zhi};
	menuListWindow(2,modes,str1,str2,str3,hzIndex,9);
}//һ���˵���2.ͨ������
void menu_hksz(void){
	u8 modes[3]={1,1,0};
	u8 str1[]="1.";
	u8 str2[]="2.";
	u8 str3[]="3.";
	u8 hzIndex[12]={tongDao,dao,weiTiao,tiao, tongDao,dao,zheng,fan, hun,kong,sheZhi,zhi};
	menuListWindow(3,modes,str1,str2,str3,hzIndex,9);
}//һ���˵���3.�������
void menu_jsgl(void){
	u8 modes[3]={0,1,1};
	u8 str1[]="4.";
	u8 str2[]="5.";
	u8 str3[]="6.";
	u8 hzIndex[12]={jie,shouDao,guanLi,li,shu,chu,sheZhi,zhi,tongDao,dao,ying,sheMen};
	menuListWindow(4,modes,str1,str2,str3,hzIndex,9);
}//һ���˵���4.���չ���
void menu_scsz(void){
	u8 modes[3]={1,0,1};
	u8 str1[]="4.";
	u8 str2[]="5.";
	u8 str3[]="6.";
	u8 hzIndex[12]={jie,shouDao,guanLi,li,shu,chu,sheZhi,zhi,tongDao,dao,ying,sheMen};
	menuListWindow(5,modes,str1,str2,str3,hzIndex,9);
}//һ���˵���5.�������
void menu_tdys(void){
	u8 modes[3]={1,1,0};
	u8 str1[]="4.";
	u8 str2[]="5.";
	u8 str3[]="6.";
	u8 hzIndex[12]={jie,shouDao,guanLi,li,shu,chu,sheZhi,zhi,tongDao,dao,ying,sheMen};
	menuListWindow(6,modes,str1,str2,str3,hzIndex,9);
}//һ���˵���6.ͨ��ӳ��
void menu_dljs(void){
	u8 modes[3]={0,1,1};
	u8 str1[]="7.";
	u8 str2[]="8.";
	u8 str3[]="9.";
	u8 hzIndex[12]={duo,liang,jianShi,shiYe,xi,tongYi,sheZhi,zhi,guanMen,yu,ben,jiQi};
	menuListWindow(7,modes,str1,str2,str3,hzIndex,9);
}//һ���˵���7.��������
void menu_xtsz(void){
	u8 modes[3]={1,0,1};
	u8 str1[]="7.";
	u8 str2[]="8.";
	u8 str3[]="9.";
	u8 hzIndex[12]={duo,liang,jianShi,shiYe,xi,tongYi,sheZhi,zhi,guanMen,yu,ben,jiQi};
	menuListWindow(8,modes,str1,str2,str3,hzIndex,9);
}//һ���˵���8.ϵͳ����
void menu_gybj(void){
	u8 modes[3]={1,1,0};
	u8 str1[]="7.";
	u8 str2[]="8.";
	u8 str3[]="9.";
	u8 hzIndex[12]={duo,liang,jianShi,shiYe,xi,tongYi,sheZhi,zhi,guanMen,yu,ben,jiQi};
	menuListWindow(9,modes,str1,str2,str3,hzIndex,9);
}//һ���˵���9.���ڱ���

//һ���˵�-�����˵� ͨ��΢������ �л�
void menuList2tdwt(void)
{
	OLED_Fill(0,0,127,63,0);//���
	OLED_Fill(63,0,64,63,1);//�м�����
}
/*mode[]������CH����ʾģʽ����*/
void tdwtWindow(u8 mode[])
{
	OLED_ShowString(1,0, (u8 *)"CH1",16,mode[0]);
	OLED_ShowNum(30,0,abs(setData.PWMadjustValue[0]),3,16);
	if(setData.PWMadjustValue[0]<0) OLED_ShowString(54,0, (u8 *)"-",16,1);
	else OLED_ShowString(54,0, (u8 *)"+",16,1);
	
	OLED_ShowString(1,16, (u8 *)"CH2",16,mode[1]);
	OLED_ShowNum(30,16,abs(setData.PWMadjustValue[1]),3,16);
	if(setData.PWMadjustValue[1]<0) OLED_ShowString(54,16, (u8 *)"-",16,1);
	else OLED_ShowString(54,16, (u8 *)"+",16,1);
	
	OLED_ShowString(1,32, (u8 *)"CH3",16,mode[2]);
	OLED_ShowNum(30,32,abs(setData.PWMadjustValue[2]),3,16);
	if(setData.PWMadjustValue[2]<0) OLED_ShowString(54,32, (u8 *)"-",16,1);
	else OLED_ShowString(54,32, (u8 *)"+",16,1);
	
	OLED_ShowString(1,48, (u8 *)"CH4",16,mode[3]);
	OLED_ShowNum(30,48,abs(setData.PWMadjustValue[3]),3,16);
	if(setData.PWMadjustValue[3]<0) OLED_ShowString(54,48, (u8 *)"-",16,1);
	else OLED_ShowString(54,48, (u8 *)"+",16,1);
	
	OLED_ShowString(66,0, (u8 *)"CH5",16,mode[4]);
	OLED_ShowNum(95,0,abs(setData.PWMadjustValue[4]),3,16);
	if(setData.PWMadjustValue[4]<0) OLED_ShowString(119,0, (u8 *)"-",16,1);
	else OLED_ShowString(119,0, (u8 *)"+",16,1);
	
	OLED_ShowString(66,16, (u8 *)"CH6",16,mode[5]);
	OLED_ShowNum(95,16,abs(setData.PWMadjustValue[5]),3,16);
	if(setData.PWMadjustValue[5]<0) OLED_ShowString(119,16, (u8 *)"-",16,1);
	else OLED_ShowString(119,16, (u8 *)"+",16,1);
	
	OLED_ShowString(66,32, (u8 *)"CH7",16,mode[6]);
	OLED_ShowNum(95,32,abs(setData.PWMadjustValue[6]),3,16);
	if(setData.PWMadjustValue[6]<0) OLED_ShowString(119,32, (u8 *)"-",16,1);
	else OLED_ShowString(119,32, (u8 *)"+",16,1);
	
	OLED_ShowString(66,48, (u8 *)"CH8",16,mode[7]);
	OLED_ShowNum(95,48,abs(setData.PWMadjustValue[7]),3,16);
	if(setData.PWMadjustValue[7]<0) OLED_ShowString(119,48, (u8 *)"-",16,1);
	else OLED_ShowString(119,48, (u8 *)"+",16,1);
}
void menu_tdwt1(void){
	u8 modes[8]={0,1,1,1,1,1,1,1};
	tdwtWindow(modes);
}//�����˵���ͨ��1
void menu_tdwt2(void){
	u8 modes[8]={1,0,1,1,1,1,1,1};
	tdwtWindow(modes);
}//�����˵���ͨ��2
void menu_tdwt3(void){
	u8 modes[8]={1,1,0,1,1,1,1,1};
	tdwtWindow(modes);
}//�����˵���ͨ��3
void menu_tdwt4(void){
	u8 modes[8]={1,1,1,0,1,1,1,1};
	tdwtWindow(modes);
}//�����˵���ͨ��4
void menu_tdwt5(void){
	u8 modes[8]={1,1,1,1,0,1,1,1};
	tdwtWindow(modes);
}//�����˵���ͨ��5
void menu_tdwt6(void){
	u8 modes[8]={1,1,1,1,1,0,1,1};
	tdwtWindow(modes);
}//�����˵���ͨ��6
void menu_tdwt7(void){
	u8 modes[8]={1,1,1,1,1,1,0,1};
	tdwtWindow(modes);
}//�����˵���ͨ��7
void menu_tdwt8(void){
	u8 modes[8]={1,1,1,1,1,1,1,0};
	tdwtWindow(modes);
}//�����˵���ͨ��8

//һ���˵�-�����˵� ͨ��΢������ �л�
void menuList2tdzf(void)
{
	OLED_Fill(0,0,127,63,0);//���
	OLED_Fill(63,0,64,63,1);//�м�����
}
/*mode[]������CH����ʾģʽ����*/
void tdzfWindow(u8 mode[])
{
	OLED_ShowString(1,0, (u8 *)"CH1",16,mode[0]);
	if(setData.chReverse[0])OLED_DrawPointBMP(30,4,ironOn,21,7,1);//��
	else OLED_DrawPointBMP(30,4,ironOff,21,7,1);//��
	
	OLED_ShowString(1,16, (u8 *)"CH2",16,mode[1]);
	if(setData.chReverse[1])OLED_DrawPointBMP(30,20,ironOn,21,7,1);//��
	else OLED_DrawPointBMP(30,20,ironOff,21,7,1);//��
	
	OLED_ShowString(1,32, (u8 *)"CH3",16,mode[2]);
	if(setData.chReverse[2])OLED_DrawPointBMP(30,36,ironOn,21,7,1);//��
	else OLED_DrawPointBMP(30,36,ironOff,21,7,1);//��
	
	OLED_ShowString(1,48, (u8 *)"CH4",16,mode[3]);
	if(setData.chReverse[3])OLED_DrawPointBMP(30,52,ironOn,21,7,1);//��
	else OLED_DrawPointBMP(30,52,ironOff,21,7,1);//��
	
	OLED_ShowString(66,0, (u8 *)"CH5",16,mode[4]);
	if(setData.chReverse[4])OLED_DrawPointBMP(94,4,ironOn,21,7,1);//��
	else OLED_DrawPointBMP(94,4,ironOff,21,7,1);//��
	
	OLED_ShowString(66,16, (u8 *)"CH6",16,mode[5]);
	if(setData.chReverse[5])OLED_DrawPointBMP(94,20,ironOn,21,7,1);//��
	else OLED_DrawPointBMP(94,20,ironOff,21,7,1);//��
	
	OLED_ShowString(66,32, (u8 *)"CH7",16,mode[6]);
	if(setData.chReverse[6])OLED_DrawPointBMP(94,36,ironOn,21,7,1);//��
	else OLED_DrawPointBMP(94,36,ironOff,21,7,1);//��
	
	OLED_ShowString(66,48, (u8 *)"CH8",16,mode[7]);
	if(setData.chReverse[7])OLED_DrawPointBMP(94,52,ironOn,21,7,1);//��
	else OLED_DrawPointBMP(94,52,ironOff,21,7,1);//��
}
void menu_tdzf1(void){
	u8 modes[8]={0,1,1,1,1,1,1,1};
	tdzfWindow(modes);
}//�����˵���ͨ��1
void menu_tdzf2(void){
	u8 modes[8]={1,0,1,1,1,1,1,1};
	tdzfWindow(modes);
}//�����˵���ͨ��2
void menu_tdzf3(void){
	u8 modes[8]={1,1,0,1,1,1,1,1};
	tdzfWindow(modes);
}//�����˵���ͨ��3
void menu_tdzf4(void){
	u8 modes[8]={1,1,1,0,1,1,1,1};
	tdzfWindow(modes);
}//�����˵���ͨ��4
void menu_tdzf5(void){
	u8 modes[8]={1,1,1,1,0,1,1,1};
	tdzfWindow(modes);
}//�����˵���ͨ��5
void menu_tdzf6(void){
	u8 modes[8]={1,1,1,1,1,0,1,1};
	tdzfWindow(modes);
}//�����˵���ͨ��6
void menu_tdzf7(void){
	u8 modes[8]={1,1,1,1,1,1,0,1};
	tdzfWindow(modes);
}//�����˵���ͨ��7
void menu_tdzf8(void){
	u8 modes[8]={1,1,1,1,1,1,1,0};
	tdzfWindow(modes);
}//�����˵���ͨ��8
/*
listIndex��0-3
mode�����е���ʾģʽ
*/
void hkszWindow(u8 listIndex,u8 mode[])
{
	//��ͷ
	OLED_Fill(0,0,16,63,0);//��ͷ���
	OLED_DrawPointBMP(0,16*listIndex,ironHand,16,16,1);//ָʾ��ͷ
	//��һ��
	OLED_ShowString(16,0,(u8 *)"1.",16,mode[0]);
	u8 line1Index[]={san,jiaoDu,yi,hun,kong};
	OLED_ShowChineseWords(32,0,line1Index,5,mode[0]);
	//�ڶ���
	OLED_ShowString(16,16,(u8 *)"2.",16,mode[1]);
	OLED_ShowString(32,16,(u8 *)"V ",16,mode[1]);
	u8 line2Index[]={yi,hun,kong};
	OLED_ShowChineseWords(48,16,line2Index,3,mode[1]);
	//������
	OLED_ShowString(16,32,(u8 *)"3.",16,mode[2]);
	u8 line3Index[]={cha,su,hun,kong};
	OLED_ShowChineseWords(32,32,line3Index,4,mode[2]);
	//������
	OLED_ShowString(16,48,(u8 *)"4.",16,mode[3]);
	u8 line4Index[]={jin,fuYi,yi,hun,kong};
	OLED_ShowChineseWords(32,48,line4Index,5,mode[3]);
}
void menu_sjyhk(void){
	u8 modes[4]={0,1,1,1};
	hkszWindow(0,modes);
}//�����˵�����������
void menu_vyhk(void){
	u8 modes[4]={1,0,1,1};
	hkszWindow(1,modes);
}//�����˵���V����
void menu_cshk(void){
	u8 modes[4]={1,1,0,1};
	hkszWindow(2,modes);
}//�����˵������ٻ��
void menu_jfyhk(void){
	u8 modes[4]={1,1,1,0};
	hkszWindow(3,modes);
}//�����˵���������
/*
listIndex��0-3
mode�����е���ʾģʽ
*/
void jsglWindow(u8 listIndex,u8 mode[])
{
	//��һ��
	OLED_ShowString(0,0,(u8 *)"1.",16,mode[0]);
	u8 line1Index[]={xinJian,jianShe,jie,shouDao};
	OLED_ShowChineseWords(16,0,line1Index,4,mode[0]);
	//�ڶ���
	OLED_ShowString(0,16,(u8 *)"2.",16,mode[1]);
	u8 line2Index[]={xuan,ze,moXing,xingHao};
	OLED_ShowChineseWords(16,16,line2Index,4,mode[1]);
	//������
	OLED_ShowString(0,32,(u8 *)"3.",16,mode[2]);
	u8 line3Index[]={baoCun,cun,moXing,xingHao};
	OLED_ShowChineseWords(16,32,line3Index,4,mode[2]);
	//������
	OLED_ShowString(0,48,(u8 *)"4.",16,mode[3]);
	u8 line4Index[]={tongDao,xinXin,mi,ma};
	OLED_ShowChineseWords(16,48,line4Index,4,mode[3]);
}
void menu_xjjs(void){
	u8 modes[4]={0,1,1,1};
	jsglWindow(0,modes);
}//�����˵����½�����
void menu_xzmx(void){
	u8 modes[4]={1,0,1,1};
	jsglWindow(1,modes);
}//�����˵���ѡ��ģ��
void menu_bcmx(void){
	u8 modes[4]={1,1,0,1};
	jsglWindow(2,modes);
}//�����˵�������ģ��
void menu_txmm(void){
	u8 modes[4]={1,1,1,0};
	jsglWindow(3,modes);
}//�����˵���ͨ������
/*
�������
listIndex��0-3
mode�����е���ʾģʽ
*/
void scszWindow(u8 listIndex,u8 mode[])
{
	//��һ��
	OLED_Fill(0,0,31,15,1);//�������
	u8 line1Index[]={shu,chu,sheZhi,zhi};
	OLED_ShowChineseWords(32,0,line1Index,4,0);
	OLED_Fill(96,0,127,15,1);//�������
	//�ڶ���
	OLED_ShowString(0,16,(u8 *)"1.PPM",16,mode[0]);
	u8 line2Index[]={shu,chu};
	OLED_ShowChineseWords(40,16,line2Index,2,mode[0]);
	//������
	OLED_ShowString(0,32,(u8 *)"2.",16,mode[1]);
	u8 line3Index[]={wu,xian,fa,sheMen};
	OLED_ShowChineseWords(16,32,line3Index,4,mode[1]);
	//������
	OLED_ShowString(0,48,(u8 *)"3.",16,mode[2]);
	u8 line4Index[]={fa,sheMen,gong,lv};
	OLED_ShowChineseWords(16,48,line4Index,4,mode[2]);
}
void menu_ppmsc(void){
	u8 modes[3]={0,1,1};
	scszWindow(0,modes);
}//�����˵���PPM���
void menu_wxfs(void){
	u8 modes[3]={1,0,1};
	scszWindow(1,modes);
}//�����˵������߷���
void menu_fsgl(void){
	u8 modes[3]={1,1,0};
	scszWindow(2,modes);
}//�����˵������书��

//һ���˵�-�����˵� ͨ��ӳ�䴰�� �л�
void menuList2tdys(void)
{
	OLED_Fill(0,0,127,63,0);//���
	OLED_Fill(63,0,64,63,1);//�м�����
}

/*mode[]������CH����ʾģʽ����*/
void tdysWindow(u8 mode[])
{
	OLED_ShowString(1,0, (u8 *)"CH1",16,mode[0]);
	OLED_ShowString(40,0, (u8 *)"Y1",16,1);//�˴�Ӧ��ȡsetData,��if�ж���ʾ
	
	OLED_ShowString(1,16, (u8 *)"CH2",16,mode[1]);
	OLED_ShowString(40,16, (u8 *)"Y2",16,1);
	
	OLED_ShowString(1,32, (u8 *)"CH3",16,mode[2]);
	OLED_ShowString(40,32, (u8 *)"Y3",16,1);
	
	OLED_ShowString(1,48, (u8 *)"CH4",16,mode[3]);
	OLED_ShowString(40,48, (u8 *)"Y4",16,1);
	
	OLED_ShowString(66,0, (u8 *)"CH5",16,mode[4]);
	OLED_ShowString(105,0, (u8 *)"K1",16,1);
	
	OLED_ShowString(66,16, (u8 *)"CH6",16,mode[5]);
	OLED_ShowString(105,16, (u8 *)"K2",16,1);
	
	OLED_ShowString(66,32, (u8 *)"CH7",16,mode[6]);
	OLED_ShowString(105,32, (u8 *)"K3",16,1);
	
	OLED_ShowString(66,48, (u8 *)"CH8",16,mode[7]);
	OLED_ShowString(105,48, (u8 *)"K4",16,1);
}
void menu_tdys1(void){
	u8 modes[8]={0,1,1,1,1,1,1,1};
	tdysWindow(modes);
}//�����˵���ͨ��1
void menu_tdys2(void){
	u8 modes[8]={1,0,1,1,1,1,1,1};
	tdysWindow(modes);
}//�����˵���ͨ��2
void menu_tdys3(void){
	u8 modes[8]={1,1,0,1,1,1,1,1};
	tdysWindow(modes);
}//�����˵���ͨ��3
void menu_tdys4(void){
	u8 modes[8]={1,1,1,0,1,1,1,1};
	tdysWindow(modes);
}//�����˵���ͨ��4
void menu_tdys5(void){
	u8 modes[8]={1,1,1,1,0,1,1,1};
	tdysWindow(modes);
}//�����˵���ͨ��5
void menu_tdys6(void){
	u8 modes[8]={1,1,1,1,1,0,1,1};
	tdysWindow(modes);
}//�����˵���ͨ��6
void menu_tdys7(void){
	u8 modes[8]={1,1,1,1,1,1,0,1};
	tdysWindow(modes);
}//�����˵���ͨ��7
void menu_tdys8(void){
	u8 modes[8]={1,1,1,1,1,1,1,0};
	tdysWindow(modes);
}//�����˵���ͨ��8
u8 chLineLen[8];
void menu_dljs18(void){
	u8 num,temp;
	for(num=0;num<8;num++)
	{
		temp = map(PWMvalue[num],1000+setData.PWMadjustValue[num],2000+setData.PWMadjustValue[num],0,63);
		if(nowMenuIndex!=lastMenuIndex|chLineLen[num]!=temp){
			OLED_Fill(16*num+4,63-temp,16*num+12,63,1);//�²���д1
			OLED_Fill(16*num+4,0,16*num+12,63-temp,0);//�ϲ���д0
			chLineLen[num]=temp;
		}
	}
}//�����˵���ͨ��1-8��������

void menu_cysz(void){
	u8 modes[3]={0,1,1};
	u8 str1[]="1.";
	u8 str2[]="2.";
	u8 str3[]="3.";
	u8 hzIndex[12]={changYong,yong,sheZhi,zhi,dian,ya,sheZhi,zhi,nao,zhongDian,sheZhi,zhi};
	menuListWindow(1,modes,str1,str2,str3,hzIndex,6);
}//�����˵�����������
void menu_dysz(void){
	u8 modes[3]={1,0,1};
	u8 str1[]="1.";
	u8 str2[]="2.";
	u8 str3[]="3.";
	u8 hzIndex[12]={changYong,yong,sheZhi,zhi,dian,ya,sheZhi,zhi,nao,zhongDian,sheZhi,zhi};
	menuListWindow(2,modes,str1,str2,str3,hzIndex,6);
}//�����˵�����ѹ����
void menu_nzsz(void){
	u8 modes[3]={1,1,0};
	u8 str1[]="1.";
	u8 str2[]="2.";
	u8 str3[]="3.";
	u8 hzIndex[12]={changYong,yong,sheZhi,zhi,dian,ya,sheZhi,zhi,nao,zhongDian,sheZhi,zhi};
	menuListWindow(3,modes,str1,str2,str3,hzIndex,6);
}//�����˵�����������
void menu_ymsz(void){
	u8 modes[3]={0,1,1};
	u8 str1[]="4.";
	u8 str2[]="5.";
	u8 str3[]="6.";
	u8 hzIndex[12]={youMen,men,sheZhi,zhi,xingCheng,chengDu,jiaoZhun,zhun,huiFu,fuXing,moRen,ren};
	menuListWindow(4,modes,str1,str2,str3,hzIndex,6);
}//�����˵�����������
void menu_xcjz(void){
	u8 modes[3]={1,0,1};
	u8 str1[]="4.";
	u8 str2[]="5.";
	u8 str3[]="6.";
	u8 hzIndex[12]={youMen,men,sheZhi,zhi,xingCheng,chengDu,jiaoZhun,zhun,huiFu,fuXing,moRen,ren};
	menuListWindow(5,modes,str1,str2,str3,hzIndex,6);
}//�����˵����г�У׼
void menu_hfcc(void){
	u8 modes[3]={1,1,0};
	u8 str1[]="4.";
	u8 str2[]="5.";
	u8 str3[]="6.";
	u8 hzIndex[12]={youMen,men,sheZhi,zhi,xingCheng,chengDu,jiaoZhun,zhun,huiFu,fuXing,moRen,ren};
	menuListWindow(6,modes,str1,str2,str3,hzIndex,6);
}//�����˵����ָ���������
/*
hzIndex��16�������������е�����
mode�����е���ʾģʽ
*/
void line3Window(u8 hzIndex[],u8 mode[])
{
	//��һ��
	OLED_Fill(0,0,31,15,1);//�������
	u8 line1Index[]={hzIndex[0],hzIndex[1],hzIndex[2],hzIndex[3]};
	OLED_ShowChineseWords(32,0,line1Index,4,0);
	OLED_Fill(96,0,127,15,1);//�������
	//�ڶ���
	OLED_ShowString(0,16,(u8 *)"1.",16,mode[0]);
	u8 line2Index[]={hzIndex[4],hzIndex[5],hzIndex[6],hzIndex[7]};
	OLED_ShowChineseWords(16,16,line2Index,4,mode[0]);
	//������
	OLED_ShowString(0,32,(u8 *)"2.",16,mode[1]);
	u8 line3Index[]={hzIndex[8],hzIndex[9],hzIndex[10],hzIndex[11]};
	OLED_ShowChineseWords(16,32,line3Index,4,mode[1]);
	//������
	OLED_ShowString(0,48,(u8 *)"3.",16,mode[2]);
	u8 line4Index[]={hzIndex[12],hzIndex[13],hzIndex[14],hzIndex[15]};
	OLED_ShowChineseWords(16,48,line4Index,4,mode[2]);
}

void menu_wtdw(void){
	u8 hzIndex[16]={changYong,yong,sheZhi,zhi,weiTiao,tiao,dan,weiZhi,an,jianPan,yin,xiao,kai,jiQi,hua,mian};
	u8 modes[3] = {0,1,1};
	line3Window(hzIndex, modes);
	OLED_ShowNum(88,16,setData.PWMadjustUnit,1,16);
}//�����˵���΢����λ
void menu_ajyx(void){
	u8 hzIndex[16]={changYong,yong,sheZhi,zhi,weiTiao,tiao,dan,weiZhi,an,jianPan,yin,xiao,kai,jiQi,hua,mian};
	u8 modes[3] = {1,0,1};
	line3Window(hzIndex, modes);
}//�����˵���������Ч
void menu_kjhm(void){
	u8 hzIndex[16]={changYong,yong,sheZhi,zhi,weiTiao,tiao,dan,weiZhi,an,jianPan,yin,xiao,kai,jiQi,hua,mian};
	u8 modes[3] = {1,1,0};
	line3Window(hzIndex, modes);
}//�����˵�����������

void menu_dyjz(void){
	u8 hzIndex[16]={dian,ya,sheZhi,zhi,dian,ya,jiaoZhun,zhun,baoJing,jing,dian,ya,jie,shouDao,baoJing,jing};
	u8 modes[3] = {0,1,1};
	line3Window(hzIndex, modes);
	sprintf((char *)batVoltStr,"%2.2fV",batVolt);
	OLED_ShowString(88,16, (u8 *)batVoltStr,16,1);
	sprintf((char *)batVoltStr,"%2.2fV",setData.warnBatVolt);
	OLED_ShowString(88,32, (u8 *)batVoltStr,16,1);
	sprintf((char *)batVoltStr,"%2.2fV",setData.warnBatVolt);//�ɻ��ĵ�ر���
	OLED_ShowString(88,48, (u8 *)batVoltStr,16,1);
}//�����˵�����ѹУ׼
void menu_bjdy(void){
	u8 hzIndex[16]={dian,ya,sheZhi,zhi,dian,ya,jiaoZhun,zhun,baoJing,jing,dian,ya,jie,shouDao,baoJing,jing};
	u8 modes[3] = {1,0,1};
	line3Window(hzIndex, modes);
	sprintf((char *)batVoltStr,"%2.2fV",setData.warnBatVolt);
	OLED_ShowString(88,32, (u8 *)batVoltStr,16,1);
}//�����˵���������ѹ
void menu_jsbj(void){
	u8 hzIndex[16]={dian,ya,sheZhi,zhi,dian,ya,jiaoZhun,zhun,baoJing,jing,dian,ya,jie,shouDao,baoJing,jing};
	u8 modes[3] = {1,1,0};
	line3Window(hzIndex, modes);
	sprintf((char *)batVoltStr,"%2.2fV",setData.warnBatVolt);//�ɻ��ĵ�ر���
	OLED_ShowString(88,48, (u8 *)batVoltStr,16,1);
}//�����˵������ձ���
	
void menu_nzbj(void){
	u8 hzIndex[16]={nao,zhongDian,sheZhi,zhi,nao,zhongDian,baoJing,jing,nao,zhongDian,shiChang,changDu,kai,jiQi,zi,jianCha};
	u8 modes[3] = {0,1,1};
	line3Window(hzIndex, modes);
}//�����˵������ӱ���
void menu_nzsc(void){
	u8 hzIndex[16]={nao,zhongDian,sheZhi,zhi,nao,zhongDian,baoJing,jing,nao,zhongDian,shiChang,changDu,kai,jiQi,zi,jianCha};
	u8 modes[3] = {1,0,1};
	line3Window(hzIndex, modes);
}//�����˵�������ʱ��
void menu_kjzj(void){
	u8 hzIndex[16]={nao,zhongDian,sheZhi,zhi,nao,zhongDian,baoJing,jing,nao,zhongDian,shiChang,changDu,kai,jiQi,zi,jianCha};
	u8 modes[3] = {1,1,0};
	line3Window(hzIndex, modes);
}//�����˵��������Լ�
	
void menu_ymph(void){
	u8 hzIndex[16]={youMen,men,sheZhi,zhi,youMen,men,pian,hao,youMen,men,qu,xian,shiQu,kong,baoCun,hu};
	u8 modes[3] = {0,1,1};
	line3Window(hzIndex, modes);
	if(setData.throttlePreference)OLED_ShowChinese(88,16,zuo,16,1);//��
	else OLED_ShowChinese(88,16,youShou,16,1);//��
	OLED_ShowChinese(104,16,shouJi,16,1);//��
}//�����˵�������ƫ��
void menu_ymqx(void){
	u8 hzIndex[16]={youMen,men,sheZhi,zhi,youMen,men,pian,hao,youMen,men,qu,xian,shiQu,kong,baoCun,hu};
	u8 modes[3] = {1,0,1};
	line3Window(hzIndex, modes);
}//�����˵�����������
void menu_skbh(void){
	u8 hzIndex[16]={youMen,men,sheZhi,zhi,youMen,men,pian,hao,youMen,men,qu,xian,shiQu,kong,baoCun,hu};
	u8 modes[3] = {1,1,0};
	line3Window(hzIndex, modes);
}//�����˵���ʧ�ر���

void menu_xcjzTip(void){
	OLED_Fill(0,0,127,63,0);//���
	OLED_ShowString(0,0, (u8 *)"1.",16,1);
	u8 qjyghzIndex[] = {qing,jiang,yao,gan,huiLai,zhongXin};
	OLED_ShowChineseWords(16,0,qjyghzIndex,6,1);
	OLED_ShowString(112,0, (u8 *)",",16,1);
	u8 bkymtdIndex[]={baoKuo,kuo,youMen,men,tongDao,dao};
	OLED_ShowChineseWords(16,16,bkymtdIndex,6,1);
	OLED_ShowString(0,32, (u8 *)"2.",16,1);
	u8 qjjsjddIndex[]={qing,jiang,jie,shouDao,jiQi,duan,dian};
	OLED_ShowChineseWords(16,32,qjjsjddIndex,7,1);
	OLED_ShowChinese(0,48,an,16,1);
	OLED_ShowString(16,48, (u8 *)"Enter",16,1);
	u8 jxIndex[]={jiXu,xu};
	OLED_ShowChineseWords(56,48,jxIndex,2,1);
}//�����˵����뽫ң�˻���
void menu_xcjz14(void){
	OLED_ShowString(2,0, (u8 *)"1:",16,1);
	OLED_ShowNum(18,0,setData.chLower[0],4,16);
	OLED_ShowNum(56,0,setData.chMiddle[0],4,16);
	OLED_ShowNum(95,0,setData.chUpper[0],4,16);
	
	OLED_ShowString(2,16, (u8 *)"2:",16,1);
	OLED_ShowNum(18,16,setData.chLower[1],4,16);
	OLED_ShowNum(56,16,setData.chMiddle[1],4,16);
	OLED_ShowNum(95,16,setData.chUpper[1],4,16);
	
	OLED_ShowString(2,32, (u8 *)"3:",16,1);
	OLED_ShowNum(18,32,setData.chLower[2],4,16);
	OLED_ShowNum(56,32,setData.chMiddle[2],4,16);
	OLED_ShowNum(95,32,setData.chUpper[2],4,16);
	
	OLED_ShowString(2,48, (u8 *)"4:",16,1);
	OLED_ShowNum(18,48,setData.chLower[3],4,16);
	OLED_ShowNum(56,48,setData.chMiddle[3],4,16);
	OLED_ShowNum(95,48,setData.chUpper[3],4,16);
}//�����˵���ͨ��1-4��ʾ

void menu_hfccTip(void){
	OLED_Fill(0,0,127,63,0);//���
	u8 jhfmrszIndex[]={jiang,huiFu,fuXing,moRen,ren,sheZhi,zhi};
	OLED_ShowChineseWords(0,16,jhfmrszIndex,7,1);
	OLED_ShowChinese(0,32,an,16,1);
	OLED_ShowString(16,32, (u8 *)"Enter",16,1);
	u8 jxIndex[]={jiXu,xu};
	OLED_ShowChineseWords(56,32,jxIndex,2,1);
}//�����˵������ָ�Ĭ��
void menu_hfcg(void){
	OLED_Fill(0,0,127,63,0);//���
	u8 hfcgIndex[]={huiFu,fuXing,chengGong,gong};
	OLED_ShowChineseWords(32,24,hfcgIndex,4,1);
}//�����˵����ָ��ɹ�

void menu_gybjInf(void){
	OLED_Fill(0,0,127,63,0);//���
	OLED_ShowString(0,0, (u8 *)"J20RC",16,1);
	u8 hfcgIndex[]={kai,fa,tuan,dui};
	OLED_ShowChineseWords(40,0,hfcgIndex,4,1);
	OLED_ShowString(0,16, (u8 *)"2020.10.01",16,1);
	OLED_ShowString(0,32, (u8 *)"V2.0",16,1);
}//�����˵���J20RC�����Ŷ�-V2.0
