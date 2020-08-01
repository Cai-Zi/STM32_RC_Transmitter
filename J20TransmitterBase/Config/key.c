#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
#include "usart.h"
#include "adc.h"
#include "flash.h"
#include "menu.h"
#include "oled.h"
//�ο�����https://blog.csdn.net/qq_42679566/article/details/105892105��ԭ�Ĵ���������

Key_Config Key_Buf[KEY_NUM];	// ������������
u16 menuEvent[4];
u8 keyEvent=0;
#define KEY_LONG_DOWN_DELAY 30 	// ����30��TIM3��ʱ���ж�=600ms�㳤��	
#define DBGMCU_CR  (*((volatile u32 *)0xE0042004))
	
/*ͨ�ö�ʱ��3�жϳ�ʼ����ʹ��TIM3���ư�����ʱ���
  ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
* ������arr���Զ���װֵ��
		psc��ʱ��Ԥ��Ƶ��
*/
void TIM3_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 		//ʱ��ʹ��

	TIM_TimeBaseInitStructure.TIM_Period = arr; //�Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //Ԥ��Ƶֵ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; // ���ϼ���
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; //ʱ�ӷָ�Ϊ0,��Ȼʹ��72MHz
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//���������ж�
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��NVIC�Ĵ���
	
	TIM_Cmd(TIM3,ENABLE);
}

void TIM3_IRQHandler(void)   //TIM3�жϷ�����
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
	{
		// �жϴ�������
		ReadKeyStatus();  //����״̬��
		volatile u8 i,status,ch;
		for(i = 0;i < KEY_NUM;i++)
    	{
			status = Key_Buf[i].Status.KEY_EVENT;
			if(i<6 && status==KEY_DOWN && nowIndex==0) //�̰�
			{
				if(i==0 | i==1) ch = 0;
				if(i==2 | i==3) ch = 1;
				if(i==4 | i==5) ch = 3;//�ڼ���ͨ��
				if(setData.PWMadjustValue[ch]>100-setData.PWMadjustUnit) setData.PWMadjustValue[ch]=100-setData.PWMadjustUnit;//����΢����Χ
				if(setData.PWMadjustValue[ch]<setData.PWMadjustUnit-100) setData.PWMadjustValue[ch]=setData.PWMadjustUnit-100;//����΢����Χ
				if(i%2==0) setData.PWMadjustValue[ch] -= setData.PWMadjustUnit;//΢����
				else setData.PWMadjustValue[ch] += setData.PWMadjustUnit;//΢����
				keyEvent = i+1;//�а������±�־
			}
			if(i<6 && status==KEY_LONG && nowIndex==0) //����
			{
				Key_Buf[i].Status.KEY_COUNT = 29;//���ڼӼ��ٶȣ�ҪС��KEY_LONG_DOWN_DELAY
				if(i==0 | i==1) ch = 0;
				if(i==2 | i==3) ch = 1;
				if(i==4 | i==5) ch = 3;//�ڼ���ͨ��
				
				if(setData.PWMadjustValue[ch]>100-setData.PWMadjustUnit) setData.PWMadjustValue[ch]=100-setData.PWMadjustUnit;//����΢����Χ
				if(setData.PWMadjustValue[ch]<setData.PWMadjustUnit-100) setData.PWMadjustValue[ch]=setData.PWMadjustUnit-100;//����΢����Χ
				
				if(i%2==0) setData.PWMadjustValue[ch] -= setData.PWMadjustUnit;
				else setData.PWMadjustValue[ch] += setData.PWMadjustUnit;
				keyEvent = i+1;//�а������±�־
			}
			if(i==4 && status==KEY_LONG){
				Key_Buf[i].Status.KEY_COUNT = 29;//���ڼӼ��ٶȣ�ҪС��KEY_LONG_DOWN_DELAY
				if(nowIndex>=5 && nowIndex<=8) 
				{	
					menuEvent[0]=1;//�˵��¼�
					menuEvent[1]=NUM_down; //����CH4Left	����ֵ-��
				}
			}
			if(i==5 && status==KEY_LONG){
				Key_Buf[i].Status.KEY_COUNT = 29;//���ڼӼ��ٶȣ�ҪС��KEY_LONG_DOWN_DELAY
				if(nowIndex>=5 && nowIndex<=8) 
				{	
					menuEvent[0]=1;//�˵��¼�
					menuEvent[1]=NUM_up; //����CH4Right		����ֵ+��
				}
			}
			if(i==0 && status==KEY_DOWN && nowIndex>0)
			{
				menuEvent[0]=1;//�˵��¼�
				menuEvent[1]=KEY_home; //����CH1Left	��home��
			}
			if(i==2 && status==KEY_DOWN && nowIndex>0)
			{
				menuEvent[0]=1;//�˵��¼�
				menuEvent[1]=KEY_enter; //����CH2Down	��ȷ����
			}
			if(i==3 && status==KEY_DOWN && nowIndex>0)
			{
				menuEvent[0]=1;//�˵��¼�
				menuEvent[1]=KEY_esc; //����CH2Up		�����ء�
			}
			if(i==4 && status==KEY_DOWN && nowIndex>0)
			{
				menuEvent[0]=1;//�˵��¼�
				menuEvent[1]=NUM_down; //����CH4Left	����ֵ-��
			}
			if(i==5 && status==KEY_DOWN && nowIndex>0)
			{
				menuEvent[0]=1;//�˵��¼�
				menuEvent[1]=NUM_up; //����CH4Right		����ֵ+��
			}
			if(i==6 && status==KEY_DOWN)
			{
				menuEvent[0]=1;//�˵��¼�
				menuEvent[1]=KEY_enter; //��ת�������̰�ȷ��
			}
			if(i==6 && status==KEY_LONG)
			{
				menuEvent[0]=1;//�˵��¼�
				menuEvent[1]=KEY_home; //��ת����������home
			}
//			if(status!=KEY_NULL) printf("%d,%d\n",i,status);
		}
		
		
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //���TIMx�����жϱ�־ 
	}
}

