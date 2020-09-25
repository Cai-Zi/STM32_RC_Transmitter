/* USER CODE BEGIN Header */
/*
Pin	PINs		FUNCTIONs	LABELs
2	PF0-OSC_IN	GPIO_Output	LED
3	PF1-OSC_OUT	GPIO_EXTI1	KEY
6	PA0			TIM2_CH1	PWM1
7	PA1			TIM2_CH2	PWM2
8	PA2			TIM2_CH3	PWM3
9	PA3			TIM2_CH4	PWM4
10	PA4			GPIO_Output	NRF_CE
11	PA5			GPIO_EXTI5	NRF_IRQ
12	PA6			TIM3_CH1	PWM5
13	PA7			TIM3_CH2	PWM6
14	PB0			TIM3_CH3	PWM7
15	PB1			TIM3_CH4	PWM8
18	PA8			TIM1_CH1	PPM
19	PA9			USART1_TX	TX
20	PA10		USART1_RX	RX
21	PA13		SYS_SWDIO	
22	PA14		SYS_SWCLK	
23	PA15		GPIO_Output	NRF_CSN
24	PB3			SPI1_SCK	NRF_SCK
25	PB4			SPI1_MISO	NRF_MISO
26	PB5			SPI1_MOSI	NRF_MOSI
27	PB6			I2C1_SCL	EEPROM_SCL
28	PB7			I2C1_SDA	EEPROM_SDA
*/
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "nrf24L01.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
u8 update_count = 0;
u8 PPM_count=0;
uint16_t CH_Safe[16];			//ϵͳ�����ʧ�ر���ͨ��ֵ
uint32_t CH_PPM[9];				//�û�����������DMA����PPM�������Ҫ8�����һ�����ڼ�������
u16 PWMvalue[SBUS_CHANNEL_NUMBER];// ����PWMռ�ձ�
u16 recPWMvalue[SBUS_CHANNEL_NUMBER];// ����PWMռ�ձ�
u8 sbusPacket[SBUS_PACKET_LENGTH];// 25���ֽڵ�SBUS���ݰ�
u8 signalLoss = 0;  // 1��ʾ��ʧ�ź�
u16 i=0,startIndex=0;
u32 nowTime = 0;
u32 lastTime = 0;
u32 sbusTime = 0;
u8 chPacket[32];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
HAL_StatusTypeDef My_HAL_TIM_PWM_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_TIM17_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	HAL_Delay(500);
	//������������Ҫ��EEPROM�ж�ȡ������Ϣ������ȷ������ģ�����������ģʽ��ʧ�ر����ȡ�
	
	//��������ģ��
	NRF24L01_Init();
	
	//�����ֵ�Ƿ������д�ģ�ʵ��ʹ�ô�EEPROM��ȡ
	for(i = 0; i<16; i ++) {CH_Safe[i]=1500;}
	CH_Safe[2] = 1000;		//����ͨ�����
	
	//��ʼ��ͨ��ֵ����ʧ�ر���ֵΪ׼
	for(i = 0; i<16; i ++) {PWMvalue[i] = CH_Safe[i];}
	
	//PPM
	for(i = 0; i<8; i ++) {CH_PPM[i] = PWMvalue[i]-500;}//PWM�ߵ�ƽʱ��-500us�ĵ͵�ƽʱ��
	CH_PPM[8] = 20000 - PWMvalue[0] - PWMvalue[1] - PWMvalue[2] - PWMvalue[3] - PWMvalue[4] - PWMvalue[5] - PWMvalue[6] - PWMvalue[7];
	My_HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, CH_PPM, 9);
	
	//PWM
	TIM2->CCR1 = PWMvalue[0];
	TIM2->CCR2 = PWMvalue[1];
	TIM2->CCR3 = PWMvalue[2];
	TIM2->CCR4 = PWMvalue[3];
	TIM3->CCR1 = PWMvalue[4];
	TIM3->CCR2 = PWMvalue[5];
	TIM3->CCR3 = PWMvalue[6];
	TIM3->CCR4 = PWMvalue[7];
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
	
	HAL_TIM_Base_Start_IT(&htim17);

	while(NRF24L01_Check())
	{
 		HAL_Delay(1000);
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);//��˸LED
	}
	NRF24L01_RX_Mode();


  while (1)
  {
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	if(NRF24L01_RxPacket(chPacket)==0)
	{
		for(i=0;i<32-chNum*2;i++)//Ѱ����ʼλ�ã�һ���ڵ�2-3��Ԫ��
		{
			if(chPacket[i]==0x00 & chPacket[i+1]==0x00) 
			{
				startIndex = i+2;
				break;
			}
		}
		for(i=0;i<chNum;i++)//����ʼλ�ÿ�ʼ����
		{
			recPWMvalue[i] = ((u16)chPacket[startIndex] << 8) | ((u16)(chPacket[startIndex+1]));// �ϲ�u8Ϊu16
			if(recPWMvalue[i]!=0) PWMvalue[i] = recPWMvalue[i];
			startIndex = startIndex+2;
		}
		for (i=chNum; i<16; i++) 
		{
			PWMvalue[i] = 1500;//δ�õ���ͨ��ȫ������
		}
		//printf("%d,%d,%d,%d\n",PWMvalue[0],PWMvalue[1],PWMvalue[2],PWMvalue[3]);
		LED_LOW();
		lastTime = nowTime;
	}
	if(nowTime-lastTime>2000)//�����ϴν���ʱ�����2s����˵��ʧȥ�ź�
	{
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);//��˸LED
		signalLoss = 1;//ʧȥ�źű�־
		PWMvalue[0] = 1500;//ͨ��1����
		PWMvalue[1] = 1500;//ͨ��2����
		PWMvalue[2] = 1000;//�������
		PWMvalue[3] = 1500;//ͨ��4����
		HAL_Delay(200);
	}
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	//��ʱ��17����ϵͳ��ʱ��������Ƶ��Ϊ1000Hz���ṩϵͳ����1msһ��
	if (htim->Instance == htim17.Instance)
	{
		nowTime ++;//��ϵͳ�����������ۼ�
		
		update_count++;
		if(update_count == 100) {update_count = 0;}
		//���ݶ�ȡ����ģ���ͨ���������PWM��10ms����һ�Ρ�
		if(update_count % 10 == 0)
		{
			TIM2->CCR1 = PWMvalue[0];
			TIM2->CCR2 = PWMvalue[1];
			TIM2->CCR3 = PWMvalue[2];
			TIM2->CCR4 = PWMvalue[3];
			TIM3->CCR1 = PWMvalue[4];
			TIM3->CCR2 = PWMvalue[5];
			TIM3->CCR3 = PWMvalue[6];
			TIM3->CCR4 = PWMvalue[7];
		}
		if(update_count % SBUS_UPDATE_RATE == 0)
		{
			//���S.bus��10ms����һ��
			sbusPreparePacket(sbusPacket, PWMvalue, signalLoss, 0); //chNumͨ����ֵת��ΪSBUS���ݰ�
			HAL_UART_Transmit_IT(&huart1, (uint8_t *)sbusPacket, sizeof(sbusPacket));
		}
		
		//����Ӧ���ڰ���ʱ��¼��ǰ Sys_count �����Ҽ������һ�����ֵ��
		//�����������Sys_count ������ֵʱ�����жϴ���
		/*if((Sys_count == check_value) && (Key_Status == Key_Mode))��������*/
	}
}
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	for(uint8_t i = 0; i != 8; i ++) {CH_PPM[i] = PWMvalue[i];}
	CH_PPM[8] = 20000 - PWMvalue[0] - PWMvalue[1] - PWMvalue[2] - PWMvalue[3] - PWMvalue[4] - PWMvalue[5] - PWMvalue[6] - PWMvalue[7];
}

