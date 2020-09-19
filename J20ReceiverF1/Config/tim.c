#include "tim.h" //�ǵ���Ӵ�ͷ�ļ�����Ϊconfig.c�õ�GPIO��غ�����
#include "sys.h"
#include "pwm.h"

#define MS20 ((int)(500*20/1.15))	/* 20ms */
#define MS05 ((int)(250/1.15))		/* 0.5ms */
enum PPM_OUTPUT_CH_STATE {
    CH_DOWN_STATE,
    CH_UP_STATE,
};

static enum PPM_OUTPUT_CH_STATE state = CH_DOWN_STATE;
static uint64_t total_value = 0;
static uint8_t ch_idx = 0;
uint32_t ch_val = 0;
volatile u32 nowTime = 0;//��������ʱ�䣬��λ10us
u32 ppmCount = 0;//ppm������2usһ��
volatile u32 chTime[chNum] = {0};//72000--1ms,72000*2--2ms


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
		ppmCount++;
		ppm_output();
	}
}

void Clock_Start(){
	TIM4->CNT=0x00;
	TIM_Cmd(TIM4,ENABLE);
}

u32 Clock_End(){
	u32 result;
	result = TIM4->CNT;
	TIM_Cmd(TIM4,DISABLE);
	return result;
}


void ppm_output(void){
    /* ����CH1~CH8�����һ���͵�ƽʱ���*/
    if(ppmCount>=total_value)
	{
		if(total_value==MS20)
		{
			total_value = 0;
			ppmCount = 0; 
		}
		if (CH_DOWN_STATE == state) {
			/* 0.5ms�������һ��systick�ж� */
			total_value += MS05;
			state = CH_UP_STATE;
			PPM_Pin = 0;
		} 
		else {
			/* ͨ��CH1~CH8�ߵ�ƽʱ��� */
			if (ch_idx < chNum) {
				if (chTime[ch_idx] < (MS05))
					ch_val = MS05;
				else {
					ch_val = (chTime[ch_idx]) - MS05;
				}
				total_value += ch_val;
				ch_idx++;
			} 
			else {
				/* ���һ�����ĸߵ�ƽ��� */
				total_value = MS20;
				ch_idx = 0;
			}
			state = CH_DOWN_STATE;
			PPM_Pin = 1;
		}
	}
}


