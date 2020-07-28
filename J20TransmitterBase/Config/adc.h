#ifndef __ADC_H
#define __ADC_H
#include "stm32f10x.h" //�ǵ���Ӵ�ͷ�ļ�����Ϊconfig.c�õ�GPIO��غ�����
#include "sys.h"
#define adcNum 9		//����ͨ����
#define chNum 8	//ң��ͨ����
#define sampleNum 10 //ÿ��ͨ����������
#define setDataSize sizeof(setData)/2 //ÿ��ͨ����������

#pragma pack(1)//���ֽڶ��룬����Ҫ��
typedef struct _set_Config// �û����ýṹ��
{
	u16 writeFlag;//�Ƿ��һ��д��=2�ֽ�16λ
	u16 dataLen;//�ṹ������ݳ��ȣ�δ֪ԭ����ǰ4���ֽڶ�ȡ����ǰ�������ݶ��������������������������
	u16 chLower[chNum];//ң�˵���Сֵ=2*chNum
	u16 chMiddle[chNum];//ң�˵���ֵ
	u16 chUpper[chNum];//ң�˵����ֵ
	int PWMadjustValue[chNum];//΢��PWMռ�ձ�=4�ֽ�*chNum
	u16 chReverse[chNum];//ͨ����������1Ϊ������0Ϊ��ת
	u16 PWMadjustUnit;//΢����λ
	float warnBatVolt;//������ѹ=4���ֽ�32λ
}set_Config;
#pragma pack()

extern u16 chValue[adcNum*sampleNum];//ADC����ֵ
extern u16 PWMvalue[chNum];//����PWMռ�ձ�
extern u8 batVoltSignal;//�Ƿ񱨾�
extern set_Config setData;

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
