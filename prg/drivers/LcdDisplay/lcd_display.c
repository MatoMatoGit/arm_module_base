/*
 * lcd_display.c
 *
 *  Created on: Mar 2, 2019
 *      Author: dorus
 */


#include "lcd_display.h"
#include "lcd_display_config.h"

#include "gpio.h"

#define AsmDelayUs(us) do {\
	asm volatile (	"MOV R0,%[loops]\n\t"\
			"1: \n\t"\
			"SUB R0, #1\n\t"\
			"CMP R0, #0\n\t"\
			"BNE 1b \n\t" : : [loops] "r" (16*us) : "memory"\
		      );\
} while(0)

static HD44780_Result (ILcdDisplayDataWrite)(HD44780_GPIO_Interface *interface,
  uint8_t data);

static HD44780_Result ILcdDisplayCtrlWrite(HD44780_GPIO_Interface *interface,
    HD44780_Pin pin, HD44780_PinState value);

static void DelayUs(uint16_t us);

static HD44780 LcdDisplay;

static HD44780_GPIO_Interface LcdDisplayGpioConfig = {
	.configure = NULL,
	.write_data = ILcdDisplayDataWrite,
	.write_ctrl = ILcdDisplayCtrlWrite,
	.read_data = NULL,
};

static const HD44780_Config LcdDisplayConfig = {
	.gpios = &LcdDisplayGpioConfig,
	.delay_microseconds = DelayUs,
	.assert_failure_handler = NULL,
	.options = HD44780_OPT_USE_BACKLIGHT,
};



HD44780_Result hd44780_init(HD44780 *display, HD44780_Mode mode,
  const HD44780_Config *config, uint8_t columns, uint8_t rows, HD44780_CharSize charsize);

SysResult_t LcdDisplayInit(void)
{
	SysResult_t res = SYS_RESULT_ERROR;

	GpioLcdCtrlInit();
	GpioLcdDataInit();
	if(hd44780_init(&LcdDisplay, HD44780_MODE_4BIT, &LcdDisplayConfig,
			LCD_DISPLAY_CONFIG_NUM_COLUMNS, LCD_DISPLAY_CONFIG_NUM_ROWS,
			HD44780_CHARSIZE_5x10) == HD44780_RESULT_OK) {
		res = SYS_RESULT_OK;
	}

	return res;
}

HD44780 *LcdDisplayHandleGet(void)
{
	return &LcdDisplay;
}

static HD44780_Result ILcdDisplayDataWrite(HD44780_GPIO_Interface *interface,
  uint8_t data)
{
	(void) interface;

	GpioLcdDataWrite(data);

	return HD44780_RESULT_OK;
}

static HD44780_Result ILcdDisplayCtrlWrite(HD44780_GPIO_Interface *interface,
    HD44780_Pin pin, HD44780_PinState value)
{
	(void) interface;

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

static void DelayUs(uint16_t us)
{
	AsmDelayUs(us);
}
