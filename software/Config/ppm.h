#ifndef __PPM_H
#define __PPM_H
#include "main.h"
#define MS20	(20000)	/* 20ms */
#define MS05	(500)	/* 0.5ms */
#define PPM_NUM chNum*2+2 //PPM�����еĸ�����������8ͨ��1+8*2+1

extern u16 PPM_Array[PPM_NUM];

void PPM_Init(void);

#endif

