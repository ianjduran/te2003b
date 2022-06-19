/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "poll_keyboard_task.h"
#include "tx_task.h"
#include "rx_tast.h"
#include "RFIDTask.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#include "LCDTask/LCDTask.h"
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 SPI_HandleTypeDef hspi1;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */

osThreadId_t pollKeypadTaskHandle;
const osThreadAttr_t pollKeypad_attributes = {
		.name = "pollKeypadTask",
		.stack_size = 128 * 4,
		.priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t rxTaskHandle;
const osThreadAttr_t rxTask_attributes = {
		.name = "rxTask",
		.stack_size = 128 * 4,
//		.priority = (osPriority_t) osPriorityAboveNormal,
		.priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t txTaskHandle;
const osThreadAttr_t txTask_attributes = {
		.name = "txTask",
		.stack_size = 128*4,
		.priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t lcdTaskHandle;
const osThreadAttr_t lcdTask_attributes = {
		.name = "lcdTask",
		.stack_size = 128 * 16,
		.priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t rfidTaskHandle;
const osThreadAttr_t rfidTask_attributes = {
		.name = "rfidTask",
		.stack_size = 128 * 4,
		.priority = (osPriority_t) osPriorityNormal,
};


osMessageQueueId_t inputQueueHandle;
const osMessageQueueAttr_t inputQueue_attributes = {
		.name = "inputQueue",
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
void StartDefaultTask(void *argument);

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
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  USER_RCC_Init();
  USER_GPIO_Init();
  USER_USART1_Init();
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  inputQueueHandle = osMessageQueueNew(20, sizeof(uint8_t), &inputQueue_attributes);
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  rxTaskHandle = osThreadNew(receive_data_task, NULL, &rxTask_attributes);
  lcdTaskHandle = osThreadNew(lcd_task, NULL, &lcdTask_attributes);
  txTaskHandle = osThreadNew(transmit_data_task, NULL, &txTask_attributes);
  pollKeypadTaskHandle = osThreadNew(poll_keyboard_button_task, NULL, &pollKeypad_attributes);
  rfidTaskHandle = osThreadNew(rfid_task, NULL, &rfidTask_attributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(RC522_CS_GPIO_Port, RC522_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : RC522_CS_Pin */
  GPIO_InitStruct.Pin = RC522_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RC522_CS_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void USER_USART1_Init(void) {
//	USART1->BRR	=	0x1D4C; //9600
//	USART1->BRR	=	0x027A; //115200
	USART1->BRR	=	0x3A98; //4800
	USART1->CR1 &=	~USART_CR1_M
				&	~USART_CR1_WAKE
				&	~USART_CR1_PCE
				&	~USART_CR1_TXEIE
				&	~USART_CR1_TCIE
				&	~USART_CR1_RXNEIE
				&	~USART_CR1_IDLEIE
				&	~USART_CR1_RWU
				&	~USART_CR1_SBK;
	USART1->CR1	|=	USART_CR1_UE
				|	USART_CR1_TE
				|	USART_CR1_RE;
	USART1->CR2 &=	~USART_CR2_STOP;
}

void config_pin(GPIO_TypeDef *port, uint8_t pin, uint8_t mode){
	uint8_t modeBits, configBits;
	uint8_t finalBits;
	if(mode == 0){
		//Input
		modeBits = 0b00;
		configBits = 0b01;
	} else if(mode == 1){
		//Output
		modeBits = 0b01;
		configBits = 0b00;
	} else{
		//Input with pullup
		modeBits = 0b00;
		configBits = 0b10;
		port->ODR |= (0b1) << pin;
	}

	finalBits = (0b1100 & (configBits << 2));
	finalBits |= modeBits & 0b11;

	if(mode < 8){
		//Low reg
		port->CRL &= ~(0b1111 << pin * 4);
		port->CRL |= (0b1111 << pin * 4) & (finalBits << pin * 4);
	} else {
		//High reg
		pin -= 8;
		port->CRH &= ~(0b1111 << pin * 4);
		port->CRH |= (0b1111 << pin * 4) & (finalBits << pin * 4);
	}
}

void USER_RCC_Init(void){
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	RCC->APB2ENR	|=	RCC_APB2ENR_USART1EN
						|	RCC_APB2ENR_IOPCEN
						|	RCC_APB2ENR_IOPAEN;
}

void USER_GPIO_Init(void){
	//Configure serial terminal
	GPIOA->CRH	&= ~GPIO_CRH_CNF9_0 & ~GPIO_CRH_MODE9_1;
	GPIOA->CRH	|= GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9_0;

	GPIOA->CRH &=	~GPIO_CRH_CNF10_1 & ~GPIO_CRH_MODE10;
	GPIOA->CRH |=	GPIO_CRH_CNF10_0;

	GPIOA->BSRR	=	GPIO_BSRR_BS13;

	GPIOC->CRH	&=	~GPIO_CRH_CNF13 & ~GPIO_CRH_MODE13_1;
	GPIOC->CRH	|=	GPIO_CRH_MODE13_0;

	//Configure pins for keypad
	config_pin(GPIOA, 0, 2);
	config_pin(GPIOA, 1, 2);
	config_pin(GPIOA, 2, 2);
	config_pin(GPIOA, 3, 2);
	config_pin(GPIOA, 4, 1);
	config_pin(GPIOA, 5, 1);
	config_pin(GPIOA, 6, 1);
	config_pin(GPIOA, 7, 1);
	config_pin(GPIOC, 13, 1);
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {

    osDelay(1);
  }
  /* USER CODE END 5 */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM4 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM4) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
