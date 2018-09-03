/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
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
#include "gpio.h"

#include "stm32f1xx_hal.h"

/* USER CODE BEGIN 0 */

/***** 7 Segment Display SPI & Select GPIO. *****/
#define CLOCK_ENABLE_7SD_SPI	__HAL_RCC_GPIOA_CLK_ENABLE
#define MODE_7SD_SPI		GPIO_MODE_AF_PP
#define MODE_7SD_SPI_NSS	GPIO_MODE_OUTPUT_PP
#define PORT_7SD_SPI		GPIOA
#define PIN_7SD_SPI_NSS		GPIO_PIN_4
#define PIN_7SD_SPI_SCK		GPIO_PIN_5
#define PIN_7SD_SPI_MOSI	GPIO_PIN_7

#define CLOCK_ENABLE_7SD_GPIO	__HAL_RCC_GPIOC_CLK_ENABLE
#define MODE_7SD_SEL		GPIO_MODE_OUTPUT_PP
#define PORT_7SD_SEL		GPIOC
#define PIN_7SD_SEL_0		GPIO_PIN_6
#define PIN_7SD_SEL_1		GPIO_PIN_7
#define PIN_7SD_SEL_2		GPIO_PIN_8
#define PIN_7SD_SEL_3		GPIO_PIN_9
#if GPIO_CONFIG_7SD_SELECT_ACTIVE_LOW==1
#define PIN_7SD_SEL_SET		GPIO_PIN_RESET
#define PIN_7SD_SEL_RESET	GPIO_PIN_SET
#else
#define PIN_7SD_SEL_SET		GPIO_PIN_SET
#define PIN_7SD_SEL_RESET	GPIO_PIN_RESET
#endif

/***** User Interface Button GPIO. *****/
#define CLOCK_ENABLE_UI_BUTTON	__HAL_RCC_GPIOB_CLK_ENABLE
#define MODE_UI_BUTTON		GPIO_MODE_IT_FALLING
#define PORT_UI_BUTTON		GPIOB
#define PIN_UI_BUTTON_INC	GPIO_PIN_5
#define PIN_UI_BUTTON_DEC	GPIO_PIN_6
#define PIN_UI_BUTTON_SEL	GPIO_PIN_7

static GpioIntCallback_t UiButtonIncCb = NULL;
static GpioIntCallback_t UiButtonDecCb = NULL;
static GpioIntCallback_t UiButtonSelCb = NULL;

/***** Pump GPIO. *****/
#define CLOCK_ENABLE_PUMP		__HAL_RCC_GPIOB_CLK_ENABLE
#define MODE_PUMP			GPIO_MODE_OUTPUT_PP
#define PORT_PUMP			GPIOB
#define PIN_PUMP			GPIO_PIN_8

/***** RGB LED GPIO. *****/
#define CLOCK_ENABLE_RGBLED		__HAL_RCC_GPIOB_CLK_ENABLE
#define MODE_RGBLED			GPIO_MODE_OUTPUT_PP
#define PORT_RGBLED			GPIOB
#define PIN_RGBLED_RED		GPIO_PIN_10
#define PIN_RGBLED_GREEN	GPIO_PIN_11
#define PIN_RGBLED_BLUE		GPIO_PIN_12

/***** Debug UART GPIO. *****/
#define MODE_DEBUG_UART_TX	GPIO_MODE_AF_PP
#define MODE_DEBUG_UART_RX	GPIO_MODE_INPUT
#define PORT_DEBUG_UART		GPIOA
#define PIN_DEBUG_UART_TX	GPIO_PIN_2
#define PIN_DEBUG_UART_RX	GPIO_PIN_3

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */


/* USER CODE BEGIN 2 */

/***** 7 Segment Display SPI & Select GPIO. *****/

void Gpio7SdSpiInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	CLOCK_ENABLE_7SD_SPI();

	/**SPI1 GPIO Configuration
	PA4     ------> SPI1_NSS
	PA5     ------> SPI1_SCK
	PA7     ------> SPI1_MOSI
	*/
	GPIO_InitStruct.Pin = PIN_7SD_SPI_SCK|PIN_7SD_SPI_MOSI;
	GPIO_InitStruct.Mode = MODE_7SD_SPI;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(PORT_7SD_SPI, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PIN_7SD_SPI_NSS;
	GPIO_InitStruct.Mode = MODE_7SD_SPI_NSS;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(PORT_7SD_SPI, &GPIO_InitStruct);
}

