/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
  ******************************************************************************
*/

#include "gpio.h"

#include "nvic.h"
#include "stm32f1xx_hal.h"

/***** Shiftregister SPI GPIO. *****/
#define CLOCK_ENABLE_SHIFTREG_SPI	__HAL_RCC_GPIOA_CLK_ENABLE
#define MODE_SHIFTREG_SPI		GPIO_MODE_AF_PP
#define MODE_SHIFTREG_SPI_NSS	GPIO_MODE_OUTPUT_PP
#define PORT_SHIFTREG_SPI		GPIOA
#define PIN_SHIFTREG_SPI_NSS		GPIO_PIN_4
#define PIN_SHIFTREG_SPI_SCK		GPIO_PIN_5
#define PIN_SHIFTREG_SPI_MOSI	GPIO_PIN_7

/***** 7 Segment Display select GPIO. *****/
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

/***** LCD Display control GPIO. *****/
#define CLOCK_ENABLE_LCD_GPIO	__HAL_RCC_GPIOC_CLK_ENABLE
#define MODE_LCD_CTRL		GPIO_MODE_OUTPUT_PP
#define PORT_LCD_CTRL		GPIOC
#define PIN_LCD_CTRL_RS		GPIO_PIN_6
#define PIN_LCD_CTRL_RW		GPIO_PIN_7
#define PIN_LCD_CTRL_ENABLE	GPIO_PIN_8
#define PIN_LCD_CTRL_BACKLIGHT GPIO_PIN_9


/***** User Interface Button GPIO. *****/
#define CLOCK_ENABLE_UI_BUTTON	__HAL_RCC_GPIOB_CLK_ENABLE
#define IRQ_UI_BUTTON		EXTI9_5_IRQn
#define MODE_UI_BUTTON		GPIO_MODE_IT_RISING_FALLING
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
#define CLOCK_ENABLE_DEBUG_UART	__HAL_RCC_GPIOA_CLK_ENABLE
#define MODE_DEBUG_UART_TX	GPIO_MODE_AF_PP
#define MODE_DEBUG_UART_RX	GPIO_MODE_INPUT
#define PORT_DEBUG_UART		GPIOA
#define PIN_DEBUG_UART_TX	GPIO_PIN_2
#define PIN_DEBUG_UART_RX	GPIO_PIN_3

/***** Sensor ADC GPIO. *****/
#define CLOCK_ENABLE_SENSOR_ADC	__HAL_RCC_GPIOA_CLK_ENABLE
#define MODE_SENSOR_ADC GPIO_MODE_ANALOG
#define PORT_SENSOR_ADC	GPIOA
#define PIN_SENSOR_ADC_0 GPIO_PIN_1

/***** Level Sensor GPIO. *****/
#define CLOCK_ENABLE_LEVEL_SENSOR __HAL_RCC_GPIOB_CLK_ENABLE
#define MODE_LEVEL_SENSOR GPIO_MODE_OUTPUT_PP
#define PORT_LEVEL_SENSOR GPIOB
#define PIN_LEVEL_SENSOR GPIO_PIN_1

/***** Shiftregister SPI GPIO. *****/

void GpioShiftregSpiInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	CLOCK_ENABLE_SHIFTREG_SPI();

	/**SPI1 GPIO Configuration
	PA4     ------> SPI1_NSS
	PA5     ------> SPI1_SCK
	PA7     ------> SPI1_MOSI
	*/
	GPIO_InitStruct.Pin = PIN_SHIFTREG_SPI_SCK|PIN_SHIFTREG_SPI_MOSI;
	GPIO_InitStruct.Mode = MODE_SHIFTREG_SPI;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(PORT_SHIFTREG_SPI, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PIN_SHIFTREG_SPI_NSS;
	GPIO_InitStruct.Mode = MODE_SHIFTREG_SPI_NSS;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(PORT_SHIFTREG_SPI, &GPIO_InitStruct);
}

void GpioShiftregSpiDeinit(void)
{
	/**SPI1 GPIO Configuration
	PA4     ------> SPI1_NSS
	PA5     ------> SPI1_SCK
	PA7     ------> SPI1_MOSI
	*/
	HAL_GPIO_DeInit(PORT_SHIFTREG_SPI, PIN_SHIFTREG_SPI_NSS|PIN_SHIFTREG_SPI_SCK|PIN_SHIFTREG_SPI_MOSI);
}

