
#include "seven_seg_port.h"



int SevenSegPortTimerInit(uint32_t interval_ms)
{
	
}

int SevenSegPortGpioInit(void)
{
	
}

int SevenSegPortDigitSelect(uint8_t digit_num)
{
	
}

int SevenSegPortDigitWrite(uint8_t digit_num, uint8_t segments)
{
	
}

static void SevenSegCallbackTimer(void)
{
	SevenSegHalCallbackDisplayUpdate();
}

SevenSegHal_t SevenSegPort = {
	.timer_init = SevenSegPortTimerInit,
	.gpio_init = SevenSegPortGpioInit,
	.digit_select = SevenSegPortDigitSelect,
	.digit_write = SevenSegPortDigitWrite,
};

