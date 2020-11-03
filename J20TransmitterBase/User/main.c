/*
=============J20��ģң����ң������-������V2.0==============
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
#include "beeper.h"
#include "key.h"
#include "flash.h"
#include "menu.h"
#include "ppm.h"

void keyEventHandle(void);
void menuEventHandle(void);
	
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
	BEEPER_Init();	//BEEPER��ʼ��
	KEY_Init();		//KEY��ʼ��
	NRF24L01_Init();//NRF24L01��ʼ��
//	PPM_Pin_Init();//PPM��ʼ��
//	systick_init(10000);//PPM��ʱ��ʼ������ʼֵ��������
	
	OLED_Init();	//��ʼ��OLED
	OLED_Clear();
	OLED_DrawPointBMP(9,0,logo,110,56,1);//��ʾlogo
	OLED_Refresh_Gram();//ˢ���Դ�
	beeperOnce();
	while(NRF24L01_Check())
	{
 		delay_ms(100);
		Beeper = !Beeper;//������1Hz��������ʾ����ģ�����
	}
	NRF24L01_TX_Mode();
	delay_ms(100);
		
	mainWindow();//��ʾ������
	OLED_Refresh_Gram();//ˢ���Դ�
	while (1){
		if(sendCount == 5)//ÿ��20�μ��һ��
		{
			if(batVoltSignal==1) Beeper = 1;//������������У�����
			else Beeper = 0;//������
			if(abs(PWMvalue[2]/20-lastThrPWM)>0) updateWindow[0] = 1;
			lastThrPWM = PWMvalue[2]/20;//��1000���̽���20��Ƶ
			if(batVolt*100-lastBatVolt*100>5) updateWindow[0] = 1;//0.05V����һ��
			lastBatVolt = batVolt;
		}
		
		if(updateWindow[0] && nowMenuIndex==0 && sendCount == 10)//���Ÿ����¼�
		{
			thrNum = (int)(PWMvalue[2]-1000)/16;//��������
			if(setData.throttlePreference)//��������
			{
				OLED_Fill(0,63-thrNum,0,63,1);//�²���д1
				OLED_Fill(0,0,0,63-thrNum,0);//�ϲ���д0
			}
			else{//��������
				OLED_Fill(127,63-thrNum,127,63,1);//�²���д1
				OLED_Fill(127,0,127,63-thrNum,0);//�ϲ���д0
			}
			sprintf((char *)batVoltStr,"%2.2fV",batVolt);//���µ�ص�ѹ
			OLED_ShowString(90,0, (u8 *)batVoltStr,12,1);
			OLED_Refresh_Gram();//ˢ���Դ�
			updateWindow[0] = 0;
			sendCount = 0;
		}
		if(sendCount > 20) sendCount = 0;
		if(keyEvent>0)//΢�������¼�
		{
			beeperOnce();
			keyEventHandle();
		}
		if(nowMenuIndex==xcjz14)//�г�У׼
		{
			menu_xcjz14();
			OLED_Refresh_Gram();//ˢ���Դ�
			for(int i=0;i<4;i++)
			{
				if(chResult[i]>setData.chUpper[i]) setData.chUpper[i]=chResult[i];
				if(chResult[i]<setData.chLower[i]) setData.chLower[i]=chResult[i];
			}
		}
		if(nowMenuIndex==dljs18)//��������
		{
			menu_dljs18();
			OLED_Refresh_Gram();//ˢ���Դ�
		}
		if(menuEvent[0])//�˵��¼�
		{
			beeperOnce();
			menuEventHandle();
		}
		lastMenuIndex = nowMenuIndex;
	}
}


//΢���¼�������������������
void keyEventHandle(void)
{
	if(nowMenuIndex==0)
	{
		if(keyEvent==1|keyEvent==2) 
		{//��1ͨ��΢��
			if(setData.throttlePreference)//��������
			{
				OLED_Fill(66,59,124,62,0);//д0�����ԭ���ı�־
				loca = (int)95+setData.PWMadjustValue[0]/12;
				OLED_Fill(loca,59,loca,62,1);//д1
				OLED_DrawPlusSign(95,61);//���ı�ʶ
			}
			else//��������
			{//��4ͨ��΢��
				OLED_Fill(66,59,124,62,0);//д0�����ԭ���ı�־
				loca = (int)95+setData.PWMadjustValue[3]/12;
				OLED_Fill(loca,59,loca,62,1);//д1
				OLED_DrawPlusSign(95,61);//���ı�ʶ
			}
		}
		if(keyEvent==3|keyEvent==4) 
		{
			if(setData.throttlePreference)//��������
			{//��2ͨ��΢��
				OLED_Fill(123,1,126,63,0);//д0
				loca = (int)32-setData.PWMadjustValue[1]/12;
				OLED_Fill(123,loca,126,loca,1);//д1
				OLED_DrawPlusSign(125,32);//���ı�ʶ
			}
			else//��������
			{//��2ͨ��΢��
				OLED_Fill(1,1,4,63,0);//д0
				loca = (int)32-setData.PWMadjustValue[1]/12;
				OLED_Fill(1,loca,4,loca,1);//д1
				OLED_DrawPlusSign(2,32);//���ı�ʶ
			}
		}
		if(keyEvent==5|keyEvent==6) 
		{	
			if(setData.throttlePreference)//��������
			{//��4ͨ��΢��
				OLED_Fill(4,59,62,62,0);//д0�����ԭ���ı�־
				loca = (int)33+setData.PWMadjustValue[3]/12;
				OLED_Fill(loca,59,loca,62,1);//д1
				OLED_DrawPlusSign(33,61);//���ı�ʶ
			}
			else//��������
			{//��1ͨ��΢��
				OLED_Fill(4,59,62,62,0);//д0�����ԭ���ı�־
				loca = (int)33+setData.PWMadjustValue[0]/12;
				OLED_Fill(loca,59,loca,62,1);//д1
				OLED_DrawPlusSign(33,61);//���ı�ʶ
			}
		}
		OLED_Refresh_Gram();//ˢ���Դ�
		STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//���û�����д��FLASH
	}
	keyEvent = 0;
}

//�˵��¼������������²˵����桢���ݴ���
void menuEventHandle(void)
{
	OLED_display();
	if(nowMenuIndex==xcjz14 && lastMenuIndex != xcjz14)//ͨ��������У׼
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
		if(setData.PWMadjustValue[i]>300-setData.PWMadjustUnit) setData.PWMadjustValue[i]=300-setData.PWMadjustUnit;
		if(setData.PWMadjustValue[i]<setData.PWMadjustUnit-300) setData.PWMadjustValue[i]=setData.PWMadjustUnit-300;
	}
	if(setData.PWMadjustUnit>8) setData.PWMadjustUnit = 8;//����΢����λ��Χ
	if(setData.PWMadjustUnit<1) setData.PWMadjustUnit = 2;
	if(menuEvent[1]==NUM_up)
	{
		if(nowMenuIndex==tdwt1){setData.PWMadjustValue[0] += setData.PWMadjustUnit;menu_tdwt1();}
		if(nowMenuIndex==tdwt2){setData.PWMadjustValue[1] += setData.PWMadjustUnit;menu_tdwt2();}
		if(nowMenuIndex==tdwt3){setData.PWMadjustValue[2] += setData.PWMadjustUnit;menu_tdwt3();}
		if(nowMenuIndex==tdwt4){setData.PWMadjustValue[3] += setData.PWMadjustUnit;menu_tdwt4();}
		if(nowMenuIndex==tdwt5){setData.PWMadjustValue[4] += setData.PWMadjustUnit;menu_tdwt5();}
		if(nowMenuIndex==tdwt6){setData.PWMadjustValue[5] += setData.PWMadjustUnit;menu_tdwt6();}
		if(nowMenuIndex==tdwt7){setData.PWMadjustValue[6] += setData.PWMadjustUnit;menu_tdwt7();}
		if(nowMenuIndex==tdwt8){setData.PWMadjustValue[7] += setData.PWMadjustUnit;menu_tdwt8();}
		if(nowMenuIndex==tdzf1){setData.chReverse[0] = !setData.chReverse[0];menu_tdzf1();}
		if(nowMenuIndex==tdzf2){setData.chReverse[1] = !setData.chReverse[1];menu_tdzf2();}
		if(nowMenuIndex==tdzf3){setData.chReverse[2] = !setData.chReverse[2];menu_tdzf3();}
		if(nowMenuIndex==tdzf4){setData.chReverse[3] = !setData.chReverse[3];menu_tdzf4();}
		if(nowMenuIndex==tdzf5){setData.chReverse[4] = !setData.chReverse[4];menu_tdzf5();}
		if(nowMenuIndex==tdzf6){setData.chReverse[5] = !setData.chReverse[5];menu_tdzf6();}
		if(nowMenuIndex==tdzf7){setData.chReverse[6] = !setData.chReverse[6];menu_tdzf7();}
		if(nowMenuIndex==tdzf8){setData.chReverse[7] = !setData.chReverse[7];menu_tdzf8();}
		if(nowMenuIndex==ymph) {setData.throttlePreference = !setData.throttlePreference;menu_ymph();}
		if(nowMenuIndex==dyjz) {setData.batVoltAdjust += 1;menu_dyjz();}
		if(nowMenuIndex==bjdy) {setData.warnBatVolt += 0.01;menu_bjdy();}
		if(nowMenuIndex==wtdw) {setData.PWMadjustUnit += 1;menu_wtdw();}
	}
	if(menuEvent[1]==NUM_down)
	{
		if(nowMenuIndex==tdwt1){setData.PWMadjustValue[0] -= setData.PWMadjustUnit;menu_tdwt1();}
		if(nowMenuIndex==tdwt2){setData.PWMadjustValue[1] -= setData.PWMadjustUnit;menu_tdwt2();}
		if(nowMenuIndex==tdwt3){setData.PWMadjustValue[2] -= setData.PWMadjustUnit;menu_tdwt3();}
		if(nowMenuIndex==tdwt4){setData.PWMadjustValue[3] -= setData.PWMadjustUnit;menu_tdwt4();}
		if(nowMenuIndex==tdwt5){setData.PWMadjustValue[4] -= setData.PWMadjustUnit;menu_tdwt5();}
		if(nowMenuIndex==tdwt6){setData.PWMadjustValue[5] -= setData.PWMadjustUnit;menu_tdwt6();}
		if(nowMenuIndex==tdwt7){setData.PWMadjustValue[6] -= setData.PWMadjustUnit;menu_tdwt7();}
		if(nowMenuIndex==tdwt8){setData.PWMadjustValue[7] -= setData.PWMadjustUnit;menu_tdwt8();}
		if(nowMenuIndex==tdzf1){setData.chReverse[0] = !setData.chReverse[0];menu_tdzf1();}
		if(nowMenuIndex==tdzf2){setData.chReverse[1] = !setData.chReverse[1];menu_tdzf2();}
		if(nowMenuIndex==tdzf3){setData.chReverse[2] = !setData.chReverse[2];menu_tdzf3();}
		if(nowMenuIndex==tdzf4){setData.chReverse[3] = !setData.chReverse[3];menu_tdzf4();}
		if(nowMenuIndex==tdzf5){setData.chReverse[4] = !setData.chReverse[4];menu_tdzf5();}
		if(nowMenuIndex==tdzf6){setData.chReverse[5] = !setData.chReverse[5];menu_tdzf6();}
		if(nowMenuIndex==tdzf7){setData.chReverse[6] = !setData.chReverse[6];menu_tdzf7();}
		if(nowMenuIndex==tdzf8){setData.chReverse[7] = !setData.chReverse[7];menu_tdzf8();}
		if(nowMenuIndex==ymph) {setData.throttlePreference = !setData.throttlePreference;menu_ymph();}
		if(nowMenuIndex==dyjz) {setData.batVoltAdjust -= 1;menu_dyjz();}
		if(nowMenuIndex==bjdy) {setData.warnBatVolt -= 0.01;menu_bjdy();}
		if(nowMenuIndex==wtdw) {setData.PWMadjustUnit -= 1;menu_wtdw();}
	}
	if(nowMenuIndex!=lastMenuIndex)
	{
		STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//���û�����д��FLASH
	}
	OLED_Refresh_Gram();//ˢ���Դ�
	menuEvent[0] = 0;
	menuEvent[1] = BM_NULL;
}
