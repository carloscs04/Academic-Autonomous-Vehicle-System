/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "stm32f1xx.h"

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
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint8_t enviar[2] = {0x01,0x1D};
uint8_t recibir[6]; 
int16_t XaxisW,YaxisW,ZaxisW;
int AsMag;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
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
  MX_USART2_UART_Init();
  MX_USB_DEVICE_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */	
	
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	HAL_I2C_Mem_Write(&hi2c1, 0x1A, 0x0B, 1, &enviar[0], 1, 100);
	HAL_I2C_Mem_Write(&hi2c1, 0x1A, 0x09, 1, &enviar[1], 1, 100);
	
	int Xgap = -400;
	int Ygap = -520;
	int Zgap = 0;
	
  while (1)
  {
    /* USER CODE END WHILE */
		/* USER CODE END WHILE */
		 /* USER CODE END WHILE */
		/* USER CODE END WHILE */
		HAL_I2C_Mem_Read(&hi2c1, 0x1A, 0x06, 1, recibir, 1, 100);
		uint8_t mensaje[18] = " 0000, 0000, 0000 ";
		mensaje[17] = 10;
		//if(recibir[0]&0x01){
				HAL_I2C_Mem_Read(&hi2c1, 0x1A, 0x00, 1, recibir, 6, 100);
				XaxisW = (recibir[1]<<8) | recibir[0];
				YaxisW = (recibir[3]<<8) | recibir[2];
				ZaxisW = (recibir[5]<<8) | recibir[4];
		//}
		int Xaxis = XaxisW - Xgap;
		int Yaxis = YaxisW - Ygap;
		int Zaxis = ZaxisW - Zgap;
		
		if(Xaxis >= 0){
			mensaje[0] = 32;
		} else {
			mensaje[0] = 45;
		}
		
		if(abs(Xaxis) > 1000){
			mensaje[1] = abs(Xaxis)/1000 + 48;
			Xaxis = abs(Xaxis)%1000;
		} else{
					mensaje[1] = 48;
			}
		if(abs(Xaxis) > 100){
			mensaje[2] = abs(Xaxis)/100 + 48;
			Xaxis = abs(Xaxis)%100;
		} else{
					mensaje[2] = 48;
			}
		if(abs(Xaxis) > 10){
			mensaje[3] = abs(Xaxis)/10 + 48;
			Xaxis = abs(Xaxis)%10;
		} else{
					mensaje[3] = 48;
			}
		mensaje[4] = abs(Xaxis) + 48;
		
		if (Xaxis + 48 >= 58){
				mensaje[4] = 48; // Asegurar un valor válido si Zaxis es menor a 10
		}		
		
		if(Yaxis >= 0){
			mensaje[6] = 32;
		} else {
			mensaje[6] = 45;
		}
		
		if(abs(Yaxis) > 1000){
			mensaje[7] = abs(Yaxis)/1000 + 48;
			Yaxis = abs(Yaxis)%1000;
			} else{
					mensaje[7] = 48;
			}
		if(abs(Yaxis) > 100){
			mensaje[8] = abs(Yaxis)/100 + 48;
			Yaxis = abs(Yaxis)%100;
			} else{
					mensaje[8] = 48;
			}
		if(abs(Yaxis) > 10){
			mensaje[9] = abs(Yaxis)/10 + 48;
			Yaxis = abs(Yaxis)%10;
			} else{
				mensaje[9] = 48;
			}
		
	  mensaje[10] = abs(Yaxis) + 48;
			if (Yaxis + 48 >= 58){
				mensaje[10] = 48; // Asegurar un valor válido si Zaxis es menor a 10
			}
			
		if(Zaxis >= 0){
			mensaje[12] = 32;
		} else {
			mensaje[12] = 45;
		}
			
		if(abs(Zaxis) > 1000){
			mensaje[13] = abs(Zaxis)/1000 + 48;
			Zaxis = abs(Zaxis)%1000;
			} else{
					mensaje[13] = 48;
			}
		if(abs(Zaxis) > 100){
			mensaje[14] = abs(Zaxis)/100 + 48;
			Zaxis = abs(Zaxis)%100;
			} else{
					mensaje[14] = 48;
			}
		if(abs(Zaxis) > 10){
			mensaje[15] = abs(Zaxis)/10 + 48;
			Zaxis = abs(Zaxis)%10;
			} else{
					mensaje[15] = 48;
			}
		
		mensaje[16] = abs(Zaxis) + 48;
			if (Zaxis + 48 >= 58){
					mensaje[16] = 48; // Asegurar un valor válido si Zaxis es menor a 10
			}	
		CDC_Transmit_FS(mensaje,18);	
		HAL_UART_Transmit(&huart2, mensaje, sizeof(mensaje), HAL_MAX_DELAY);
		HAL_Delay(100);
			
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
