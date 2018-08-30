/**
  ******************************************************************************
  * File Name          : TIM.c
  * Description        : This file provides code for the configuration
  *                      of the TIM instances.
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
#include "tim.h"

#include "err.h"
#include "stm32f1xx_hal.h"

/* USER CODE BEGIN 0 */

#define TIMER_INST_OS TIM2
#define TIMER_INST_APP TIM3

static TIM_HandleTypeDef *ITimHandleFromTimer(TimerInst_t timer);

/* USER CODE END 0 */

TIM_HandleTypeDef tim_os;
TIM_HandleTypeDef tim_app;

static TimerCallback_t tim_os_period_elapsed = NULL;
static TimerCallback_t tim_app_period_elapsed = NULL;

/* TIM init function */
void TimerInit(TimerInst_t timer, uint32_t prescaler, uint32_t period)
{
	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_HandleTypeDef *tim = ITimHandleFromTimer(timer);

	if(tim != NULL) {
		if(tim == &tim_os) {
			tim->Instance = TIMER_INST_OS;
		} else {
			tim->Instance = TIMER_INST_APP;
		}
		tim->Init.Prescaler = prescaler;
		tim->Init.CounterMode = TIM_COUNTERMODE_UP;
		tim->Init.Period = period;
		tim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		tim->Init.RepetitionCounter = 0;
		tim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
		if (HAL_TIM_Base_Init(tim) != HAL_OK)
		{
			ErrorHandler(__FILE__, __LINE__);
		}

		sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
		if (HAL_TIM_ConfigClockSource(tim, &sClockSourceConfig) != HAL_OK)
		{
			ErrorHandler(__FILE__, __LINE__);
		}
	} else {
		ErrorHandler(__FILE__, __LINE__);
	}

}

void TimerStart(TimerInst_t timer)
{
	TIM_HandleTypeDef *tim = ITimHandleFromTimer(timer);
	if(tim != NULL) {
		HAL_TIM_Base_Start_IT(tim);
	}
}

void TimerStop(TimerInst_t timer)
{
	TIM_HandleTypeDef *tim = ITimHandleFromTimer(timer);
	if(tim != NULL) {
		HAL_TIM_Base_Stop_IT(tim);
	}
}

void TimerCallbackRegisterPeriodElapsed(TimerInst_t timer, TimerCallback_t on_elapsed)
{
	if(timer == TIMER_OSTICK) {
		tim_os_period_elapsed = on_elapsed;
	} else if (timer == TIMER_APP_0) {
		tim_app_period_elapsed = on_elapsed;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &tim_os) {
		if(tim_os_period_elapsed != NULL) {
			tim_os_period_elapsed();
		}
	} else if (htim == &tim_app) {
		if(tim_app_period_elapsed != NULL) {
			tim_app_period_elapsed();
		}
	}
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIMER_INST_OS)
  {
  /* USER CODE BEGIN TIM1_MspInit 0 */

  /* USER CODE END TIM1_MspInit 0 */
    /* TIM1 clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();
  /* USER CODE BEGIN TIM1_MspInit 1 */

  /* USER CODE END TIM1_MspInit 1 */
  }
  else if(tim_baseHandle->Instance==TIMER_INST_APP)
  {
  /* USER CODE BEGIN TIM3_MspInit 0 */

  /* USER CODE END TIM3_MspInit 0 */
    /* TIM3 clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();
  /* USER CODE BEGIN TIM3_MspInit 1 */

  /* USER CODE END TIM3_MspInit 1 */
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIMER_INST_OS)
  {
  /* USER CODE BEGIN TIM1_MspDeInit 0 */

  /* USER CODE END TIM1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM2_CLK_DISABLE();

    /* TIM1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM1_MspDeInit 1 */

  /* USER CODE END TIM1_MspDeInit 1 */
  }
  else if(tim_baseHandle->Instance==TIMER_INST_APP)
  {
  /* USER CODE BEGIN TIM3_MspDeInit 0 */

  /* USER CODE END TIM3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM3_CLK_DISABLE();

    /* TIM3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM3_IRQn);
  /* USER CODE BEGIN TIM3_MspDeInit 1 */

  /* USER CODE END TIM3_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */

static TIM_HandleTypeDef *ITimHandleFromTimer(TimerInst_t timer)
{
	TIM_HandleTypeDef *tim = NULL;

	switch(timer) {
	case TIMER_OSTICK: {
		tim = &tim_os;
		break;
	}
	case TIMER_APP_0: {
		tim = &tim_app;
		break;
	}
	default: {
		break;
	}
	}

	return tim;
}

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
