#ifndef __PWM_H
#define __PWM_H
#include "stm32f10x.h" //�ǵ���Ӵ�ͷ�ļ�����Ϊconfig.c�õ�GPIO��غ�����
#include "sys.h"
#define chNum 6		//����ͨ����
//extern u16 PWMvalue[chNum];//����PWMռ�ձ�

void TIM3_PWM_Init(u16 arr,u16 psc);//PB5��ʱ����ʼ��
#endif
