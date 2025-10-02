/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "stm32f4xx.h"
#include "lcd_stm32f4.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// TODO: Add values for below variables
#define NS 128       // Number of samples in LUT
#define TIM2CLK  16000000 // STM Clock frequency: Hint You might want to check the ioc file
#define F_SIGNAL 128000 	// Frequency of output analog signal

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
DMA_HandleTypeDef hdma_tim2_ch1;

/* USER CODE BEGIN PV */
// TODO: Add code for global variables, including LUTs
uint32_t Sin_LUT[NS] = {2048, 2148, 2248, 2348, 2447, 2545, 2642, 2737,
	    2831, 2923, 3013, 3100, 3185, 3267, 3346, 3423,
	    3495, 3565, 3630, 3692, 3750, 3804, 3853, 3898,
	    3939, 3975, 4007, 4034, 4056, 4073, 4085, 4093,
	    4095, 4093, 4085, 4073, 4056, 4034, 4007, 3975,
	    3939, 3898, 3853, 3804, 3750, 3692, 3630, 3565,
	    3495, 3423, 3346, 3267, 3185, 3100, 3013, 2923,
	    2831, 2737, 2642, 2545, 2447, 2348, 2248, 2148,
	    2048, 1947, 1847, 1747, 1648, 1550, 1453, 1358,
	    1264, 1172, 1082, 995, 910, 828, 749, 672,
	    600, 530, 465, 403, 345, 291, 242, 197,
	    156, 120, 88, 61, 39, 22, 10, 2,
	    0, 2, 10, 22, 39, 61, 88, 120,
	    156, 197, 242, 291, 345, 403, 465, 530,
	    600, 672, 749, 828, 910, 995, 1082, 1172,
	    1264, 1358, 1453, 1550, 1648, 1747, 1847, 1947};
uint32_t Saw_LUT[NS] = {0, 32, 64, 96, 128, 160, 192, 224,
	    256, 288, 320, 352, 384, 416, 448, 480,
	    512, 544, 576, 608, 640, 672, 704, 736,
	    768, 800, 832, 864, 896, 928, 960, 992,
	    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248,
	    1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504,
	    1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760,
	    1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,
	    2048, 2079, 2111, 2143, 2175, 2207, 2239, 2271,
	    2303, 2335, 2367, 2399, 2431, 2463, 2495, 2527,
	    2559, 2591, 2623, 2655, 2687, 2719, 2751, 2783,
	    2815, 2847, 2879, 2911, 2943, 2975, 3007, 3039,
	    3071, 3103, 3135, 3167, 3199, 3231, 3263, 3295,
	    3327, 3359, 3391, 3423, 3455, 3487, 3519, 3551,
	    3583, 3615, 3647, 3679, 3711, 3743, 3775, 3807,
	    3839, 3871, 3903, 3935, 3967, 3999, 4031, 4063};
uint32_t Triangle_LUT[NS] = {0, 64, 128, 192, 256, 320, 384, 448,
	    512, 576, 640, 704, 768, 832, 896, 960,
	    1024, 1088, 1152, 1216, 1280, 1344, 1408, 1472,
	    1536, 1600, 1664, 1728, 1792, 1856, 1920, 1984,
	    2048, 2111, 2175, 2239, 2303, 2367, 2431, 2495,
	    2559, 2623, 2687, 2751, 2815, 2879, 2943, 3007,
	    3071, 3135, 3199, 3263, 3327, 3391, 3455, 3519,
	    3583, 3647, 3711, 3775, 3839, 3903, 3967, 4031,
	    4095, 4031, 3967, 3903, 3839, 3775, 3711, 3647,
	    3583, 3519, 3455, 3391, 3327, 3263, 3199, 3135,
	    3071, 3007, 2943, 2879, 2815, 2751, 2687, 2623,
	    2559, 2495, 2431, 2367, 2303, 2239, 2175, 2111,
	    2048, 1984, 1920, 1856, 1792, 1728, 1664, 1600,
	    1536, 1472, 1408, 1344, 1280, 1216, 1152, 1088,
	    1024, 960, 896, 832, 768, 704, 640, 576,
	    512, 448, 384, 320, 256, 192, 128, 64};
uint32_t Piano_LUT = {1954, 2197, 1809, 2069, 1978, 2018, 1946, 2155,
	    1987, 2028, 1996, 1994, 1830, 2726, 2013, 1335,
	    1943, 1951, 1860, 1981, 1860, 2069, 821, 1947,
	    2569, 1948, 4095, 963, 1358, 1493, 2417, 2426,
	    802, 2297, 1726, 2002, 1906, 2026, 1350, 1634,
	    1740, 1987, 2404, 1351, 2101, 1640, 3752, 1933,
	    2048, 1991, 1961, 1929, 1785, 1935, 3589, 1701,
	    2805, 1379, 1723, 1834, 0, 1646, 1067, 1966,
	    1146, 1910, 2213, 1980, 1920, 1886, 1537, 2138,
	    1774, 2024, 1298, 2907, 2364, 1909, 2606, 2378,
	    1963, 2086, 1889, 1976, 1967, 1925, 2375, 1992,
	    1988, 1676, 1537, 1913, 1800, 2383, 1982, 2135,
	    2743, 1920, 2086, 1992, 2030, 1939, 1131, 1551,
	    1699, 1921, 2210, 2151, 1872, 1894, 1478, 2272,
	    1925, 2024, 1825, 1980, 2027, 1947, 1687, 1623,
	    1020, 1016, 2062, 2312, 1941, 2033, 1825, 1954};
