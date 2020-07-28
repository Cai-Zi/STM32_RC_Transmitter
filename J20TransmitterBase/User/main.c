/*
=============J20��ģң����ң������-������V1.1==============
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
				CH1Left ��PB5
				CH1Right��PB4
				CH2Up	��PA15
				CH2Down	��PB3
				CH4Left	��PA12
				CH4Right��PA11
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
extern unsigned char logoR[];
int main()
{
	
	//u8 txt[16]={0};//����ַ����ı�
	
	u16 lastThrPWM = 0;//��һʱ�̵����Ŵ�С
	u16 loca;//�������
	u16 updateWindow[10];//���ڸ��±�־
	u16 thrNum;//���Ż����Ĵ�С
	delay_init();//��ʼ����ʱ����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2��2λ��ռ���ȼ���2λ�����ȼ�
	OLED_Init();	//��ʼ��OLED
	OLED_Clear();
	OLED_DrawBMP(0,0,128,8,logoR);//��ʾlogo
	
	usart_init(115200);//��ʼ������1��������Ϊ115200
	TIM2_Init(999,71);//1MHz��ÿ1ms����ADC����һ�Σ�����������
	TIM3_Init(19999,71);//1MHz��ÿ20ms��ⰴ��һ�Σ�
	DMA1_Init();	//DMA��ʼ��
	Adc_Init();		//ADC��ʼ��
	RTC_Init();		//RTC��ʼ��
	LED_Init();		//LED��ʼ��
	KEY_Init();		//KEY��ʼ��
	NRF24L01_Init();//��ʼ��NRF24L01
	
	while(NRF24L01_Check())
	{
 		delay_ms(200);
	}
	NRF24L01_TX_Mode();
	delay_ms(1000);
	
	mainWindow();//��ʾ������
	OLED_Refresh_Gram();//д��Ӣ���ַ�����Ҫˢ���Դ棻ͼƬ����Ҫ
	while (1){
		if(sendCount > 100)//ÿ��100�μ��һ��
		{
			if(batVoltSignal==1) Beeper = !Beeper;//������������У�����
			else Beeper = 0;//������
			LED = !LED;// LED��˸��ʾ���ڷ�������
			if(abs(PWMvalue[2]/20-lastThrPWM)>0) updateWindow[0] = 1;
			lastThrPWM = PWMvalue[2]/20;//��1000���̽���20��Ƶ
			sendCount = 0;
		}
		
		if(updateWindow[0] && nowIndex==0)//���Ÿ����¼�
		{
			thrNum = (int)(PWMvalue[2]-1000)/16;
			OLED_Fill(0,63-thrNum,0,63,1);//�²���д1
			OLED_Fill(0,0,0,63-thrNum,0);//�ϲ���д0
			OLED_Refresh_Gram();//ˢ���Դ�
			updateWindow[0] = 0;
		}
		if(keyEvent>0)//΢�������¼�
		{
			if(nowIndex==0)
			{
				if(keyEvent==1|keyEvent==2) 
				{
					OLED_Fill(66,59,124,62,0);//д0�����ԭ���ı�־
					loca = (int)95+setData.PWMadjustValue[0]/4;
					OLED_Fill(loca,59,loca,62,1);//д1
					OLED_DrawPlusSign(95,61);//���ı�ʶ
				}
				if(keyEvent==3|keyEvent==4) 
				{
					OLED_Fill(123,1,126,63,0);//д0
					loca = (int)32+setData.PWMadjustValue[1]/4;
					OLED_Fill(123,loca,126,loca,1);//д1
					OLED_DrawPlusSign(125,32);//���ı�ʶ
				}
				if(keyEvent==5|keyEvent==6) 
				{	
					OLED_Fill(4,59,62,62,0);//д0�����ԭ���ı�־
					loca = (int)33+setData.PWMadjustValue[3]/4;
					OLED_Fill(loca,59,loca,62,1);//д1
					OLED_DrawPlusSign(33,61);//���ı�ʶ
				}
				OLED_Refresh_Gram();//ˢ���Դ�
			}
			STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);
			keyEvent = 0;
		}
		if(encoderEvent[0])//��ת�������¼�
		{
			OLED_display();
			encoderEvent[0] = 0;
		}
	}
}

