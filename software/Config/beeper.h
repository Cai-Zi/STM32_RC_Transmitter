#ifndef __BEEPER_H
#define __BEEPER_H	 
#include "sys.h"

#define Beeper PAout(10)	//PA10

void BEEPER_Init(void);//��ʼ��
void beeperOnce(u16 freq);//����������һ��
void keyDownSound(void);
void onSound(void);
#endif
