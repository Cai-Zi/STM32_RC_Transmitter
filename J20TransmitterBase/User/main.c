/*
=============J20��ģң����ң������-������V1.0==============
	оƬSTM32F103C8T6��ʵ����NRF24L01���߷����10��ͨ����ADC����
	NRF24L01ģ�飺
				GND   	��Դ��
				VCC		��3.3V��Դ
				CSN		PB12
				SCK		PB13
				MISO	PB14
				MOSI	PB15
				CE		PA8
				IRQ		PA9
	ADC������PA0-7��PB0-1
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
	
	by Bilibili ����CaiZi
*/
#include "adc.h"
#include "delay.h"
#include "usart.h"
#include "stm32f10x.h"
#include "oled.h"
#include "stdio.h"
#include "string.h"
#include "nrf24l01.h"
#include "led.h"
int main()
{
	u16 t=0, tNum=0;
	u8 chPacket[32];
//	u8 txt[16]={0};
	delay_init();//��ʼ����ʱ����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2��2λ��ռ���ȼ���2λ�����ȼ�
	usart_init(115200);//��ʼ������1��������Ϊ115200
	TIM2_Init(499,71);//1MHz��ÿ500us�ɼ�һ�Σ�������9us���ϣ�����СӰ��OLED��ʾ
	DMA1_Init();	//DMA��ʼ��
	GPIOA_Init();	//PA��ʼ��
	Adc_Init();		//ADC��ʼ��
	//RTC_Init();	  	//RTC��ʼ��
	LED_Init();		//LED��ʼ��
	OLED_Init();	//��ʼ��OLED  
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
			LED = !LED;// LED��˸��ʾ���ڷ�������
			tNum = 0;
		}	
//		itoa(PWM1value,txt,10);//��int����ת����10�����ַ���
//		OLED_ShowString(6,3,txt,24); //λ��6,3���ַ���С24*24����
//		OLED_Refresh_Gram();
	}
}

