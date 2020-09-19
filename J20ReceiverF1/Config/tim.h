#ifndef __TIM_H
#define __TIM_H
#include "stm32f10x.h"
#include "sys.h"
#include "pwm.h"

#define PPM_Pin PAout(4)	// PA4

extern volatile u32 nowTime;//��������ʱ�䣬��λ10us
extern volatile u32 chTime[chNum];//ÿ��ͨ��ʱ��
void TIM4_Counter_Init(u16 arr,u16 psc);
void Clock_Start(void);
u32 Clock_End(void);
void ppm_output(void);
#endif
