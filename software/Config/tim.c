#include "main.h"
#include "tim.h"
#include "sys.h"

volatile u32 nowTime = 0;//��������ʱ�䣬��λ10us
extern char timeStr[9];//ʱ���ַ���

void SysTick_Handler(void)
{
	nowTime++;
}
void drawClockTime(void)
{
	u16 hour=0,min=0,sec=0;
	sec = nowTime/3.2;//ϵͳ��ʱ
	hour = sec/3600;
	min = (sec%3600)/60;
	sec = sec%60;
	sprintf((char *)timeStr,"%02d",hour);//���ʱ���ַ���
	OLED_ShowString(86,1,(u8*)timeStr,12,0);
	sprintf((char *)timeStr,"%02d",min);//���ʱ���ַ���
	OLED_ShowString(101,1,(u8*)timeStr,12,0);
	sprintf((char *)timeStr,"%02d",sec);//���ʱ���ַ���
	OLED_ShowString(116,1,(u8*)timeStr,12,0);
}

