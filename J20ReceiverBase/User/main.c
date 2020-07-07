/*
=============J20��ģң����ң������-������V1.0==============
	оƬSTM32F103C8T6��ʹ��ADC��ȡ��λ���ĵ�ѹ����ֵ��0~4095ת����1000~2000����ֵ��PWM�����
					 TIM2��ʱ����ADC������ͨ��DMA���������ch1Value��ȡ10�ν��о�ֵ�˲���
	���ƶ����PWM������20ms���ߵ�ƽʱ��0.5ms~2.5ms�仯���ɿ��ƶ��0~180��ĽǶȱ仯��
				   ����ģ�����ʵ�ʿ����У���������180��仯������ͨ�õĸߵ�ƽ�����ʵ��1ms~2ms
	��λ����GND - PA0 - 3.3V
	OLED��ʾ����
				GND   ��Դ��
				VCC   ��3.3v��Դ
				SCL   ��PB8��SCL��
				SDA   ��PB9��SDA��
	����USB-TTL�ӷ���	
				GND   ��Դ��
				3V3   ��3.3v
				TXD   ��PB7
				RXD   ��PB6
	ST-LINK V2�ӷ���
				GND   ��Դ��
				3V3   ��3.3v
				SWCLK ��DCLK
				SWDIO ��DIO
	PWM�����PB5
	by Bilibili ����CaiZi
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
			TIM_SetCompare2(TIM3,PWMvalue[0]);//�����PWM-PB5
			//printf("%d\n",PWMvalue[0]);//printf("\n");
			LED = 0;
			lastTime = nowTime;
			if(PWMvalue[1]>1800) testPin = 1;
			else testPin = 0;
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