//�ı�ԭ�п⺯������DMAĿ��Ĵ�����ΪARR��
HAL_StatusTypeDef My_HAL_TIM_PWM_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length)
{
  uint32_t tmpsmcr;

  /* Check the parameters */
  assert_param(IS_TIM_CCX_INSTANCE(htim->Instance, Channel));

  if (htim->State == HAL_TIM_STATE_BUSY)
  {
    return HAL_BUSY;
  }
  else if (htim->State == HAL_TIM_STATE_READY)
  {
    if ((pData == NULL) && (Length > 0U))
    {
      return HAL_ERROR;
    }
    else
    {
      htim->State = HAL_TIM_STATE_BUSY;
    }
  }
  else
  {
    /* nothing to do */
  }

  switch (Channel)
  {
    case TIM_CHANNEL_1:
    {
      /* Set the DMA compare callbacks */
      htim->hdma[TIM_DMA_ID_CC1]->XferCpltCallback = TIM_DMADelayPulseCplt;
      htim->hdma[TIM_DMA_ID_CC1]->XferHalfCpltCallback = TIM_DMADelayPulseHalfCplt;

      /* Set the DMA error callback */
      htim->hdma[TIM_DMA_ID_CC1]->XferErrorCallback = TIM_DMAError ;

      /* Enable the DMA channel */
      if (HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC1], (uint32_t)pData, (uint32_t)&htim->Instance->ARR, Length) != HAL_OK)
      {
        return HAL_ERROR;
      }

      /* Enable the TIM Capture/Compare 1 DMA request */
      __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC1);
      break;
    }

    case TIM_CHANNEL_2:
    {
      /* Set the DMA compare callbacks */
      htim->hdma[TIM_DMA_ID_CC2]->XferCpltCallback = TIM_DMADelayPulseCplt;
      htim->hdma[TIM_DMA_ID_CC2]->XferHalfCpltCallback = TIM_DMADelayPulseHalfCplt;

      /* Set the DMA error callback */
      htim->hdma[TIM_DMA_ID_CC2]->XferErrorCallback = TIM_DMAError ;

      /* Enable the DMA channel */
      if (HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC2], (uint32_t)pData, (uint32_t)&htim->Instance->CCR2, Length) != HAL_OK)
      {
        return HAL_ERROR;
      }
      /* Enable the TIM Capture/Compare 2 DMA request */
      __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC2);
      break;
    }

    case TIM_CHANNEL_3:
    {
      /* Set the DMA compare callbacks */
      htim->hdma[TIM_DMA_ID_CC3]->XferCpltCallback = TIM_DMADelayPulseCplt;
      htim->hdma[TIM_DMA_ID_CC3]->XferHalfCpltCallback = TIM_DMADelayPulseHalfCplt;

      /* Set the DMA error callback */
      htim->hdma[TIM_DMA_ID_CC3]->XferErrorCallback = TIM_DMAError ;

      /* Enable the DMA channel */
      if (HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC3], (uint32_t)pData, (uint32_t)&htim->Instance->CCR3, Length) != HAL_OK)
      {
        return HAL_ERROR;
      }
      /* Enable the TIM Output Capture/Compare 3 request */
      __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC3);
      break;
    }

    case TIM_CHANNEL_4:
    {
      /* Set the DMA compare callbacks */
      htim->hdma[TIM_DMA_ID_CC4]->XferCpltCallback = TIM_DMADelayPulseCplt;
      htim->hdma[TIM_DMA_ID_CC4]->XferHalfCpltCallback = TIM_DMADelayPulseHalfCplt;

      /* Set the DMA error callback */
      htim->hdma[TIM_DMA_ID_CC4]->XferErrorCallback = TIM_DMAError ;

      /* Enable the DMA channel */
      if (HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC4], (uint32_t)pData, (uint32_t)&htim->Instance->CCR4, Length) != HAL_OK)
      {
        return HAL_ERROR;
      }
      /* Enable the TIM Capture/Compare 4 DMA request */
      __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC4);
      break;
    }

    default:
      break;
  }

  /* Enable the Capture compare channel */
  TIM_CCxChannelCmd(htim->Instance, Channel, TIM_CCx_ENABLE);

  if (IS_TIM_BREAK_INSTANCE(htim->Instance) != RESET)
  {
    /* Enable the main output */
    __HAL_TIM_MOE_ENABLE(htim);
  }

  /* Enable the Peripheral, except in trigger mode where enable is automatically done with trigger */
  tmpsmcr = htim->Instance->SMCR & TIM_SMCR_SMS;
  if (!IS_TIM_SLAVEMODE_TRIGGER_ENABLED(tmpsmcr))
  {
    __HAL_TIM_ENABLE(htim);
  }

  /* Return function status */
  return HAL_OK;
}

/*PPM���*/
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == htim1.Instance)
	{
		TIM1->ARR = CH_PPM[PPM_count];
		PPM_count ++;
		if(PPM_count == 8)
		{
			PPM_count = 0;
			//HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
			//HAL_TIM_Base_Stop(&htim1);
		}
	}
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
