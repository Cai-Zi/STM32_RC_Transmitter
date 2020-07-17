#ifndef __CONFIG_H
#define __CONFIG_H
#include "stm32f10x.h" //�ǵ���Ӵ�ͷ�ļ�����Ϊconfig.c�õ�GPIO��غ�����
#include "sys.h"
#define chNum 9		//����ͨ����
#define sampleNum 10 //ÿ��ͨ����������
extern u16 chValue[chNum*10];//ADC����ֵ
extern u16 PWMvalue[chNum];//����PWMռ�ձ�
extern u16 chLower[chNum];//ң�˵���Сֵ
extern u16 chMiddle[chNum];//ң�˵���ֵ
extern u16 chUpper[chNum];//ң�˵����ֵ
extern u16 chReverse[chNum];//ͨ����������1Ϊ������0Ϊ��ת
extern float batVolt,warnBatVolt;//��ص�ѹ��������ѹ��
extern u8 batVoltSignal;//�Ƿ񱨾�

void TIM2_Init(u16 arr,u16 psc);//TIM2��ʱ����ʼ��
void DMA1_Init(void);
void GPIOA_Init(void);
void Adc_Init(void);//ADC1��ʼ��
u16 Get_Adc(u8 ch); //��ȡһ��ADC��ֵ
u16 Get_Adc_Average(u8 ch,u8 times);//ADC����ֵ���о�ֵ�˲�
float map(float value,float fromLow,float fromHigh,float toLow,float toHigh);//ӳ�亯��
int mapChValue(int val, int lower, int middle, int upper, int reverse);//ͨ������ӳ�亯��
int GetMedianNum(volatile u16 * bArray, int ch);//��ֵ�˲�
#endif
