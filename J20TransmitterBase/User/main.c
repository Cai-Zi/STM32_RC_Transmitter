/*
=============J20��ģң����ң������-������V1.0==============
	�����壺STM32F103C8T6��ɫ��
	NRF24L01ģ�飺
				GND   	��Դ��
				VCC		��3.3V��Դ
				CSN		��PB12
				SCK		��PB13
				MISO	��PB14
				MOSI	��PB15
				CE		��PA8
				IRQ		��PA9
	ADC������PA0-7
	��ص�ѹ��⣺PB0
	��������PA10
	6��������
				CH1Left ��PB5	��home��
				CH1Right��PB4
				CH2Up	��PA15	�����ء�
				CH2Down	��PB3	��ȷ����
				CH4Left	��PA12	����ֵ-��
				CH4Right��PA11	����ֵ+��
	��ת������ģ�飺
				GND   	��Դ��
				VCC   	��3.3V��Դ
				SW		��PB11
				DT		��PB10
				CLK		��PB1
	OLED��ʾ����
				GND   	��Դ��
				VCC   	��3.3V��Դ
				SCL   	��PB8��SCL��
				SDA   	��PB9��SDA��
	����USB-TTL�ӷ���	
				GND   	��Դ��
				3V3   	��3.3V
				TXD   	��PB7
				RXD   	��PB6
	ST-LINK V2�ӷ���
				GND   	��Դ��
				3V3   	��3.3V
				SWCLK 	��DCLK
				SWDIO 	��DIO
	
	by J20�����Ŷ�
*/
#include "adc.h"
#include "delay.h"
#include "usart.h"
#include "rtc.h"
#include "stm32f10x.h"
#include "oled.h"
#include "stdio.h"
#include "string.h"
#include "nrf24l01.h"
#include "led.h"
#include "key.h"
#include "flash.h"
#include "menu.h"
extern unsigned char logo[];
u16 lastThrPWM = 0;//��һʱ�̵����Ŵ�С
u16 loca;//�������
u16 updateWindow[10];//���ڸ��±�־
u16 thrNum;//���Ż����Ĵ�С
float lastBatVolt=0.00;//��һʱ�̵ĵ�ص�ѹ
extern char batVoltStr[10];//��ص�ѹ�ַ���

