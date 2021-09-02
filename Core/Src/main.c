/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "NOS_ModBus.h"
#include "../Src/Detector/Code/Detector.h"
#include "stdint.h"
#include <stdbool.h>
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
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
ModBus_Master_Command master;
ModBus_Slave_Command slave;
ModBusState state;
bool AddressOk = false;
uint8_t rx_buff[256];
uint8_t fuck_buff[1024];
uint16_t fuckIndex = 0;
uint8_t lenght = 0;
uint8_t currCommand = 0;
bool rx_flag = false;
bool tx_flag = false;
uint8_t tx_buff[16];
uint8_t counter = 0;
NOS_Short tickcount1;
uint32_t tickcount2 = 0;

NOS_Short time;
TStatus_Stat stat;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void NOS_ModBus_SendSlaveCommand(ModBus_Slave_Command* slave)
{

    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0);
  if(slave->type < 2)
  {
    tx_buff[0] = slave->Addr;
    tx_buff[1] = slave->Command;
    tx_buff[2] = slave->Byte_Count;
    NOS_Short crc;
    if(slave->type == 0)
    {
      tx_buff[3] = slave->ShortValue.bytes[1];
      tx_buff[4] = slave->ShortValue.bytes[0];
      crc.data = GetCRC16(&tx_buff,5);
      tx_buff[5] = crc.bytes[1];
      tx_buff[6] = crc.bytes[0];
      HAL_UART_Transmit_IT(&huart2,&tx_buff,7);
    }
    else
    {
      tx_buff[3] = slave->FloatValue.bytes[3];
      tx_buff[4] = slave->FloatValue.bytes[2];
      tx_buff[5] = slave->FloatValue.bytes[1];
      tx_buff[6] = slave->FloatValue.bytes[0];
      crc.data = GetCRC16(&tx_buff,7);
      tx_buff[8] = crc.bytes[1];
      tx_buff[9] = crc.bytes[0];
      HAL_UART_Transmit_IT(&huart2,&tx_buff,9);
    }
  }
  else
  {
    Stat_GetStatus(&stat);
    NOS_Float dat;
    dat.data = stat.CPS;
    tx_buff[0] = dat.bytes[3];
    tx_buff[1] = dat.bytes[2];
    tx_buff[2] = dat.bytes[1];
    tx_buff[3] = dat.bytes[0];
    tx_buff[4] = stat.Delta;
    dat.data = Detector_GetuZvValue(&stat);
    tx_buff[5] = dat.bytes[3];
    tx_buff[6] = dat.bytes[2];
    tx_buff[7] = dat.bytes[1];
    tx_buff[8] = dat.bytes[0];
    tx_buff[9] = stat.Status;
    tx_buff[10] = tickcount1.bytes[1];
    tx_buff[11] = tickcount1.bytes[0];

    HAL_UART_Transmit_IT(&huart2,tx_buff,12); 
  } 
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {

static uint8_t* rx_buff_ptr = rx_buff;

fuck_buff[fuckIndex] = *rx_buff_ptr;
fuckIndex++;
if(fuckIndex > 1023)
{
  fuckIndex = 0;
}

if(*rx_buff_ptr == 101 && !AddressOk)
{
  AddressOk = true;
  lenght = 0;
}

if(AddressOk && lenght == 1)
{
  currCommand = *rx_buff_ptr;
}

if(lenght > 10)
{
  rx_buff_ptr = rx_buff;
  lenght = 0;
  AddressOk = false;
  for(int i = 0; i < 10; i++)
  {
    rx_buff[i] = 0;
  }
}

if(AddressOk && lenght == 7 && currCommand == 0x03)
{
  rx_buff_ptr = rx_buff;
  AddressOk = false;
  lenght = 0;
  rx_flag = true;
}
else
{
  rx_buff[lenght] = *rx_buff_ptr;
  ++rx_buff_ptr;
  ++lenght;
}
    HAL_UART_Receive_IT (&huart2, rx_buff_ptr, 1); 
}

void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */
time.data++;
if(time.data >= 250)
{
  counter++;
  Stat_AddData250ms(tickcount1.data);
  if(counter >= 4)
  {
    slave.type = 5;
    NOS_ModBus_SendSlaveCommand(&slave);
    counter = 0; 
  }
  
  time.data = 0;
  tickcount1.data = 0;
}
}

void EXTI0_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_IRQn 0 */

  /* USER CODE END EXTI0_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
  /* USER CODE BEGIN EXTI0_IRQn 1 */
tickcount1.data++;
  /* USER CODE END EXTI0_IRQn 1 */
}

/**
  * @brief This function handles EXTI line1 interrupt.
  */
void EXTI1_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI1_IRQn 0 */

  /* USER CODE END EXTI1_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
  /* USER CODE BEGIN EXTI1_IRQn 1 */

  /* USER CODE END EXTI1_IRQn 1 */
}
  /* USER CODE END SysTick_IRQn 1 */


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
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT (&huart2, rx_buff, 1); 
  Stat_Init(3.4,60,0);
  Detector_Init_Param(1.395E-3f, 0);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
     if(rx_flag)
    {
      NOS_ModBus_ParseMasterCommand(&master,&rx_buff,0);
      switch(master.Command)
      {
        case 0x03:
        slave.Addr = 101;
        slave.Command = 0x03;

        switch(master.Reg_Addr)
        {
          case 0x0000:
          slave.ShortValue.data = 1000;
          slave.Byte_Count = 2;
          slave.type = 0;
          break;

          case 0x0001:
          slave.ShortValue.data = 25;
          slave.Byte_Count = 2;
          slave.type = 0;
          break;

          case 0x0003:
          Stat_GetStatus(&stat);
          slave.FloatValue.data = Detector_GetuZvValue(&stat);
          slave.Byte_Count = 4;
          slave.type = 1;
          break;

          case 0x0005:
          slave.ShortValue.data = 400;
          slave.Byte_Count = 2;
          slave.type = 0;
          break;
         
          case 0x0006:
          slave.FloatValue.data = 0.25f;
          slave.Byte_Count = 4;
          slave.type = 1;
          break;

          case 0x0008:
          slave.FloatValue.data = 1.25f;
          slave.Byte_Count = 4;
          slave.type = 1;
          break;         
        }  
      }

      NOS_ModBus_SendSlaveCommand(&slave);
      HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_8);
      rx_flag = false;
    }
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
  huart2.Init.BaudRate = 9600;
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
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);

  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
