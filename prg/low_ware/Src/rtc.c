/**
  ******************************************************************************
  * File Name          : RTC.c
  * Description        : This file provides code for the configuration
  *                      of the RTC instances.
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
#include "rtc.h"

#include "nvic.h"
#include "err.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

#define RTC_IRQN RTC_IRQn
#define RTC_IRQ_PRIO 3

RTC_HandleTypeDef hrtc;

static RtcCallbackSecond_t OnSecond = NULL;
/* RTC init function */
void RtcInit(void)
{
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef DateToUpdate;

    /**Initialize RTC Only 
    */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    ErrorHandler(__FILE__, __LINE__);
  }
	/**Initialize RTC and set the Time and Date
	*/
	sTime.Hours = 0x0;
	sTime.Minutes = 0x0;
	sTime.Seconds = 0x0;

	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
	{
		ErrorHandler(__FILE__, __LINE__);
	}

	DateToUpdate.WeekDay = RTC_WEEKDAY_SUNDAY;
	DateToUpdate.Month = RTC_MONTH_SEPTEMBER;
	DateToUpdate.Date = 0x21;
	DateToUpdate.Year = 0x18;

	if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
	{
		ErrorHandler(__FILE__, __LINE__);
	}

	HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR1,0x32F2);
	NvicInterruptPrioritySet(RTC_IRQN, RTC_IRQ_PRIO, 0);
	/* RTC interrupt Init */
	HAL_RTCEx_SetSecond_IT(&hrtc);
	NvicInterruptEnable(RTC_IRQN, 1);
}

void RtcCallbackSetOnSecond(RtcCallbackSecond_t cb)
{
	OnSecond = cb;
}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */
    HAL_PWR_EnableBkUpAccess();
    /* Enable BKP CLK enable for backup registers */
    __HAL_RCC_BKP_CLK_ENABLE();
    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();

    /* RTC interrupt Deinit */
    HAL_NVIC_DisableIRQ(RTC_IRQn);
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}
void HAL_RTCEx_RTCEventCallback(RTC_HandleTypeDef *hrtc)
{
	RTC_TimeTypeDef sTime;
	HAL_RTC_GetTime(hrtc, &sTime, RTC_FORMAT_BIN);
	if(OnSecond != NULL) {
		OnSecond(sTime.Seconds);
	}
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
