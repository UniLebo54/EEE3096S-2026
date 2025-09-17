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
//TODO: Define variables you think you might need
// - Performance timing variables (e.g execution time, throughput, pixels per second, clock cycles)
#define MAX_ITER 420

int image_sizes[] = {128, 160, 192, 224, 256};
// Results storage
uint64_t checksums_fixed[5] = {0};
uint64_t checksums_double[5] = {0};
uint32_t execution_times_fixed[5] = {0}; // Time in ms
uint32_t execution_times_double[5] = {0}; // Time in ms
uint32_t execution_cycles_fixed[5] = {0};   // NEW: Cycles for fixed point
uint32_t execution_cycles_double[5] = {0};  // NEW: Cycles for double
float throughput_fixed_mps[5] = {0};

//Current test variables
uint64_t checksum = 0;
uint32_t start_time = 0;
uint32_t end_time = 0;
uint32_t execution_time = 0;
uint32_t start_cycles = 0;  // NEW: For DWT
uint32_t end_cycles = 0;    // NEW: For DWT
uint32_t delta_cycles = 0;  // NEW: For DWT
uint32_t total_pixels = 0;
float throughput_mps = 0.0f;

// Test state
uint8_t current_size_index = 0;
uint8_t test_type = 0;  // 0 = fixed point, 1 = double
uint8_t test_complete = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
uint64_t calculate_mandelbrot_fixed_point_arithmetic(int width, int height, int max_iterations);
uint64_t calculate_mandelbrot_double(int width, int height, int max_iterations);
void run_all_tests(void);
void EnableDWT(void); // NEW: Function to enable DWT
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// NEW: Function to enable the DWT Cycle Counter
void EnableDWT(void) {
    // Unlock access to the DWT and ITM units (from CoreDebug)
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    // Enable the DWT cycle counter
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

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
  // NEW: Enable the DWT Cycle Counter as soon as system clock is configured
  EnableDWT();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
  run_all_tests();

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 15;
  RCC_OscInitStruct.PLL.PLLN = 144;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB0 PB1 PB2 PB3
                           PB4 PB5 PB6 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
//TODO: Function signatures you defined previously , implement them here

void run_all_tests(void){
	for (int i = 0; i < 5; i++) {
		  int size = image_sizes[i];
		  total_pixels = size * size;
	  //TODO: Visual indicator: Turn on LED0 to signal processing start
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

	  //TODO: Benchmark and Profile Performance for FIXED POINT
	  start_time = HAL_GetTick(); // Get ms timestamp
	  DWT->CYCCNT = 0;               // Reset cycle counter
	  start_cycles = DWT->CYCCNT;    // Read start cycles (should be ~0)
	  checksum = calculate_mandelbrot_fixed_point_arithmetic(size, size, MAX_ITER);
	  end_cycles = DWT->CYCCNT;      // Read end cycles immediately after function returns
	  end_time = HAL_GetTick();   // Get ms timestamp

	  execution_time = end_time - start_time;
	  delta_cycles = end_cycles - start_cycles;
	  if (execution_time > 0) {
		  throughput_mps = (total_pixels * 120000000) / delta_cycles;
	  }
	  else {
		  throughput_mps = 0.0;
	  }


	  checksums_fixed[i] = checksum;
	  execution_times_fixed[i] = execution_time;
	  execution_cycles_fixed[i] = delta_cycles; // Store cycle count
	  throughput_fixed_mps[i] = throughput_mps;

	  //TODO: Visual indicator: Turn on LED1 to signal processing done for fixed point
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	  HAL_Delay(1000); // Keep LED on for 1s
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); // Turn off LED1

	  // TODO: Turn OFF all LEDs before next test
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_RESET);

	  // Delay between test sizes
	  HAL_Delay(100);
	  }
}
uint64_t calculate_mandelbrot_fixed_point_arithmetic(int width, int height, int max_iterations){
    uint64_t checksum = 0;
    const int32_t scale = 1 << 20;
    const int32_t x_scale = (int32_t)(3.5 * scale);
    const int32_t y_scale = (int32_t)(2.0 * scale);
    const int32_t x_offset = (int32_t)(2.5 * scale);
    const int32_t y_offset = (int32_t)(1.0 * scale);
    const int32_t diverge_threshold = 4 * scale;
    //TODO: Complete the function implimentation
    for (int y = 0; y < height; y++) {
      int32_t y0 = ((y * y_scale) / height) - y_offset;
      for(int x =0; x < width; x++){
    	  int32_t x0 = ((x * x_scale) / width) - x_offset;
    	  int32_t xi = 0;
    	  int32_t yi = 0;
    	  int iteration = 0;

    	  while (iteration < max_iterations){
    		  int64_t xi_sq = ((int64_t)xi * xi) / scale;
    		  int64_t yi_sq = ((int64_t)yi * yi) / scale;
    		  if ((xi_sq + yi_sq) > diverge_threshold) break;
    		  int64_t temp = xi_sq - yi_sq;
    		  yi = (2 * ((int64_t)xi * yi) / scale) + y0;
    		  xi = temp + x0;
    		  iteration++;
    	  }
    	  checksum += iteration;
      }
  }
    return checksum;
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
