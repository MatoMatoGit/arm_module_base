/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

#include "nvic.h"
#include "err.h"
#include "gpio.h"

/* USER CODE BEGIN 0 */

#define UART_INST_DEBUG USART2
#define UART_IRQN_RX_DEBUG USART2_IRQn
#define UART_IRQ_RX_PRIO_DEBUG 10

/* USER CODE END 0 */

UART_HandleTypeDef huart_debug;
uint8_t DebugRxChar = 0;
UartCallbackRx_t OnRx = NULL;

/* USART1 init function */

void UartDebugInit(uint32_t baud_rate)
{

  huart_debug.Instance = UART_INST_DEBUG;
  huart_debug.Init.BaudRate = baud_rate;
  huart_debug.Init.WordLength = UART_WORDLENGTH_8B;
  huart_debug.Init.StopBits = UART_STOPBITS_1;
  huart_debug.Init.Parity = UART_PARITY_NONE;
  huart_debug.Init.Mode = UART_MODE_TX_RX;
  huart_debug.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart_debug.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart_debug) != HAL_OK)
  {
    ErrorHandler(__FILE__, __LINE__);
  }

  NvicInterruptPrioritySet(UART_IRQN_RX_DEBUG, UART_IRQ_RX_PRIO_DEBUG, 0);
  HAL_UART_Receive_IT(&huart_debug, &DebugRxChar, 1);
}

void UartIntEnableRxDebug(uint8_t en)
{
	NvicInterruptEnable(UART_IRQN_RX_DEBUG, en);
}

void UartDebugCallbackSetOnRx(UartCallbackRx_t cb)
{
	OnRx = cb;
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{
  if(uartHandle->Instance==UART_INST_DEBUG)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
    GpioDebugUartInit();

  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==UART_INST_DEBUG)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();
    GpioDebugUartDeinit();

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart_debug) {
		if(OnRx != NULL) {
			OnRx(DebugRxChar);
		}
	}
	HAL_UART_Receive_IT(&huart_debug, &DebugRxChar, 1);
}

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
