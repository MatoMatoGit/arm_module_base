/*
 * seven_seg_display.c
 *
 *  Created on: 1 jul. 2018
 *      Author: Dorus
 */

#include "seven_seg.h"

#include <string.h>
#include <stdlib.h>

#define DIGIT_NUM_CHARS 16
#define DIGIT_SEG_INVALID 0xFF

const uint8_t DigitSegLut[DIGIT_NUM_CHARS] = {
	DIGIT_0,
	DIGIT_1,
	DIGIT_2,
	DIGIT_3,
	DIGIT_4,
	DIGIT_5,
	DIGIT_6,
	DIGIT_7,
	DIGIT_8,
	DIGIT_9,
	DIGIT_A,
	DIGIT_B,
	DIGIT_C,
	DIGIT_D,
	DIGIT_E,
	DIGIT_F,
};

static uint8_t DisplayDigits[SEVEN_SEG_CONFIG_DIGITS_MAX];

static struct SevenSegDisplay DisplayConfig;

static uint8_t IConvertBcdToSevenSegment(uint8_t bcd);
static void IDigitDeselectAll(void);
static void IDisplayDisable(void);
static void IDisplayEnable(void);

int SevenSegDisplayInit(struct SevenSegDisplay *config)
{
	int res = SEVEN_SEG_RES_INV_ARG;

	if(config != NULL) { /* Check config argument, hal.digit_set is a required function. */
		if( (config->hal.digit_set != NULL) && (config->num_digits <= SEVEN_SEG_CONFIG_DIGITS_MAX) ) {
				res = SEVEN_SEG_RES_OK;

				memset(&DisplayConfig, 0, sizeof(struct SevenSegDisplay));
				DisplayConfig = *config; /* Copy the configuration struct. */

				/* Set all the digits to 0. */
				for(uint8_t i = 0; i < SEVEN_SEG_CONFIG_DIGITS_MAX; i++) {
					DisplayDigits[i] = 0;
				}
				/* Call the HAL init function if it exists. */
				if(DisplayConfig.hal.init != NULL) {
					DisplayConfig.hal.init(DisplayConfig.refresh_rate_hz);
				}

				/* Disable the display. */
				IDisplayDisable();

				if(DisplayConfig.hal.timer_start != NULL) {
					DisplayConfig.hal.timer_start();
				}
		}
	}

	return res;
}

void SevenSegDisplayEnable(uint8_t val)
{
	if(val) {
		IDisplayEnable();
	} else {
		IDisplayDisable();
	}
}

int SevenSegDisplaySet(uint32_t val)
{
	int res = SEVEN_SEG_RES_INV_ARG;
	uint8_t digit = 0;
	uint8_t i = 0;

	if(val <= DisplayConfig.max_value) {
		res = SEVEN_SEG_RES_OK;

		while(val > 0 && i < DisplayConfig.num_digits && res == SEVEN_SEG_RES_OK) {
			digit = val % (uint8_t)DisplayConfig.mode; /* Isolate a single digit of the value, e.g. 21 % 10 = 1*/
			res = SevenSegDigitSet(i, digit); /* Set the digit. */
			val /= 10; /*Move to the next digit in the value, e.g. 21 / 10 = 2. */
			i++;
		}

		/* If val is not 0, the value is too large to fit on the display
		 * with the available number of digits. */
		if(val != 0) {
			res = SEVEN_SEG_RES_ERR;
		}

		/* If not all digits value have been set, e.g. 21 only fills 2 digits,
		 * set unused digits to 0. */
		if(res == SEVEN_SEG_RES_OK && i < DisplayConfig.num_digits) {
			while(i < DisplayConfig.num_digits && res == SEVEN_SEG_RES_OK) {
				res = SevenSegDigitSet(i, 0);
				i++;
			}
		}

	}

	return res;
}

int SevenSegDigitSet(uint8_t digit_num, uint8_t val)
{
	int res = SEVEN_SEG_RES_INV_ARG;
	uint8_t seg = 0;

	if(val <= DisplayConfig.mode) {
		res = SEVEN_SEG_RES_ERR;

		seg = IConvertBcdToSevenSegment(val);
		if(seg != DIGIT_SEG_INVALID) {
			DisplayDigits[digit_num] = seg;
			res = SEVEN_SEG_RES_OK;
		}
	}

	return res;
}

void SevenSegHalCallbackDisplayUpdate(void)
{
	static uint8_t current_digit = 0;

	if(DisplayConfig.enabled) {
		/* De-select the current digit, turning it off. */
		if(DisplayConfig.hal.digit_deselect != NULL) {
			DisplayConfig.hal.digit_deselect(current_digit);
		}

		/* Move to next digit, check if last digit has been reached if this
		 * is the case go back to the first digit. */
		current_digit++;
		if(current_digit > (DisplayConfig.num_digits - 1)) {
			current_digit = 0;
		}

		/* Set the digit value.
		 * Note that no NULL check if necessary because it is checked
		 * in SevenSegInit. */
		DisplayConfig.hal.digit_set(current_digit, DisplayDigits[current_digit]);

		/* Select the current digit, turning it on. */
		if(DisplayConfig.hal.digit_select != NULL) {
			DisplayConfig.hal.digit_select(current_digit);
		}
	}
}


static uint8_t IConvertBcdToSevenSegment(uint8_t bcd)
{
	uint8_t sev_seg = DIGIT_SEG_INVALID;

	if(bcd < DIGIT_NUM_CHARS) {
		sev_seg = DigitSegLut[bcd];
	}

	return sev_seg;
}

static void IDigitDeselectAll(void)
{
	if(DisplayConfig.hal.digit_deselect != NULL) {
		for(uint8_t i = 0; i < DisplayConfig.num_digits; i++) {
			DisplayConfig.hal.digit_deselect(i);
		}
	}
}

static void IDisplayDisable(void)
{
	DisplayConfig.enabled = 0;
	IDigitDeselectAll();
}

static void IDisplayEnable(void)
{
	DisplayConfig.enabled = 1;
}
