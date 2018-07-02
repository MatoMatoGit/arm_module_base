/*
 * seven_seg_display.h
 *
 *  Created on: 1 jul. 2018
 *      Author: Dorus
 */

#ifndef SEVEN_SEG_DISPLAY_H_
#define SEVEN_SEG_DISPLAY_H_

#include <stdint.h>

/* Seven Segment Display driver. */

#define SEVEN_SEG_CONFIG_DIGITS_MAX 5

#define SEVEN_SEG_RES_OK		0
#define SEVEN_SEG_RES_INV_ARG	-1
#define SEVEN_SEG_RES_ERR		-2

#define DIGIT_SEG_A	0x01
#define DIGIT_SEG_B	0x02
#define DIGIT_SEG_C	0x04
#define DIGIT_SEG_D	0x08
#define DIGIT_SEG_E	0x10
#define DIGIT_SEG_F	0x20
#define DIGIT_SEG_G	0x40
#define DIGIT_SEG_DOT	0x80

#define DIGIT_0 ( DIGIT_SEG_A | DIGIT_SEG_B | DIGIT_SEG_C | DIGIT_SEG_D |DIGIT_SEG_E | DIGIT_SEG_F )
#define DIGIT_1 ( DIGIT_SEG_B | DIGIT_SEG_C )
#define DIGIT_2 ( DIGIT_SEG_A | DIGIT_SEG_B | DIGIT_SEG_D | DIGIT_SEG_E | DIGIT_SEG_G )
#define DIGIT_3 ( DIGIT_SEG_A | DIGIT_SEG_B | DIGIT_SEG_C | DIGIT_SEG_D | DIGIT_SEG_G )
#define DIGIT_4 ( DIGIT_SEG_B | DIGIT_SEG_C | DIGIT_SEG_F | DIGIT_SEG_G )
#define DIGIT_5 ( DIGIT_SEG_A | DIGIT_SEG_C | DIGIT_SEG_D | DIGIT_SEG_F, DIGIT_SEG_G )
#define DIGIT_6 ( DIGIT_SEG_A | DIGIT_SEG_C | DIGIT_SEG_D | DIGIT_SEG_E |DIGIT_SEG_F | DIGIT_SEG_G )
#define DIGIT_7 ( DIGIT_SEG_A | DIGIT_SEG_B | DIGIT_SEG_C )
#define DIGIT_8 ( DIGIT_SEG_A | DIGIT_SEG_B | DIGIT_SEG_C | DIGIT_SEG_D |DIGIT_SEG_E | DIGIT_SEG_F | DIGIT_SEG_G )
#define DIGIT_9 ( DIGIT_SEG_A | DIGIT_SEG_B | DIGIT_SEG_C | DIGIT_SEG_F | DIGIT_SEG_G )
#define DIGIT_A ( DIGIT_SEG_A | DIGIT_SEG_B | DIGIT_SEG_C | DIGIT_SEG_E | DIGIT_SEG_F | DIGIT_SEG_G )
#define DIGIT_B ( DIGIT_SEG_C | DIGIT_SEG_D |DIGIT_SEG_E | DIGIT_SEG_F | DIGIT_SEG_G )
#define DIGIT_C ( DIGIT_SEG_A | DIGIT_SEG_D | DIGIT_SEG_E | DIGIT_SEG_F )
#define DIGIT_D ( DIGIT_SEG_B | DIGIT_SEG_C | DIGIT_SEG_D | DIGIT_SEG_E | DIGIT_SEG_G)
#define DIGIT_E ( DIGIT_SEG_A | DIGIT_SEG_D |DIGIT_SEG_E | DIGIT_SEG_F | DIGIT_SEG_G )
#define DIGIT_F ( DIGIT_SEG_A | DIGIT_SEG_E | DIGIT_SEG_F | DIGIT_SEG_G )

typedef enum {
	SEVEN_SEG_MODE_DEC = 0x9,
	SEVEN_SEG_MODE_HEX = 0xF
}SevenSegMode_t;

/* Called by the driver to update a digit. The segment
 * bits that are set in the 'segments' argument must be
 * turned on.
 * The 0th digit is the least significant one. */
typedef int (*SevenSegDigitUpdateFunc_t) (uint8_t digit_num, uint8_t segments);

/* The number of digits of the display is defined by
 * 'num_digits'. */
struct SevenSegDisplay {
	uint8_t num_digits;
	uint32_t max_value;
	SevenSegMode_t mode;
	uint16_t refresh_rate_hz;
	SevenSegDigitUpdateFunc_t update;
};

int SevenSegInit(struct SevenSegDisplay *config);

int SevenSegDisplayUpdate(uint32_t value);

int SevenSegDigitUpdate(uint8_t digit_num, uint8_t value);

#endif /* SEVEN_SEG_DISPLAY_H_ */