void Gpio7SdSpiDeinit(void)
{
	/**SPI1 GPIO Configuration
	PA4     ------> SPI1_NSS
	PA5     ------> SPI1_SCK
	PA7     ------> SPI1_MOSI
	*/
	HAL_GPIO_DeInit(PORT_7SD_SPI, PIN_7SD_SPI_NSS|PIN_7SD_SPI_SCK|PIN_7SD_SPI_MOSI);
}

void Gpio7SdSpiNssStateSet(uint8_t state)
{
	if(state) {
		HAL_GPIO_WritePin(PORT_7SD_SPI, PIN_7SD_SPI_NSS, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(PORT_7SD_SPI, PIN_7SD_SPI_NSS, GPIO_PIN_RESET);
	}

}

void Gpio7SdSelInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	CLOCK_ENABLE_7SD_GPIO();
	HAL_GPIO_WritePin(PORT_7SD_SEL, PIN_7SD_SEL_0|PIN_7SD_SEL_1|PIN_7SD_SEL_2|PIN_7SD_SEL_3, PIN_7SD_SEL_RESET);

	/*Configure GPIO pins : PC6 PC7 PC8 PC9 */
	GPIO_InitStruct.Pin = PIN_7SD_SEL_0|PIN_7SD_SEL_1|PIN_7SD_SEL_2|PIN_7SD_SEL_3;
	GPIO_InitStruct.Mode = MODE_7SD_SEL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(PORT_7SD_SEL, &GPIO_InitStruct);
}

void Gpio7SdSelDeinit(void)
{
	HAL_GPIO_DeInit(PORT_7SD_SEL, PIN_7SD_SEL_0|PIN_7SD_SEL_1|PIN_7SD_SEL_2|PIN_7SD_SEL_3);
}

void Gpio7SdSelStateSet(uint8_t n, uint8_t state)
{
	uint16_t pin = PIN_7SD_SEL_0;

	switch(n) {
	default:
	case 0: {
		pin = PIN_7SD_SEL_0;
		break;
	}
	case 1: {
		pin = PIN_7SD_SEL_1;
		break;
	}
	case 2: {
		pin = PIN_7SD_SEL_2;
		break;
	}
	case 3: {
		pin = PIN_7SD_SEL_3;
		break;
	}
	}

	if(state) {
		HAL_GPIO_WritePin(PORT_7SD_SEL, pin, PIN_7SD_SEL_SET);
	} else {
		HAL_GPIO_WritePin(PORT_7SD_SEL, pin, PIN_7SD_SEL_RESET);
	}
}

/***** User Interface Button GPIO. *****/

void GpioUiButtonInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	CLOCK_ENABLE_UI_BUTTON();

	/*Configure GPIO pins : PB5 PB6 PB7 */
	GPIO_InitStruct.Pin = PIN_UI_BUTTON_INC|PIN_UI_BUTTON_DEC|PIN_UI_BUTTON_SEL;
	GPIO_InitStruct.Mode = MODE_UI_BUTTON;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(PORT_UI_BUTTON, &GPIO_InitStruct);
}

void GpioUiButtonDeinit(void)
{
	HAL_GPIO_DeInit(PORT_UI_BUTTON, PIN_UI_BUTTON_INC|PIN_UI_BUTTON_DEC|PIN_UI_BUTTON_SEL);
}

uint8_t GpioUiButtonStateGet(uint8_t btn)
{
	uint8_t state = 0;
	uint16_t pin = 0;

	switch(btn) {
	default:
	case UI_BUTTON_INC: {
		pin = PIN_UI_BUTTON_INC;
		break;
	}
	case UI_BUTTON_DEC: {
		pin = PIN_UI_BUTTON_DEC;
		break;
	}
	case UI_BUTTON_SEL: {
		pin = PIN_UI_BUTTON_SEL;
		break;
	}
	}

	state = HAL_GPIO_ReadPin(PORT_UI_BUTTON, pin);

	return state;
}

