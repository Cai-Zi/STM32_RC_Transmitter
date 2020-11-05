#include "main.h"
#include "tim.h"
#include "sys.h"

volatile u32 nowTime = 0;//��������ʱ�䣬��λ10us

void TIM4_Counter_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE); 
	
    TIM_TimeBaseInitStructure.TIM_Period = arr; //�Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //Ԥ��Ƶֵ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; // ���ϼ���
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; //ʱ�ӷָ�Ϊ0,��Ȼʹ��72MHz
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);//��������ж�
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	TIM_Cmd(TIM4,ENABLE);
}

void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update)!=RESET)//���TIM4�����жϷ������
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);//���TIM4�����жϱ�־
		nowTime++;
	}
}
extern char timeStr[9];//ʱ���ַ���
void drawClockTime(void)
{
	u16 hour=0,min=0,sec=0;
	sec = nowTime/10;
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

