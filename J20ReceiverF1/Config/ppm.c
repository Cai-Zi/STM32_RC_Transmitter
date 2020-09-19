#include "ppm.h"

void systick_init(u32 count)
{
	SysTick_Config(count);//����Ӧ��������ʱ��Դ֮ǰ,count��ֵ��LOAD�Ĵ���
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);//ѡ���ⲿʱ��HCLK,����72000 000Ϊ1s
	SysTick->VAL = 0;//��VAL=0,LOAD�Ĵ����е���װ��ֵ��ֵ��VAL�Ĵ�����Ϊ��ֵ��ÿ����һ��Systickʱ������,VAL�Ĵ���ֵ-1
}

void SysTick_Handler(void)//�ж����
{
    ppm_output();
}

// PPM-PA4�������
void PPM_Pin_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //ʹ��GPIO����ʱ��
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_4);						 //PA4 �����
}


u32 chTime[chNum];// ����PPMռ�ձ�

enum PPM_OUTPUT_CH_STATE {
    CH_DOWN_STATE,
    CH_UP_STATE,
};

static enum PPM_OUTPUT_CH_STATE state = CH_DOWN_STATE;

#define MS20	(72000 * 20)/* 20ms */
#define MS05	(36000)		/* 0.5ms */

static uint64_t total_value = 0;
static u8 ch_idx = 0;

void ppm_output(void)
{
    u32 ch_val = 0;
    
    /* CH1 ~ CH8�����һ���͵�ƽʱ��� */
    if (state == CH_DOWN_STATE) {
		/* 0.5ms�������һ����ʱ�ж� */
        systick_init(MS05);
        total_value += MS05;
        state = CH_UP_STATE;
        PPM_Pin = 0;
    } 
	else {
        /* CH1 ~ CH8 �ߵ�ƽ*/
        if (ch_idx < chNum) {
            if (chTime[ch_idx] < (MS05))
                ch_val = MS05;//ʹ���������ؼ����1ms����
            else {
                ch_val = chTime[ch_idx] - MS05;
            }
            systick_init(ch_val);
            total_value += ch_val;
            ch_idx++;
        } 
		else {
            /* ���һ���ߵ�ƽ */
            systick_init(MS20 - total_value);
            total_value = 0;
            ch_idx = 0;
        }
        state = CH_DOWN_STATE;
        PPM_Pin = 1;
    }     
}