void GpioUiButtonIntCallbackSet(uint8_t btn, GpioIntCallback_t cb)
{
	switch(btn) {
	default:
	case UI_BUTTON_INC: {
		UiButtonIncCb = cb;
		break;
	}
	case UI_BUTTON_DEC: {
		UiButtonDecCb = cb;
		break;
	}
	case UI_BUTTON_SEL: {
		UiButtonSelCb = cb;
		break;
	}
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	GpioIntCallback_t cb = NULL;

	switch(GPIO_Pin) {
	default:{
		cb = NULL;
		break;
	}
	case PIN_UI_BUTTON_INC: {
		cb = UiButtonIncCb;
		break;
	}
	case PIN_UI_BUTTON_DEC: {
		cb = UiButtonDecCb;
		break;
	}
	case PIN_UI_BUTTON_SEL: {
		cb= UiButtonSelCb;
		break;
	}
	}

	if(cb != NULL) {
		cb(HAL_GPIO_ReadPin(PORT_UI_BUTTON, GPIO_Pin));
	}
}

/***** Pump GPIO. *****/

void GpioPumpInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	CLOCK_ENABLE_PUMP();
	HAL_GPIO_WritePin(PORT_PUMP, PIN_PUMP, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = PIN_PUMP;
	GPIO_InitStruct.Mode = MODE_PUMP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(PORT_PUMP, &GPIO_InitStruct);
}

void GpioPumpStateSet(uint8_t state)
{
	if(state) {
		HAL_GPIO_WritePin(PORT_PUMP, PIN_PUMP, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(PORT_PUMP, PIN_PUMP, GPIO_PIN_RESET);
	}
}

/***** RGB LED GPIO. *****/

void GpioRgbLedInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	CLOCK_ENABLE_RGBLED();
	HAL_GPIO_WritePin(PORT_RGBLED, PIN_RGBLED_RED|PIN_RGBLED_GREEN|PIN_RGBLED_BLUE, GPIO_PIN_RESET);

	/*Configure GPIO pins : PB10 PB11 PB12 PB8 */
	GPIO_InitStruct.Pin = PIN_RGBLED_RED|PIN_RGBLED_GREEN|PIN_RGBLED_BLUE;
	GPIO_InitStruct.Mode = MODE_RGBLED;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(PORT_RGBLED, &GPIO_InitStruct);
}

void GpioRgbLedDeinit(void)
{
	HAL_GPIO_DeInit(PORT_RGBLED, PIN_RGBLED_RED|PIN_RGBLED_GREEN|PIN_RGBLED_BLUE);
}

void GpioRgbLedRedStateSet(uint8_t state)
{
	if(state) {
		HAL_GPIO_WritePin(PORT_RGBLED, PIN_RGBLED_RED, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(PORT_RGBLED, PIN_RGBLED_RED, GPIO_PIN_RESET);
	}
}

void GpioRgbLedGreenStateSet(uint8_t state)
{
	if(state) {
		HAL_GPIO_WritePin(PORT_RGBLED, PIN_RGBLED_GREEN, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(PORT_RGBLED, PIN_RGBLED_GREEN, GPIO_PIN_RESET);
	}
}

void GpioRgbLedBlueStateSet(uint8_t state)
{
	if(state) {
		HAL_GPIO_WritePin(PORT_RGBLED, PIN_RGBLED_BLUE, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(PORT_RGBLED, PIN_RGBLED_BLUE, GPIO_PIN_RESET);
	}
}


/***** Debug UART GPIO. *****/

void GpioDebugUartInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/**USART1 GPIO Configuration
	PA9     ------> USART1_TX
	PA10     ------> USART1_RX
	*/
	GPIO_InitStruct.Pin = PIN_DEBUG_UART_TX;
	GPIO_InitStruct.Mode = MODE_DEBUG_UART_TX;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(PORT_DEBUG_UART, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PIN_DEBUG_UART_RX;
	GPIO_InitStruct.Mode = MODE_DEBUG_UART_RX;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(PORT_DEBUG_UART, &GPIO_InitStruct);
}

void GpioDebugUartDeinit(void)
{
	/**USART1 GPIO Configuration
	PA9     ------> USART1_TX
	PA10     ------> USART1_RX
	*/
	HAL_GPIO_DeInit(PORT_DEBUG_UART, PIN_DEBUG_UART_TX|PIN_DEBUG_UART_RX);
}

/* USER CODE END 2 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
