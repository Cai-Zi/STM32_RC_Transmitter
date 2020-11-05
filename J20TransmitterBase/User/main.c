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
#include "main.h"

void keyEventHandle(void);
void menuEventHandle(void);
	
extern unsigned char logo[];
u16 lastThrPWM = 0;//��һʱ�̵����Ŵ�С
u16 loca;//�������
u16 updateFlag;//���ڸ��±�־
u16 thrNum;//���Ż����Ĵ�С
float lastBatVolt=0.00;//��һʱ�̵ĵ�ص�ѹ
extern char batVoltStr[8];//��ص�ѹ�ַ���
extern char timeStr[9];//ʱ���ַ���
u16 count=0;
int main()
{
	delay_init();//��ʼ����ʱ����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2��2λ��ռ���ȼ���2λ�����ȼ�
	usart_init(115200);//��ʼ������1��������Ϊ115200
	TIM2_Init(1999,71);//1MHz��ÿ10ms����ADC����һ��
	TIM3_Init(19999,71);//1MHz��ÿ20ms��ⰴ��һ�Σ�
	TIM4_Counter_Init(999,7199);//ÿ0.1s����һ�Σ�ϵͳ��ʱ
	DMA1_Init();	//DMA��ʼ��
	Adc_Init();		//ADC��ʼ��
	set_Init();	//��ȡ�û�����
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
	
	OLED_Fill(0,0,127,63,0);//���
	mainWindow();//��ʾ������
	OLED_Refresh_Gram();//ˢ���Դ�
	while (1){
		if(count%500==0 && nowMenuIndex==home)//����ص�ѹ
		{
			if(batVoltSignal==1) beeperOnce();//������������У�����
			else Beeper = 0;//������
			sprintf((char *)batVoltStr,"%1.2fV",batVolt);
			OLED_ShowString(80,19, (u8 *)batVoltStr,16,1);//��ʾ��ص�ѹ
			OLED_Refresh_Gram();//ˢ���Դ�
		}
		if(count%100==0 && nowMenuIndex==home)//��ʾʱ��
		{
			drawClockTime();//��ʾʱ��00:00:00
			showSwState();//��ʾ���ĸ�ͨ��״̬
			OLED_Refresh_Gram();//ˢ���Դ�
		}
		if(count%20 == 0 && nowMenuIndex==home)//���Ÿ����¼�
		{
			if(abs(PWMvalue[2]-lastThrPWM)/10>0) updateFlag = 1;
			lastThrPWM = PWMvalue[2];//1000����=100%
			if(updateFlag)
			{
				thrNum = (int)(PWMvalue[2]-1000)/22;//��������
				if(setData.throttlePreference)//��������
				{
					OLED_Fill(2,62-thrNum,2,62,0);//�²���д1
					OLED_Fill(2,16,2,62-thrNum,1);//�ϲ���д0
				}
				else{//��������
					OLED_Fill(125,62-thrNum,125,62,0);//�²���д1
					OLED_Fill(125,16,125,62-thrNum,1);//�ϲ���д0
				}
				OLED_Refresh_Gram();//ˢ���Դ�
				updateFlag = 0;
			}
		}
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
		count++;
	}
}


//΢���¼�������������������
void keyEventHandle(void)
{
	if(nowMenuIndex==home)
	{
		if(keyEvent==1|keyEvent==2) 
		{
			if(setData.throttlePreference)//��������
			{//��1ͨ��΢��-�Һ���
				OLED_Fill(66,61,119,61,1);//д1�����ԭ���ı�־
				loca = (int)93+setData.PWMadjustValue[0]/12.5;
				OLED_Fill(loca-2,61,loca+2,61,0);//д0
			}
			else//��������
			{//��4ͨ��΢��-�Һ���
				OLED_Fill(66,61,119,61,1);//д1�����ԭ���ı�־
				loca = (int)93+setData.PWMadjustValue[3]/12.5;
				OLED_Fill(loca-2,61,loca+2,61,0);//д0
			}
		}
		if(keyEvent==3|keyEvent==4) 
		{
			if(setData.throttlePreference)//��������
			{//��2ͨ��΢��-������
				OLED_Fill(125,16,125,62,1);//д1
				loca = (int)39-setData.PWMadjustValue[1]/14.29;
				OLED_Fill(125,loca-2,125,loca+2,0);//д1
			}
			else//��������
			{//��2ͨ��΢��-������
				OLED_Fill(2,16,2,56,1);//д1
				loca = (int)39-setData.PWMadjustValue[1]/12;
				OLED_Fill(2,loca-2,2,loca+2,0);//д1
			}
		}
		if(keyEvent==5|keyEvent==6) 
		{	
			if(setData.throttlePreference)//��������
			{//��4ͨ��΢��-�����
				OLED_Fill(7,61,61,61,1);//д1�����ԭ���ı�־
				loca = (int)35+setData.PWMadjustValue[3]/12.5;
				OLED_Fill(loca-2,61,loca+2,61,0);//д0
			}
			else//��������
			{//��1ͨ��΢��-�����
				OLED_Fill(7,61,61,61,1);//д1�����ԭ���ı�־
				loca = (int)34+setData.PWMadjustValue[0]/12.5;
				OLED_Fill(loca-2,61,loca+2,61,0);//д0
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
		if(nowMenuIndex==xzmx) {setData.modelType += 1;if(setData.modelType>2) {setData.modelType=0;}menu_xzmx();}
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
		if(nowMenuIndex==xzmx) {if(setData.modelType==0){setData.modelType=2;}else {setData.modelType -= 1;}menu_xzmx();}
	}
	if(nowMenuIndex!=lastMenuIndex)
	{
		STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//���û�����д��FLASH
	}
	OLED_Refresh_Gram();//ˢ���Դ�
	menuEvent[0] = 0;
	menuEvent[1] = BM_NULL;
}
