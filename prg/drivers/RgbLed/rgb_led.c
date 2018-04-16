#include "gpio.h"
#include "rgb_led.h"

RgbLedColor_t CurrentColor;

/* Init RGB LED, start at color = OFF. */
void RgbLedInit(void)
{
	GpioLedInit();
	RgbLedColorSet(RGB_LED_COLOR_OFF);
}

/* Set RGB LED color. */
void RgbLedColorSet(RgbLedColor_t color)
{
	CurrentColor = color;
	switch (color) {
		case RGB_LED_COLOR_OFF:{
#ifdef RGB_LED_CONTROL_INVERTED
			GpioLedStateRed(1);
			GpioLedStateGreen(1);
			GpioLedStateBlue(1);
#else
			GpioLedStateRed(0);
			GpioLedStateGreen(0);
			GpioLedStateBlue(0);
#endif
			break;
		}

		case RGB_LED_COLOR_RED: {
#ifdef RGB_LED_CONTROL_INVERTED
			GpioLedStateRed(0);
			GpioLedStateGreen(1);
			GpioLedStateBlue(1);
#else
			GpioLedStateRed(1);
			GpioLedStateGreen(0);
			GpioLedStateBlue(0);
#endif
			break;
		}

		case RGB_LED_COLOR_GREEN: {
#ifdef RGB_LED_CONTROL_INVERTED
			GpioLedStateRed(1);
			GpioLedStateGreen(0);
			GpioLedStateBlue(1);
#else
			GpioLedStateRed(0);
			GpioLedStateGreen(1);
			GpioLedStateBlue(0);
#endif
			break;
		}

		case RGB_LED_COLOR_BLUE: {
#ifdef RGB_LED_CONTROL_INVERTED
			GpioLedStateRed(1);
			GpioLedStateGreen(1);
			GpioLedStateBlue(0);
#else
			GpioLedStateRed(0);
			GpioLedStateGreen(0);
			GpioLedStateBlue(1);
#endif
			break;
		}

		case RGB_LED_COLOR_VIOLET: {
#ifdef RGB_LED_CONTROL_INVERTED
			GpioLedStateRed(0);
			GpioLedStateGreen(1);
			GpioLedStateBlue(0);
#else
			GpioLedStateRed(1);
			GpioLedStateGreen(0);
			GpioLedStateBlue(1);
#endif
			break;
		}

		case RGB_LED_COLOR_YELLOW: {
#ifdef RGB_LED_CONTROL_INVERTED
			GpioLedStateRed(0);
			GpioLedStateGreen(0);
			GpioLedStateBlue(1);
#else
			GpioLedStateRed(1);
			GpioLedStateGreen(1);
			GpioLedStateBlue(0);
#endif
			break;
		}

		case RGB_LED_COLOR_AQUA: {
#ifdef RGB_LED_CONTROL_INVERTED
			GpioLedStateRed(1);
			GpioLedStateGreen(0);
			GpioLedStateBlue(0);
#else
			GpioLedStateRed(0);
			GpioLedStateGreen(1);
			GpioLedStateBlue(1);
#endif
			break;
		}

		case RGB_LED_COLOR_WHITE: {
#ifdef RGB_LED_CONTROL_INVERTED
			GpioLedStateRed(0);
			GpioLedStateGreen(0);
			GpioLedStateBlue(0);
#else
			GpioLedStateRed(1);
			GpioLedStateGreen(1);
			GpioLedStateBlue(1);
#endif
			break;
		}
		
		default: {
#ifdef RGB_LED_CONTROL_INVERTED
			GpioLedStateRed(1);
			GpioLedStateGreen(1);
			GpioLedStateBlue(1);
#else
			GpioLedStateRed(0);
			GpioLedStateGreen(0);
			GpioLedStateBlue(0);
#endif
		}
	}
}

/* Get current RGB LED color. */
RgbLedColor_t RgbLedColorGet(void)
{
	return CurrentColor;
}