uint32_t Guitar_LUT = {1276, 1960, 2368, 1662, 2006, 916, 628, 2877,
	    1168, 1500, 1346, 1212, 1343, 1811, 284, 1326,
	    1259, 1372, 362, 2711, 897, 899, 2853, 721,
	    266, 1255, 1185, 1299, 1243, 1206, 985, 1317,
	    1293, 1092, 1128, 1580, 680, 1733, 1064, 1238,
	    790, 1326, 1326, 1327, 1306, 883, 1585, 1255,
	    696, 810, 2061, 664, 2170, 1035, 990, 2213,
	    1148, 1677, 1418, 357, 1010, 1600, 4095, 1083,
	    672, 1531, 1607, 1991, 1862, 2381, 201, 2314,
	    1397, 1180, 1380, 1326, 2327, 827, 1325, 1303,
	    1642, 1684, 392, 1610, 1392, 0, 1223, 194,
	    1447, 1397, 1245, 1234, 1062, 812, 1425, 1274,
	    2448, 549, 2296, 615, 1633, 679, 2288, 2714,
	    1072, 1285, 1169, 1298, 1593, 814, 1300, 1272,
	    31, 1727, 1328, 1112, 1932, 1115, 1039, 212,
	    1421, 453, 2376, 894, 60, 3126, 1412, 1269};
uint32_t Drum_LUT = {2036, 3354, 1870, 2112, 2057, 3358, 1902, 1932,
	    2076, 2517, 2017, 2016, 2023, 2011, 1444, 1838,
	    2075, 2021, 2034, 2046, 2037, 2095, 2030, 2014,
	    2096, 1828, 2055, 2041, 2205, 2006, 2945, 1243,
	    1701, 2095, 2420, 2095, 1924, 2046, 2017, 2024,
	    4086, 1845, 2026, 2030, 2364, 2328, 1816, 1989,
	    2336, 2051, 2004, 2036, 630, 2409, 2074, 2063,
	    0, 2213, 2027, 2014, 2166, 2037, 2905, 2383,
	    4084, 1970, 2266, 2021, 2600, 2686, 2051, 2089,
	    1652, 2000, 2079, 2034, 2603, 1863, 2034, 2063,
	    1984, 2045, 2028, 2036, 2071, 2040, 2064, 2035,
	    2904, 2052, 2008, 2066, 2021, 2042, 1766, 2026,
	    760, 2265, 1957, 2024, 1999, 2029, 2083, 2029,
	    1663, 2046, 2050, 2019, 2040, 2402, 2144, 2220,
	    2119, 2017, 2027, 1710, 2127, 2408, 2121, 4095,
	    2136, 2000, 2030, 1619, 1963, 507, 2508, 2036};





// TODO: Equation to calculate TIM2_Ticks
uint32_t TIM2_Ticks = TIM2CLK/(NS*F_SIGNAL); // How often to write new LUT value
uint32_t DestAddress = (uint32_t) &(TIM3->CCR3); // Write LUT TO TIM3->CCR3 to modify PWM duty cycle


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
void EXTI0_IRQHandler(void);
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
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  // TODO: Start TIM3 in PWM mode on channel 3

  // TODO: Start TIM2 in Output Compare (OC) mode on channel 1

  // TODO: Start DMA in IT mode on TIM2->CH1. Source is LUT and Dest is TIM3->CCR3; start with Sine LUT

  // TODO: Write current waveform to LCD(Sine is the first waveform)

  // TODO: Enable DMA (start transfer from LUT to CCR)

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = TIM2_Ticks - 1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */
  /* TIM2_CH1 DMA Init */
  __HAL_RCC_DMA1_CLK_ENABLE();

  hdma_tim2_ch1.Instance = DMA1_Stream5;
  hdma_tim2_ch1.Init.Channel = DMA_CHANNEL_3;         // TIM2_CH1 is on channel 3
  hdma_tim2_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH; // Memory -> TIM3->CCR3
  hdma_tim2_ch1.Init.PeriphInc = DMA_PINC_DISABLE;    // Peripheral address fixed
  hdma_tim2_ch1.Init.MemInc = DMA_MINC_ENABLE;        // Memory address increments
  hdma_tim2_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_tim2_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  hdma_tim2_ch1.Init.Mode = DMA_CIRCULAR;            // Repeat LUT automatically
  hdma_tim2_ch1.Init.Priority = DMA_PRIORITY_HIGH;
  hdma_tim2_ch1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

  if (HAL_DMA_Init(&hdma_tim2_ch1) != HAL_OK)
  {
      Error_Handler();
  }

  /* Link DMA handle to TIM2 handle */
  __HAL_LINKDMA(&htim2, hdma[TIM_DMA_ID_CC1], hdma_tim2_ch1);
  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  // -------------------------------
  // LCD pins configuration
  // -------------------------------
  // Configure PC14 (RS) and PC15 (E) as output push-pull
  GPIO_InitStruct.Pin = GPIO_PIN_14 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  // Configure PB8 (D4) and PB9 (D5) as output push-pull
  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // Configure PA12 (D6) and PA15 (D7) as output push-pull
  GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Set all LCD pins LOW initially
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12 | GPIO_PIN_15, GPIO_PIN_RESET);


  // -------------------------------
  // Button0 configuration (PA0)
  // -------------------------------
  GPIO_InitStruct.Pin = Button0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; // Interrupt on rising edge
  GPIO_InitStruct.Pull = GPIO_PULLUP;         // Use pull-up resistor
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Enable and set EXTI line 0 interrupt priority
  HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void EXTI0_IRQHandler(void){

	// TODO: Debounce using HAL_GetTick()


	// TODO: Disable DMA transfer and abort IT, then start DMA in IT mode with new LUT and re-enable transfer
	// HINT: Consider using C's "switch" function to handle LUT changes




	HAL_GPIO_EXTI_IRQHandler(Button0_Pin); // Clear interrupt flags
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
