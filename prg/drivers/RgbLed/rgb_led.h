/*
 * rgb_led.h
 *
 *  Created on: 3 jan. 2018
 *      Author: Dorus
 */

#ifndef RGB_LED_H_
#define RGB_LED_H_

typedef enum {
	RGB_LED_COLOR_OFF	= 0,
	RGB_LED_COLOR_RED,
	RGB_LED_COLOR_GREEN,
	RGB_LED_COLOR_BLUE,
	RGB_LED_COLOR_VIOLET,
	RGB_LED_COLOR_YELLOW,
	RGB_LED_COLOR_AQUA,
	RGB_LED_COLOR_WHITE
}RgbLedColor_t;

#define RGB_LED_CONTROL_INVERTED

/* Init RGB LED, sets color to RGB_LED_COLOR_OFF. */
void RgbLedInit(void);

/* Set RGB LED color. */
void RgbLedColorSet(RgbLedColor_t color);

/* Get current RGB LED color. */
RgbLedColor_t RgbLedColorGet(void);

#endif /* RGB_LED_H_ */
