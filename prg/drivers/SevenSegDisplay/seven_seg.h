/*
 * seven_seg_display.h
 *
 *  Created on: 1 jul. 2018
 *      Author: Dorus
 */

#ifndef SEVEN_SEG_H_
#define SEVEN_SEG_H_

#include "seven_seg_common.h"
#include "seven_seg_hal.h"

#include <stdint.h>

/* Seven Segment Display driver. */

typedef enum {
	SEVEN_SEG_MODE_DEC = 0x9,
	SEVEN_SEG_MODE_HEX = 0xF
}SevenSegMode_t;

/* The number of digits of the display is defined by
 * 'num_digits'. */
struct SevenSegDisplay {
	uint8_t num_digits;
	uint32_t max_value;
	SevenSegMode_t mode;
	uint16_t refresh_rate_hz;
	SevenSegHal_t hal;
};

int SevenSegInit(struct SevenSegDisplay *config);

int SevenSegDisplayUpdate(uint32_t value);

int SevenSegDigitUpdate(uint8_t digit_num, uint8_t value);

#endif /* SEVEN_SEG_H_ */
