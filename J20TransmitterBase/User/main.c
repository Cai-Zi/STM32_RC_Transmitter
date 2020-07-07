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
