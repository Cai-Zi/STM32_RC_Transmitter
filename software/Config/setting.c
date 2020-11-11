#include "setting.h"
#include "main.h"

volatile set_Config setData;
volatile u32 setDataSize=sizeof(setData)/2;
//�ָ�Ĭ������
void resetData(void)
{
	for(int i=0;i<chNum;i++)
	{
		setData.chLower[i] 	= 0;	//ң�˵���Сֵ
		setData.chMiddle[i] = 2047;	//ң�˵���ֵ
		setData.chUpper[i] 	= 4095;	//ң�˵����ֵ
		setData.PWMadjustValue[i]=0;//΢��ֵ
		setData.chReverse[i] = 1;	//ͨ����������1Ϊ������0Ϊ��ת
	}
	setData.PWMadjustUnit = 2;//΢����λ
	setData.warnBatVolt = 3.7;//������ѹ
	setData.throttlePreference = 1;//��������
	setData.batVoltAdjust = 1000;//��ص�ѹУ׼ֵ
	setData.modelType = 0;//ģ�����ͣ���0����1����2
}
void set_Init(void)
{
	STMFLASH_Read(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//��FLASH�ж�ȡ�ṹ��
	if(setData.writeFlag!=0x1111){
		setData.writeFlag=0x1111;//�Ƿ��һ��д��
		resetData();
		STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//д��FLASH
	}
}

