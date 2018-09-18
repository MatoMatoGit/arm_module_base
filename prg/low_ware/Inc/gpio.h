/**
  ******************************************************************************
  * File Name          : gpio.h
  * Description        : This file contains all the functions prototypes for 
  *                      the gpio  
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __gpio_H
#define __gpio_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */
#include <stdint.h>

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

#define GPIO_CONFIG_7SD_SELECT_ACTIVE_LOW 0

 typedef void (*GpioIntCallback_t)(uint8_t pin, uint8_t state);

#define UI_BUTTON_INC 0
#define UI_BUTTON_DEC 1
#define UI_BUTTON_SEL 2

#define UI_BUTTON_IRQ_PRIO 15

/* USER CODE END Private defines */

/***** 7 Segment Display SPI & Select GPIO. *****/
void Gpio7SdSpiInit(void);
void Gpio7SdSpiDeinit(void);
void Gpio7SdSpiNssStateSet(uint8_t state);
void Gpio7SdSelInit(void);
void Gpio7SdSelDeinit(void);
void Gpio7SdSelStateSet(uint8_t n, uint8_t state);

/***** User Interface Button GPIO. *****/
void GpioUiButtonInit(void);
void GpioUiButtonDeinit(void);
void GpioIntUiButtonEnable(uint8_t en);
uint16_t GpioPinGetButton(uint8_t btn);
uint8_t GpioUiButtonStateGet(uint8_t btn);
void GpioUiButtonIntCallbackSet(uint8_t btn, GpioIntCallback_t cb);

/***** RGB LED GPIO. *****/
void GpioRgbLedInit(void);
void GpioRgbLedDeinit(void);
void GpioRgbLedRedStateSet(uint8_t state);
void GpioRgbLedGreenStateSet(uint8_t state);
void GpioRgbLedBlueStateSet(uint8_t state);

/***** Pump GPIO. *****/
void GpioPumpInit(void);
void GpioPumpStateSet(uint8_t state);

/***** Debug UART GPIO. *****/
void GpioDebugUartInit(void);
void GpioDebugUartDeinit(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
