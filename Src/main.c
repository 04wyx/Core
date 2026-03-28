/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "math.h"
#include "arm_math.h"
#include "arm_const_structs.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

int find_peak(void);
void FFT_test(void);
void DataSolve_Same(void);
void DataSolve_Different(void);
float Find_Wave_3(void);
int wave_set(int index);


#define NPT 1024
#define range 2

uint16_t ADC_Value[NPT]	= {0};

uint8_t adcflag = 0,wave_flag = 0;

float FFT_in[ NPT*2 ] = {0};
float FFT_out[ NPT/2 ];
float ADC_Float[ NPT ] = {0};
float FFT_Out_3 = 0.0;

int based_wave1_state = 0;
int based_wave2_state = 0;  //1-----正弦波；2-----三角波

int wave1_index = 1;
int wave2_index = 1;

uint32_t wave1_Freq = 0;
uint32_t wave2_Freq = 0;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	printf("START\r\n");
	HAL_TIM_Base_Start(&htim3);
	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&ADC_Value,NPT);	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		if(adcflag==1)
		{
       //adc标志位、波形频率位初始化
 			adcflag=0;
 			wave_flag=0;

       //波形的频率索引、类型初始化
 			wave1_index=1;
 			wave2_index=1;
 			based_wave1_state=0;
 			based_wave2_state=0;

 			HAL_TIM_Base_Stop(&htim3);
 			HAL_ADC_Stop_DMA(&hadc1);
			
 			FFT_test(); //FFT变换并计算幅值
			
 			printf("\r\nFFT\r\n");	//输出FFT结果

 			for(int i=0;i < NPT / 2;i++)
 			{
 				printf("%.1f , ",FFT_out[i]);
 			}			

 			printf("\r\nFFT_OVER\r\n");				
			
 			wave_flag = find_peak();

 /*=============================两者频率相同================================*/
 			if( wave_flag == 0)     
 			{
         printf("same\r\n");
 				DataSolve_Same();
 				wave2_index = wave1_index;
 				if(20 <= wave1_index && wave1_index <= 100)  //****************频率小于100kHz*************
 				{
           FFT_Out_3 = Find_Wave_3();
           printf("FFT_Out_3 = %.1f\r\n", FFT_Out_3);
 					if(FFT_Out_3 > 30000)  //FFT_out[wave1_index * 5] > 20000  //三角+三角
 					{
 						based_wave1_state = based_wave2_state=2; 
 					}
 					else if(FFT_Out_3 > 15000) //FFT_out[wave1_index * 5] > 9000 //正弦+三角
 					{
 						based_wave1_state = 1;
 						based_wave2_state = 2;
 					}
 					else
 					{
 						based_wave1_state = based_wave2_state = 1;  //正弦+正弦
 					}
 				}
 				else //***********************频率大于100kHz或者频率小于20kHz*************************
 				{
 					printf("wave_same wrong \r\n");
 				}
 			}
// /*=============================普遍情况，两者频率不相同================================*/
// 			else     
// 			{
// 				DataSolve_Different();
// 				if(wave1_index<180 && wave2_index<180)  //********************频率均小于90kHz*******************
// 				{
// 					if(wave2_index!=3*wave1_index)//普遍情况 不是恰好三倍 
// 					{
// 						if( fabs(FFT_out[wave1_index]/9.0f-FFT_out[wave1_index*3])<2000 )
// 						{
// 							based_wave1_state=2;//wave1三角
// 						}
// 						else{based_wave1_state=1;}//wave1正弦
						
// 						if( fabs(FFT_out[wave2_index]/9.0f-FFT_out[wave2_index*3])<2000 )
// 						{
// 							based_wave2_state=2;//wave2三角
// 						}
// 						else{based_wave2_state=1;}//wave2正弦
// 					}
// 					else//恰好三倍 
// 					{
// 						if( fabs(FFT_out[wave2_index]/9.0f-FFT_out[wave2_index*3])<2000 )
// 						{
// 							based_wave2_state=2;//wave2三角
// 						}
// 						else{based_wave2_state=1;}//wave2正弦
						
// 						if( fabs(FFT_out[wave1_index]/25.0f-FFT_out[wave1_index*5])<1000 )
// 						{
// 							based_wave1_state=2;//wave1三角
// 						}
// 						else{based_wave1_state=1;}//wave1正弦
// 					}
// 				}
// 				else if(wave1_index<180 && wave2_index>=180)//**************wave1小于90kHz wave2大于等于90kHz***
// 				{
// 					if(wave2_index!=3*wave1_index)//普遍情况 不是恰好三倍
// 					{
// 						if( fabs(FFT_out[wave1_index]/9.0f-FFT_out[wave1_index*3])<2000 )
// 						{
// 							based_wave1_state=2;//wave1三角
// 						}
// 						else{based_wave1_state=1;}//wave1正弦
// 						if( FFT_out[wave2_index]>290000)
// 						{
// 							based_wave2_state=1;//wave2正弦
// 						}
// 						else{based_wave2_state=2;}//wave2三角
// 					}
// 					else//恰好三倍 
// 					{
// 						if( fabs(FFT_out[wave1_index]/25.0f-FFT_out[wave1_index*5])<1000 )
// 						{
// 							based_wave1_state=2;//wave1三角
// 						}
// 						else{based_wave1_state=1;}//wave1正弦
// 						if( FFT_out[wave2_index]>280000)
// 						{
// 							based_wave2_state=1;//wave2正弦
// 						}
// 						else{based_wave2_state=2;}//wave2三角
// 					}
// 				}
// 				else//**************wave1 wave2均大于等于90kHz***
// 				{
// 					if( FFT_out[wave1_index]>290000)
// 						{
// 							based_wave1_state=1;//wave1正弦
// 						}
// 					else{based_wave1_state=2;}//wave1三角
// 					if( FFT_out[wave2_index]>290000)
// 						{
// 							based_wave2_state=1;//wave2正弦
// 						}
// 					else{based_wave2_state=2;}//wave2三角
// 				}
				
// 			}
			
/*==========================================================*/			
			//1-----正弦波；2-----三角波
			// wave1_Freq = wave1_index * 1000;
			// wave2_Freq = wave2_index * 1000;
			
			wave1_index = wave_set(wave1_index);
			wave2_index = wave_set(wave2_index);
			
      printf("wave1:%d    Fre:%d kHz\r\n",based_wave1_state,wave1_index);
			printf("wave2:%d    Fre:%d kHz\r\n",based_wave2_state,wave2_index);

			HAL_Delay(10000);
			HAL_TIM_Base_Start(&htim3);
			HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&ADC_Value,NPT);
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  if(hadc == &hadc1)
  {
    adcflag = 1; // Handle ADC conversion complete event
  }
}
//查找FFT结果误差范围内最大值
float Find_Wave_3(void)
{
  float FFT_Index_3 = FFT_out[ wave1_index * 3];
  // float FFT_Index_5 = FFT_out[ wave1_index * 5];
  float FFT_Max = FFT_Index_3;
  for(int i = (wave1_index - range) * 3; i < (wave1_index + range) * 3; i++){
    if(FFT_out[i] > FFT_Max)
    {
      FFT_Max = FFT_out[i];
    }
  }
  return FFT_Max;
}
//修正wave_index的偏差
int wave_set(int index)
{
//  if(index < 20)
//  { 
//    index = 20; 
//  }
//  else if(index > 100)
//  { 
//    index = 100; 
//  }
  int corrected_index = index + range;
  int num = corrected_index / 5;
  int index_new = num * 5;
//	printf("%d,%d,%d\r\n",index,num,index_new);
  if(index_new >= (index - range) && index_new <= (index + range))
  {
    return index_new;
  }else{
		return index;
	}
}
/*判断两个信号频率是否一样*/
int find_peak(void)
{
	int count=0;
	for(int i = 20 - range;i <= 100 + range;i++)
	{
		if( (FFT_out[i] > FFT_out[i-1] ) && (FFT_out[i] > FFT_out[i+1] ) && FFT_out[i] > 200000 )
		{
			count++;
//			printf("count = %d\r\n",i);
		}
	}
  printf("wave_flag = %d\r\n",count);
	if(count == 1){ return 0; }  //特殊情况两者频率相同
	
	else{ return 1; }					//普遍情况
}
/*fft——test*/
void FFT_test(void)
{
	printf("ADC \r\n");
	for(int i=0;i<NPT;i++)
	{
		ADC_Float[i]=(float)ADC_Value[i];
		printf("%d ,",ADC_Value[i]);
	}			
	printf("\r\nADC Over\r\n");
	for(int j=0;j < NPT; j++)
	{
		FFT_in[j*2]=ADC_Float[j];
		FFT_in[j*2+1]=0;				
	}			
	arm_cfft_f32(&arm_cfft_sR_f32_len1024,FFT_in,0,1);		 
	arm_cmplx_mag_f32(FFT_in,FFT_out,NPT/2);	
}
/*处理相同频率的信号*/
void DataSolve_Same(void)
{
	printf("/*==========same=============*/\r\n");
	for(int j = 20;j <= 100; j++)
	{
		if(FFT_out[wave1_index] < FFT_out[j]) 
		{
			wave1_index = j;            // 更新最大值索引
		} 
	}
	printf("wave1_index %d \r\n",wave1_index);
}
/*处理不同频率信号*/
void DataSolve_Different(void)
{
	printf("/*=========different==========*/\r\n");
	for(int j = 40; j <= 200; j++) 
	{
		if(FFT_out[wave1_index] < FFT_out[j]) 
		{
			wave2_index = wave1_index;  // 原最大值变成次大值
			wave1_index = j;            // 更新最大值索引
		} 
		else if(FFT_out[wave2_index] < FFT_out[j] && j != wave1_index) 
		{
			wave2_index = j;  // 更新次大值索引
		}	 
	}
	if(wave2_index<wave1_index)//保证wave1索引小于wave2索引
	{
		int t;
		t=wave1_index;
		wave1_index=wave2_index;
		wave2_index=t;
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
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