void GpioShiftregSpiNssStateSet(uint8_t state)
{
	if(state) {
		HAL_GPIO_WritePin(PORT_SHIFTREG_SPI, PIN_SHIFTREG_SPI_NSS, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(PORT_SHIFTREG_SPI, PIN_SHIFTREG_SPI_NSS, GPIO_PIN_RESET);
	}

}

/***** 7 Segment Display select GPIO. *****/

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

/***** LCD Display control GPIO. *****/

void GpioLcdCtrlInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	CLOCK_ENABLE_LCD_GPIO();

	HAL_GPIO_WritePin(PORT_LCD_CTRL, PIN_LCD_CTRL_RS|PIN_LCD_CTRL_RW|
			PIN_LCD_CTRL_ENABLE|PIN_LCD_CTRL_BACKLIGHT, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin =  PIN_LCD_CTRL_RS|PIN_LCD_CTRL_RW|PIN_LCD_CTRL_ENABLE|
			PIN_LCD_CTRL_BACKLIGHT;
	GPIO_InitStruct.Mode = MODE_LCD_CTRL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(PORT_LCD_CTRL, &GPIO_InitStruct);
}

void GpioLcdCtrlDeinit(void)
{
	HAL_GPIO_DeInit(MODE_LCD_CTRL, PIN_LCD_CTRL_RS|PIN_LCD_CTRL_RW|
			PIN_LCD_CTRL_ENABLE|PIN_LCD_CTRL_BACKLIGHT);
}

void GpioLcdCtrlRsStateSet(uint8_t state)
{
	if(state) {
		HAL_GPIO_WritePin(PORT_LCD_CTRL, PIN_LCD_CTRL_RS, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(PORT_LCD_CTRL, PIN_LCD_CTRL_RS, GPIO_PIN_RESET);
	}
}

void GpioLcdCtrlRwStateSet(uint8_t state)
{
	if(state) {
		HAL_GPIO_WritePin(PORT_LCD_CTRL, PIN_LCD_CTRL_RW, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(PORT_LCD_CTRL, PIN_LCD_CTRL_RW, GPIO_PIN_RESET);
	}
}

void GpioLcdCtrlEnableStateSet(uint8_t state)
{
	if(state) {
		HAL_GPIO_WritePin(PORT_LCD_CTRL, PIN_LCD_CTRL_ENABLE, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(PORT_LCD_CTRL, PIN_LCD_CTRL_ENABLE, GPIO_PIN_RESET);
	}
}

void GpioLcdCtrlBacklightStateSet(uint8_t state)
{
	if(state) {
		HAL_GPIO_WritePin(PORT_LCD_CTRL, PIN_LCD_CTRL_BACKLIGHT, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(PORT_LCD_CTRL, PIN_LCD_CTRL_BACKLIGHT, GPIO_PIN_RESET);
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
	NvicInterruptPrioritySet(IRQ_UI_BUTTON, UI_BUTTON_IRQ_PRIO, 0);
}

void GpioUiButtonDeinit(void)
{
	HAL_GPIO_DeInit(PORT_UI_BUTTON, PIN_UI_BUTTON_INC|PIN_UI_BUTTON_DEC|PIN_UI_BUTTON_SEL);
}

void GpioIntUiButtonEnable(uint8_t en)
{
	NvicInterruptEnable(IRQ_UI_BUTTON, en);
}

uint16_t GpioPinGetButton(uint8_t btn)
{
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

	return pin;
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

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin, uint8_t state)
{
	uint8_t btn = (uint8_t)GPIO_Pin;
	GpioIntCallback_t cb = NULL;

	switch(btn) {
	default:
	case UI_BUTTON_INC: {
		cb = UiButtonIncCb;
		break;
	}
	case UI_BUTTON_DEC: {
		cb = UiButtonDecCb;
		break;
	}
	case UI_BUTTON_SEL: {
		cb = UiButtonSelCb;
		break;
	}
	}

	if(cb != NULL) {
		cb(GPIO_Pin, state);
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

	CLOCK_ENABLE_DEBUG_UART();

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

/***** Sensor ADC GPIO *****/

void GpioSensorAdcInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	CLOCK_ENABLE_SENSOR_ADC();

    GPIO_InitStruct.Pin = PIN_SENSOR_ADC_0;
    GPIO_InitStruct.Mode = MODE_SENSOR_ADC;
    HAL_GPIO_Init(PORT_SENSOR_ADC, &GPIO_InitStruct);
}

void GpioSensorAdcDeinit(void)
{
	HAL_GPIO_DeInit(PORT_SENSOR_ADC, PIN_SENSOR_ADC_0);
}

/***** Level Sensor GPIO *****/

void GpioLevelSensorInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	CLOCK_ENABLE_LEVEL_SENSOR();

    GPIO_InitStruct.Pin = PIN_LEVEL_SENSOR;
    GPIO_InitStruct.Mode = MODE_LEVEL_SENSOR;
    HAL_GPIO_Init(PORT_LEVEL_SENSOR, &GPIO_InitStruct);
}

void GpioLevelSensorDeinit(void)
{
	HAL_GPIO_DeInit(PORT_LEVEL_SENSOR, PIN_LEVEL_SENSOR);
}

uint8_t GpioLevelSensorStateGet(void)
{
	return HAL_GPIO_ReadPin(PORT_LEVEL_SENSOR, PIN_LEVEL_SENSOR);
}


