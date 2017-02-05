/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_hal.h"
#include "string.h"
#include "stdio.h"
#define TIMEOUT 5000


/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;				// Debug  UART
UART_HandleTypeDef huart2;				// ESP8266 UART

char buffer[100];
int len;

unsigned char i;
char Rx_Buffer[100];
char Rx_data[2];
char Rx_indx;
char Transfer_cplt;

const char mySSID[]="PhongTro_plus";
const char myPSK[]="12345678phongtro";


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void ESP8266_Init(void);
void UART_Print(char *msg);
void AT(void);
void AT_RST(void);
void AT_GMR(void);
void AT_CIFSR(void);
void AT_CWJAP(void);
void AT_CWMODE(void);


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	
	if(huart->Instance==USART2)
	{	
		
		if (Rx_indx ==0)
		{
			for (i=0; i<100;i++) 
			Rx_Buffer[i]=0;
		} 
		Rx_Buffer[Rx_indx++]=Rx_data[0];	
		for (i=2;i<100;i++)
		{
				if((Rx_Buffer[i-1]=='O') && (Rx_Buffer[i-0]=='K'))
				{
					Transfer_cplt=1;
				}	
		}
				
	
		HAL_UART_Receive_IT(&huart2, (uint8_t *)Rx_data, 1);
	
	
	}
}	


int main(void)
{

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_IT(&huart2, (uint8_t *)Rx_data, 1);

	
		
	ESP8266_Init();
	
	while (1)
  {		
		if(Transfer_cplt)
		{
			sprintf(buffer,"%s",Rx_Buffer);			// MUST HAVE
			HAL_UART_Transmit(&huart1, (uint8_t*) buffer,strlen(buffer),1000);			// MUST HAVE
			Transfer_cplt=0;
			Rx_indx=0;													// MUST HAVE

		}	
  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
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

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LD4_Pin|LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LD4_Pin LD3_Pin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

static void ESP8266_Init(void)
{
	AT();
	AT_CWJAP();
	AT_GMR();
}	

void AT(void)
{
	char AT[]="AT\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t *) AT, sizeof(AT)-1, TIMEOUT);
	HAL_Delay(1000);			// MUST HAVE
} 

void AT_RST(void)
{
	char AT_RST[]="AT+RST\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t *) AT_RST, sizeof(AT_RST)-1, TIMEOUT);
	HAL_Delay(1000);
}
void AT_GMR(void)
{
	char AT_GMR[]="AT+GMR\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t *) AT_GMR, sizeof(AT_GMR)-1, TIMEOUT);
	HAL_Delay(1000);
}

void AT_CIFSR(void)
{
	char AT_CIFSR[]="AT+CIFSR\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t *) AT_CIFSR, sizeof(AT_CIFSR)-1, TIMEOUT);
	HAL_Delay(1000);
}	

void AT_CWJAP(void)
{
	
	  char AT_CWJAP[100];
    strcpy(AT_CWJAP,"AT+CWJAP=\"");
    strcat(AT_CWJAP,mySSID);
    strcat(AT_CWJAP,"\",\"");
    strcat(AT_CWJAP,myPSK);
    strcat(AT_CWJAP,"\"\r\n");	
	
	HAL_UART_Transmit(&huart2, (uint8_t *) AT_CWJAP, sizeof(AT_CWJAP)-1, TIMEOUT);
	HAL_Delay(1000);
}	

void AT_CWMODE(void)
{	
	char AT_CWMODE[]="AT+CWMODE=3\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t *) AT_CWMODE, sizeof(AT_CWMODE)-1, TIMEOUT);
}
void UART_Print(char *msg)
{	
	char log[MESSAGE_SIZE]= "";
	strcat(log,msg); 			// ghep chuoi nguon vao sau chuoi dich
	strcat(log,"\r\n");				// ghep them \r\n

	HAL_UART_Transmit(&huart1, (uint8_t*) log, (sizeof(log))-1, TIMEOUT);
	
}	
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
