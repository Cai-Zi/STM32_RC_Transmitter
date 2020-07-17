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

int main()
{
	u16 t=0, tNum=0;
	u8 chPacket[32];
	for(int i=0;i<chNum;i++)
	{
		chLower[i] 	= 0;	//ң�˵���Сֵ
		chMiddle[i] = 2047;	//ң�˵���ֵ
		chUpper[i] 	= 4095;	//ң�˵����ֵ
		chReverse[i] = 1;	//ͨ����������1Ϊ������0Ϊ��ת
	}
	delay_init();//��ʼ����ʱ����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2��2λ��ռ���ȼ���2λ�����ȼ�
	usart_init(115200);//��ʼ������1��������Ϊ115200
	TIM2_Init(499,71);//1MHz��ÿ500us����ADC����һ�Σ�
	TIM3_Init(19999,71);//1MHz��ÿ20ms��ⰴ��һ�Σ�
	DMA1_Init();	//DMA��ʼ��
	Adc_Init();		//ADC��ʼ��
	RTC_Init();		//RTC��ʼ��
	LED_Init();		//LED��ʼ��
	KEY_Init();		//KEY��ʼ��
	OLED_Init();	//��ʼ��OLED,��һ�������ʱ6s
	OLED_Clear();
	NRF24L01_Init();//��ʼ��NRF24L01
	while(NRF24L01_Check())
	{
 		delay_ms(200);
	}
	NRF24L01_TX_Mode();
	while (1){
		
		if(NRF24L01_TxPacket(chPacket)==TX_OK)
		{
			//printf("%d\n",PWMvalue[1]);
			for(t=0;t<16;t++)
			{
				if(t==0)//��������ͷ00
				{
					chPacket[2*t]=0x00;
					chPacket[2*t+1]=0x00;
				}
				else if(t<=chNum)
				{
					chPacket[2*t] = (u8)(PWMvalue[t-1]>>8)&0xFF; //��8λ����u16��ֳ�����u8���д���
					chPacket[2*t+1] = (u8)PWMvalue[t-1]&0xFF; //��8λ
				}
				else 
				{
					chPacket[2*t]=0xff;
					chPacket[2*t+1]=0xff;
				}
			} 
			tNum ++;
		}
		if(tNum > 500)
		{
			if(batVoltSignal==1) Beeper = !Beeper;//������������У�����
			else Beeper = 0;//������
			LED = !LED;// LED��˸��ʾ���ڷ�������
			tNum = 0;
		}	
//		itoa(PWM1value,txt,10);//��int����ת����10�����ַ���
//		OLED_ShowString(6,3,txt,24); //λ��6,3���ַ���С24*24����
//		OLED_Refresh_Gram();
	}
}