//������ʼ������
void KEY_Init(void) //IO��ʼ��
{ 
	encoder_Init();//���������ų�ʼ��
	Key_Init KeyInit[KEY_NUM]=
	{ 
		{GPIO_Mode_IPU, GPIOB, GPIO_Pin_5, RCC_APB2Periph_GPIOB}, 	// ��ʼ������CH1Left	��home��
		{GPIO_Mode_IPU, GPIOB, GPIO_Pin_4, RCC_APB2Periph_GPIOB}, 	// ��ʼ������CH1Right
		{GPIO_Mode_IPU, GPIOB, GPIO_Pin_3, RCC_APB2Periph_GPIOB}, 	// ��ʼ������CH2Down	��ȷ����
		{GPIO_Mode_IPU, GPIOA, GPIO_Pin_15, RCC_APB2Periph_GPIOA}, 	// ��ʼ������CH2Up		�����ء�
		{GPIO_Mode_IPU, GPIOA, GPIO_Pin_12, RCC_APB2Periph_GPIOA}, 	// ��ʼ������CH4Left	����ֵ-��
		{GPIO_Mode_IPU, GPIOA, GPIO_Pin_11, RCC_APB2Periph_GPIOA}, 	// ��ʼ������CH4Right	����ֵ+��
		{GPIO_Mode_IPU, GPIOB, GPIO_Pin_11, RCC_APB2Periph_GPIOB}, 	// ��ʼ����ת������SW
	};
	Creat_Key(KeyInit); // ���ð�����ʼ������
	
	//STM32û�г����ͷ�PB3��Ϊ��ͨIO��ʹ�ã��л���SW���Կ��ͷ�PB3��PB4��PA15
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	DBGMCU_CR &=0xFFFFFFDF;  //���û����δ��룬PB3�ͻ�һֱ�ǵ͵�ƽ
}

static BitAction KEY_ReadPin(Key_Init Key) //������ȡ����
{
  return (BitAction)GPIO_ReadInputDataBit(Key.GPIOx,Key.GPIO_Pin_x);
}

void Creat_Key(Key_Init* Init)
{
	uint8_t i; 
	GPIO_InitTypeDef  GPIO_InitStructure[KEY_NUM];
  	for(i = 0;i < KEY_NUM;i++)
	{
		Key_Buf[i].Key = Init[i]; // ��ť����ĳ�ʼ�����Ը�ֵ
		RCC_APB2PeriphClockCmd(Key_Buf[i].Key.RCC_APB2Periph_GPIOx, ENABLE);//ʹ����Ӧʱ��
		GPIO_InitStructure[i].GPIO_Pin = Key_Buf[i].Key.GPIO_Pin_x;	//�趨����			
		GPIO_InitStructure[i].GPIO_Mode = Key_Buf[i].Key.GPIO_Mode; 	//�趨ģʽ		
		GPIO_Init(Key_Buf[i].Key.GPIOx, &GPIO_InitStructure[i]);       //��ʼ������
		// ��ʼ����ť�����״̬������
		Key_Buf[i].Status.KEY_SHIELD = ENABLE;
		Key_Buf[i].Status.KEY_COUNT = 0;
		Key_Buf[i].Status.KEY_LEVEL = Bit_RESET;
		if(Key_Buf[i].Key.GPIO_Mode == GPIO_Mode_IPU) // ����ģʽ���и�ֵ
			Key_Buf[i].Status.KEY_DOWN_LEVEL = Bit_RESET;
		else
			Key_Buf[i].Status.KEY_DOWN_LEVEL = Bit_SET;
		Key_Buf[i].Status.KEY_STATUS = KEY_NULL;
		Key_Buf[i].Status.KEY_EVENT = KEY_NULL;
		Key_Buf[i].Status.READ_PIN = KEY_ReadPin;	//��ֵ������ȡ����
	}
}