int main()
{
	delay_init();//��ʼ����ʱ����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2��2λ��ռ���ȼ���2λ�����ȼ�
	usart_init(115200);//��ʼ������1��������Ϊ115200
	TIM2_Init(1999,71);//1MHz��ÿ10ms����ADC����һ��
	TIM3_Init(19999,71);//1MHz��ÿ20ms��ⰴ��һ�Σ�
	DMA1_Init();	//DMA��ʼ��
	Adc_Init();		//ADC��ʼ��
	RTC_Init();		//RTC��ʼ��
	LED_Init();		//LED��ʼ��
	KEY_Init();		//KEY��ʼ��
	NRF24L01_Init();//��ʼ��NRF24L01
	
	OLED_Init();	//��ʼ��OLED
	OLED_Clear();
	OLED_DrawPointBMP(9,0,logo,110,56,1);//��ʾlogo
	OLED_Refresh_Gram();//ˢ���Դ�
	
	while(NRF24L01_Check())
	{
 		delay_ms(200);
		LED = !LED;// LED��˸��ʾ���ڵȴ�����ģ������
	}
	LED = 0;//Ϩ��LED
	NRF24L01_TX_Mode();
	delay_ms(1000);
		
	mainWindow();//��ʾ������
	OLED_Refresh_Gram();//ˢ���Դ�
	while (1){
		if(nowMenuIndex!=0){
			sendDataPacket();//�������ݰ�
			sendCount++;
		}
		if(sendCount == 5)//ÿ��10�μ��һ��
		{
			if(batVoltSignal==1) Beeper = !Beeper;//������������У�����
			else Beeper = 0;//������
			LED = !LED;// LED��˸��ʾ���ڷ�������
			if(abs(PWMvalue[2]/20-lastThrPWM)>0) updateWindow[0] = 1;
			lastThrPWM = PWMvalue[2]/20;//��1000���̽���20��Ƶ
			if(batVolt*100-lastBatVolt*100>5) updateWindow[0] = 1;//0.05V����һ��
			lastBatVolt = batVolt;
		}
		
		if(updateWindow[0] && nowMenuIndex==0 && sendCount == 10)//���Ÿ����¼�
		{
			thrNum = (int)(PWMvalue[2]-1000)/16;//��������
			OLED_Fill(0,63-thrNum,0,63,1);//�²���д1
			OLED_Fill(0,0,0,63-thrNum,0);//�ϲ���д0
			
			sprintf((char *)batVoltStr,"%2.2fV",batVolt);//���µ�ص�ѹ
			OLED_ShowString(90,0, (u8 *)batVoltStr,12,1);
			OLED_Refresh_Gram();//ˢ���Դ�
			updateWindow[0] = 0;
			sendCount = 0;
		}
		if(sendCount > 10) sendCount = 0;
		if(keyEvent>0)//΢�������¼�
		{
			if(nowMenuIndex==0)
			{
				if(keyEvent==1|keyEvent==2) 
				{
					OLED_Fill(66,59,124,62,0);//д0�����ԭ���ı�־
					loca = (int)95+setData.PWMadjustValue[0]/8;
					OLED_Fill(loca,59,loca,62,1);//д1
					OLED_DrawPlusSign(95,61);//���ı�ʶ
				}
				if(keyEvent==3|keyEvent==4) 
				{
					OLED_Fill(123,1,126,63,0);//д0
					loca = (int)32-setData.PWMadjustValue[1]/8;
					OLED_Fill(123,loca,126,loca,1);//д1
					OLED_DrawPlusSign(125,32);//���ı�ʶ
				}
				if(keyEvent==5|keyEvent==6) 
				{	
					OLED_Fill(4,59,62,62,0);//д0�����ԭ���ı�־
					loca = (int)33+setData.PWMadjustValue[3]/8;
					OLED_Fill(loca,59,loca,62,1);//д1
					OLED_DrawPlusSign(33,61);//���ı�ʶ
				}
				OLED_Refresh_Gram();//ˢ���Դ�
			}
			STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//���û�����д��FLASH
			keyEvent = 0;
		}
		if(nowMenuIndex==13)//ͨ��У׼
		{
			subMenu3_1();
			OLED_Refresh_Gram();//ˢ���Դ�
			for(int i=0;i<4;i++)
			{
				if(chResult[i]>setData.chUpper[i]) setData.chUpper[i]=chResult[i];
				if(chResult[i]<setData.chLower[i]) setData.chLower[i]=chResult[i];
			}
		}
		
		if(menuEvent[0])//�˵��¼�
		{
			OLED_display();
			if(nowMenuIndex==13 && lastMenuIndex != 13)//ͨ��������У׼
			{
				for(int i=0;i<4;i++)
				{
					setData.chLower[i] 	= chResult[i];	//ң�˵���Сֵ����
					setData.chMiddle[i] = chResult[i];	//ң�˵���ֵ
					setData.chUpper[i] 	= chResult[i];	//ң�˵����ֵ����
				}
			}
			
			for(int i=0;i<4;i++)//����΢����Χ
			{
				if(setData.PWMadjustValue[i]>200-setData.PWMadjustUnit) setData.PWMadjustValue[i]=200-setData.PWMadjustUnit;
				if(setData.PWMadjustValue[i]<setData.PWMadjustUnit-200) setData.PWMadjustValue[i]=setData.PWMadjustUnit-200;
			}
			if(setData.PWMadjustUnit>8) setData.PWMadjustUnit = 8;//����΢����λ��Χ
			if(setData.PWMadjustUnit<1) setData.PWMadjustUnit = 2;
			if(menuEvent[1]==NUM_up)
			{
				if(nowMenuIndex==5){setData.PWMadjustValue[0] += setData.PWMadjustUnit;subMenu1_1();}
				if(nowMenuIndex==6){setData.PWMadjustValue[1] += setData.PWMadjustUnit;subMenu1_2();}
				if(nowMenuIndex==7){setData.PWMadjustValue[2] += setData.PWMadjustUnit;subMenu1_3();}
				if(nowMenuIndex==8){setData.PWMadjustValue[3] += setData.PWMadjustUnit;subMenu1_4();}
				if(nowMenuIndex==9) {setData.chReverse[0] = !setData.chReverse[0];subMenu2_1();}
				if(nowMenuIndex==10) {setData.chReverse[1] = !setData.chReverse[1];subMenu2_2();}
				if(nowMenuIndex==11) {setData.chReverse[2] = !setData.chReverse[2];subMenu2_3();}
				if(nowMenuIndex==12) {setData.chReverse[3] = !setData.chReverse[3];subMenu2_4();}
				if(nowMenuIndex==14) {setData.throttlePreference = !setData.throttlePreference;subMenu4_1();}
				if(nowMenuIndex==15) {setData.batVoltAdjust += 1;subMenu4_2();}
				if(nowMenuIndex==16) {setData.warnBatVolt += 0.01;subMenu4_3();}
				if(nowMenuIndex==17) {setData.PWMadjustUnit += 1;subMenu4_4();}
			}
			if(menuEvent[1]==NUM_down)
			{
				if(nowMenuIndex==5){setData.PWMadjustValue[0] -= setData.PWMadjustUnit;subMenu1_1();}
				if(nowMenuIndex==6){setData.PWMadjustValue[1] -= setData.PWMadjustUnit;subMenu1_2();}
				if(nowMenuIndex==7){setData.PWMadjustValue[2] -= setData.PWMadjustUnit;subMenu1_3();}
				if(nowMenuIndex==8){setData.PWMadjustValue[3] -= setData.PWMadjustUnit;subMenu1_4();}
				if(nowMenuIndex==9) {setData.chReverse[0] = !setData.chReverse[0];subMenu2_1();}
				if(nowMenuIndex==10) {setData.chReverse[1] = !setData.chReverse[1];subMenu2_2();}
				if(nowMenuIndex==11) {setData.chReverse[2] = !setData.chReverse[2];subMenu2_3();}
				if(nowMenuIndex==12) {setData.chReverse[3] = !setData.chReverse[3];subMenu2_4();}
				if(nowMenuIndex==14) {setData.throttlePreference = !setData.throttlePreference;subMenu4_1();}
				if(nowMenuIndex==15) {setData.batVoltAdjust -= 1;subMenu4_2();}
				if(nowMenuIndex==16) {setData.warnBatVolt -= 0.01;subMenu4_3();}
				if(nowMenuIndex==17) {setData.PWMadjustUnit -= 1;subMenu4_4();}
			}
			if(nowMenuIndex!=lastMenuIndex && lastMenuIndex>=5 && lastMenuIndex<=17)
			{
				STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//���û�����д��FLASH
			}
			OLED_Refresh_Gram();//ˢ���Դ�
			
			menuEvent[0] = 0;
			menuEvent[1] = BM_NULL;
		}
		lastMenuIndex = nowMenuIndex;
	}
	
}

