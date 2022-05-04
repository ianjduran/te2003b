/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
void USER_RCC_Init();
void USER_GPIO_Init();
void USER_USART1_Transmit(uint8_t *pData, uint16_t size);
void USER_USART1_Init(void);
uint8_t USER_USART1_Receive(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t pollButton(uint8_t row, uint8_t col){
	//Set all row pins to 1, except polling row
	for(int i = 4; i < 8; i++){
		GPIOA->ODR |= 1 << i;
	}
	GPIOA->ODR &= ~(1 << row);

	//Read col
	//return 0;
	return GPIOA->IDR & (1 << col);
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
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPAEN;
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

  USER_RCC_Init();
  USER_GPIO_Init();
  USER_USART1_Init();

  uint8_t keypad_chars[] = {'1', '2', '3', 'A', '4', '5', '6', 'B', '7', '8', '9', 'C', '\r', '0', '\n', 'D'};
  uint8_t msg_buffer[256];

  uint8_t lastChar = 0xff;
  unsigned int charCounter = 0;

  uint8_t currentLine = 0;
  uint8_t currentRow = 0;

  LCD_Init();
  LCD_Cursor_ON();
  LCD_Clear();
  LCD_Set_Cursor(currentLine, currentRow);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  uint8_t selectedChar = 0xff;

	  for(int i = 0; i < 4; i++){
 		  for(int j = 0; j < 4; j++){
			  int out = pollButton(j + 4, i);
			  int index = i * 4 + j;

			  if(out == 0){
				  selectedChar = keypad_chars[index];
			  }
		  }
	  }

	  if(lastChar == selectedChar) charCounter++;
	  else charCounter = 0;

	  if(selectedChar != 0xff && charCounter == 1500){
		  snprintf(msg_buffer, sizeof(msg_buffer), "%c", selectedChar);
		  USER_USART1_Transmit(msg_buffer, strlen(msg_buffer));


	  }

	  lastChar = selectedChar;

	  //Check if char available
	  if((USART1->SR & USART_SR_RXNE) != 0){
		  uint8_t inputChar = USER_USART1_Receive();
		  //Handle writing character to lcd
		  //Handle special chars
		  if(inputChar == '\n'){
			  if(currentLine == 0) currentLine = 1;
			  else currentLine = 0;
		  } else if(inputChar == '\r'){
			  currentRow++;
			  if(currentRow >= 16) currentRow = 0; //Wrap around
		  } else {
			  LCD_Set_Cursor(currentLine, currentRow);
			  LCD_Put_Char(inputChar);
		  }
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

void USER_RCC_Init(void){
	RCC->APB2ENR	|=	RCC_APB2ENR_USART1EN
						|	RCC_APB2ENR_IOPCEN
						|	RCC_APB2ENR_IOPAEN
						|   RCC_APB1ENR_TIM2EN;
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
}

void USER_USART1_Init(void) {
	USART1->BRR	=	0x1D4C;
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

void USER_USART1_Transmit(uint8_t *pData, uint16_t size){
	for(int i=0; i< size; i++){
		while( (USART1->SR & USART_SR_TXE)==0 ){

		}
		USART1->DR = *pData++;
	}
}

uint8_t USER_USART1_Receive(void){
	while( (USART1->SR & USART_SR_RXNE)==0 ){

	}
	return USART1->DR;

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