static void Get_Key_Level(void) // ����ʵ�ʰ��°�ť�ĵ�ƽȥ�������������Ľ��
{
    uint8_t i;
    
    for(i = 0;i < KEY_NUM;i++)
    {
        if(Key_Buf[i].Status.KEY_SHIELD == DISABLE)
            continue;
        if(Key_Buf[i].Status.READ_PIN(Key_Buf[i].Key) == Key_Buf[i].Status.KEY_DOWN_LEVEL)
            Key_Buf[i].Status.KEY_LEVEL = Bit_SET;
        else
            Key_Buf[i].Status.KEY_LEVEL = Bit_RESET;
    }
}

void ReadKeyStatus(void)
{
    uint8_t i;
	
    Get_Key_Level();
	
    for(i = 0;i < KEY_NUM;i++)
    {
        switch(Key_Buf[i].Status.KEY_STATUS)
        {
            //״̬0��û�а�������
            case KEY_NULL:
                if(Key_Buf[i].Status.KEY_LEVEL == Bit_SET)//�а�������
                {
                    Key_Buf[i].Status.KEY_STATUS = KEY_SURE;//ת��״̬1
					Key_Buf[i].Status.KEY_EVENT = KEY_NULL;//���¼�
                }
                else
                {
                    Key_Buf[i].Status.KEY_EVENT = KEY_NULL;//���¼�
                }
                break;
            //״̬1����������ȷ��
            case KEY_SURE:
                if(Key_Buf[i].Status.KEY_LEVEL == Bit_SET)//ȷ�Ϻ��ϴ���ͬ
                {
                    Key_Buf[i].Status.KEY_STATUS = KEY_DOWN;//ת��״̬2
					Key_Buf[i].Status.KEY_EVENT = KEY_DOWN;//�����¼�
                    Key_Buf[i].Status.KEY_COUNT = 0;//����������
                }
                else
                {
                    Key_Buf[i].Status.KEY_STATUS = KEY_NULL;//ת��״̬0
                    Key_Buf[i].Status.KEY_EVENT = KEY_NULL;//���¼�
                }
                break;
            //״̬2����������
            case KEY_DOWN:
                if(Key_Buf[i].Status.KEY_LEVEL != Bit_SET)//�����ͷţ��˿ڸߵ�ƽ
                {
                    Key_Buf[i].Status.KEY_STATUS = KEY_NULL;//ת��״̬0
                    Key_Buf[i].Status.KEY_EVENT = KEY_UP;//�ɿ��¼�
                }
                else if((Key_Buf[i].Status.KEY_LEVEL == Bit_SET)
					&& (++Key_Buf[i].Status.KEY_COUNT >= KEY_LONG_DOWN_DELAY)) //����KEY_LONG_DOWN_DELAYû���ͷ�
                {
                    Key_Buf[i].Status.KEY_STATUS = KEY_LONG;//ת��״̬3
                    Key_Buf[i].Status.KEY_EVENT = KEY_LONG;//�����¼�
					Key_Buf[i].Status.KEY_COUNT = 0;//����������
                }
                else
                {
                    Key_Buf[i].Status.KEY_EVENT = KEY_NULL;//���¼�
                }
                break;
            //״̬3��������������
            case KEY_LONG:
                if(Key_Buf[i].Status.KEY_LEVEL != Bit_SET)//�����ͷţ��˿ڸߵ�ƽ
                {
                    Key_Buf[i].Status.KEY_STATUS = KEY_NULL;//ת��״̬0
                    Key_Buf[i].Status.KEY_EVENT = KEY_UP;//�ɿ��¼�
                }
                else if((Key_Buf[i].Status.KEY_LEVEL == Bit_SET) 
                && (++Key_Buf[i].Status.KEY_COUNT >= KEY_LONG_DOWN_DELAY)) //����KEY_LONG_DOWN_DELAYû���ͷ�
                {
                    Key_Buf[i].Status.KEY_EVENT = KEY_LONG;//�����¼�
                    Key_Buf[i].Status.KEY_COUNT = 0;//����������
                }
                else
                {
                    Key_Buf[i].Status.KEY_EVENT = KEY_NULL;//���¼�
                }
                break;
			default:
				break;
        }
	}
}
//��ת������CLK,DT���ų�ʼ��
void encoder_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PC�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_10; //PB1��PB10����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); //��ʼ�� GPIOB
	
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//�ⲿ�жϣ���Ҫʹ��AFIOʱ��
	
	//GPIOB1 �ж����Լ��жϳ�ʼ������
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line1;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//�����ش���
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

		
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn; //ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;		//�����ȼ�1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 
}

//�жϷ������������ת����������ת����
void EXTI1_IRQHandler(void)
{
	delay_ms(1);	//����������Ҫ
	menuEvent[0]=1;//�˵��¼�
	if(BM_CLK==1 && BM_DT==1) menuEvent[1]=BM_up; //˳ʱ����ת
	if(BM_CLK==1 && BM_DT==0) menuEvent[1]=BM_down; //��ʱ����ת
 	EXTI_ClearITPendingBit(EXTI_Line1);    //���LINE1�ϵ��жϱ�־λ 
}
