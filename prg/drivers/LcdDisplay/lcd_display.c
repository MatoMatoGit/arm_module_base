/*
 * lcd_display.c
 *
 *  Created on: Mar 2, 2019
 *      Author: dorus
 */


#include "lcd_display.h"
#include "lcd_display_config.h"

#include "gpio.h"
#include "spi.h"

static HD44780_Result (ILcdDisplayDataWrite)(HD44780_GPIO_Interface *interface,
  uint8_t data);

static HD44780_Result ILcdDisplayCtrlWrite(HD44780_GPIO_Interface *interface,
    HD44780_Pin pin, HD44780_PinState value);

static HD44780 LcdDisplay;

static HD44780_GPIO_Interface LcdDisplayGpioConfig = {
	.configure = NULL,
	.write_data = ILcdDisplayDataWrite,
	.write_ctrl = ILcdDisplayCtrlWrite,
	.read = NULL,
};

static const HD44780_Config LcdDisplayConfig = {
	.gpios = &LcdDisplayGpioConfig,
	.delay_microseconds = NULL,
	.assert_failure_handler = NULL,
	.options = HD44780_OPT_USE_BACKLIGHT,
};



HD44780_Result hd44780_init(HD44780 *display, HD44780_Mode mode,
  const HD44780_Config *config, uint8_t columns, uint8_t rows, HD44780_CharSize charsize);

SysResult_t LcdDisplayInit(void)
{
	GpioShiftregSpiInit();
	GpioLcdCtrlInit();
	hd44780_init(&LcdDisplay, HD44780_MODE_4BIT, &LcdDisplayConfig,
			LCD_DISPLAY_CONFIG_NUM_COLUMNS, LCD_DISPLAY_CONFIG_NUM_ROWS,
			HD44780_CHARSIZE_5x10);
}

HD44780 *LcdDisplayHandleGet(void)
{
	return &LcdDisplay;
}

static HD44780_Result ILcdDisplayDataWrite(HD44780_GPIO_Interface *interface,
  uint8_t data)
{
	GpioShiftregSpiNssStateSet(0);
	HAL_SPI_Transmit(&hspi_shiftreg, &data, sizeof(data), SPI_SHIFTREG_WRITE_TIMEOUT_MS);
	GpioShiftregSpiNssStateSet(1);
}

static HD44780_Result ILcdDisplayCtrlWrite(HD44780_GPIO_Interface *interface,
    HD44780_Pin pin, HD44780_PinState value)
{
	HD44780_Result res = HD44780_RESULT_OK;

	switch(pin) {
	case HD44780_PIN_RS: {
		GpioLcdCtrlRsStateSet(value);
		break;
	}
	case HD44780_PIN_ENABLE: {
		GpioLcdCtrlEnableStateSet(value);
		break;
	}
	case HD44780_PIN_RW: {
		GpioLcdCtrlRwStateSet(value);
		break;
	}
	case HD44780_PIN_BACKLIGHT: {
		GpioLcdCtrlBacklightStateSet(value);
		break;
	}
	default: {
		res = HD44780_RESULT_ERROR;
		break;
	}
	}

	return res;
}
