/*
=============J20��ģң�������ն�-������V1.0==============
	оƬSTM32F103C8T6��ʵ����NRF24L01���߽��պ�10��ͨ����PWM���
	NRF24L01ģ�飺
				GND   	��Դ��
				VCC		��3.3V��Դ
				CSN		PB12
				SCK		PB13
				MISO	PB14
				MOSI	PB15
				CE		PA8
				IRQ		PA9
	PWM�����PA0-3��PA6-7��PB0-1
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
#include "pwm.h"
#include "delay.h"
#include "usart.h"
#include "stm32f10x.h"
#include "oled.h"
#include "rtc.h" 
#include "stdio.h"
#include "string.h"
#include "nrf24l01.h"
#include "led.h"
#include "tim.h"

u16 PWMvalue[chNum];//����PWMռ�ձ�
int main()
{
	u16 i=0,startIndex=0;
	u32 lastTime = 0;
	u8 chPacket[32];
//	u8 txt[16]={0};
	delay_init();//��ʼ����ʱ����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2��2λ��ռ���ȼ���2λ�����ȼ�
	usart_init(115200);//��ʼ������1��������Ϊ115200
	TIM3_PWM_Init(19999,71);//Ԥ��Ƶ72��Ƶ��1MHz������1us���Զ�װ��ֵ20 000����PWM����1us*20 000
	TIM2_PWM_Init(19999,71);
	TIM4_Counter_Init(9,71); //Ԥ��Ƶ1MHz������1us���Զ�װ��ֵ10������С������λ10us
	RTC_Init();	  	//RTC��ʼ��
	LED_Init();		//LED��ʼ��
	OLED_Init();	//��ʼ��OLED  
	OLED_Clear();
	NRF24L01_Init();//��ʼ��NRF24L01
	while(NRF24L01_Check())
	{
 		delay_ms(200);
	}
		
	NRF24L01_RX_Mode();
	while (1){ 	
		if(NRF24L01_RxPacket(chPacket)==0)
		{
			for(i=0;i<32-chNum*2;i++)//Ѱ����ʼλ�ã�һ���ڵ�2-3��Ԫ��
			{
				if(chPacket[i]==0x00 & chPacket[i+1]==0x00) 
				{
					startIndex = i+2;
					break;
				}
			}
			for(i=0;i<chNum;i++)//����ʼλ�ÿ�ʼ����
			{
				PWMvalue[i] = ((u16)chPacket[startIndex] << 8) | ((u16)(chPacket[startIndex+1]));// �ϲ�u8Ϊu16
				startIndex = startIndex+2;
			}
			TIM_SetCompare1(TIM2,PWMvalue[0]);//�����PWM-PA0
			TIM_SetCompare2(TIM2,PWMvalue[1]);//�����PWM-PA1
			TIM_SetCompare3(TIM2,PWMvalue[2]);//�����PWM-PA2
			TIM_SetCompare4(TIM2,PWMvalue[3]);//�����PWM-PA3
			TIM_SetCompare1(TIM3,PWMvalue[4]);//�����PWM-PA6
			TIM_SetCompare2(TIM3,PWMvalue[5]);//�����PWM-PA7
			TIM_SetCompare3(TIM3,PWMvalue[6]);//�����PWM-PB0
			TIM_SetCompare4(TIM3,PWMvalue[7]);//�����PWM-PB1
			//printf("%d\t%d\t%d\t%d\t%d\n",PWMvalue[0],PWMvalue[1],PWMvalue[2],PWMvalue[3],PWMvalue[4]);
			LED = 0;
			lastTime = nowTime;
//			if(PWMvalue[1]>1800) testPin = 1;
//			else testPin = 0;
		}
//		printf("%d\t%d\n",nowTime,lastTime);
		if(nowTime-lastTime>100*2000)//�����ϴν���ʱ�����2s����˵��ʧȥ�ź�
		{
			LED=!LED;
			delay_ms(200);
		}
//		itoa(PWM1value,txt,10);//��int����ת����10�����ַ���
//		OLED_ShowString(6,3,txt,24); //λ��6,3���ַ���С24*24����
//		OLED_Refresh_Gram();
	}
}
