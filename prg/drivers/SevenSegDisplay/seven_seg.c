/*
 * seven_seg_display.c
 *
 *  Created on: 1 jul. 2018
 *      Author: Dorus
 */

#include "seven_seg.h"

#include <string.h>
#include <stdlib.h>

#define DIGIT_NUM_CHARS 0xF
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
	DIGIT_F
};

static uint8_t DisplayDigits[SEVEN_SEG_CONFIG_DIGITS_MAX];

static struct SevenSegDisplay DisplayConfig;

static uint8_t ConvertBcdToSevenSegment(uint8_t bcd);

int SevenSegInit(struct SevenSegDisplay *config)
{
	int res = SEVEN_SEG_RES_INV_ARG;

	memset(&DisplayConfig, 0, sizeof(struct SevenSegDisplay));
	if(config->update != NULL) {
		if(config->num_digits <= SEVEN_SEG_CONFIG_DIGITS_MAX) {
			DisplayConfig = *config;
			for(uint8_t i = 0; i < SEVEN_SEG_CONFIG_DIGITS_MAX; i++) {
				DisplayDigits[i] = 0;
			}
		}
	}

	return res;
}

uint32_t SevenSegDisplayMaxValueGet(void)
{
	return DisplayConfig.max_value;
}

int SevenSegDisplayUpdate(uint32_t value)
{
	int res = SEVEN_SEG_RES_INV_ARG;
	uint8_t digit = 0;
	uint8_t i = 0;

	if(value <= DisplayConfig.max_value) {
		res = SEVEN_SEG_RES_OK;
		while(value > 0 && i < DisplayConfig.num_digits && res == SEVEN_SEG_RES_OK) {
			digit = value % (uint8_t)DisplayConfig.mode;
			res = SevenSegDigitUpdate(i, digit);
			value /= 10;
			i++;
		}
		if(value != 0) {
			res = SEVEN_SEG_RES_ERR;
		}
	}

	return res;
}

int SevenSegDigitUpdate(uint8_t digit_num, uint8_t value)
{
	int res = SEVEN_SEG_RES_INV_ARG;
	uint8_t seg = 0;

	if(value <= DisplayConfig.mode) {
		res = SEVEN_SEG_RES_ERR;
		seg = ConvertBcdToSevenSegment(value);
		if(seg != DIGIT_SEG_INVALID) {
			DisplayDigits[digit_num] = seg;
			res = SEVEN_SEG_RES_OK;
		}
	}

	return res;
}



static uint8_t ConvertBcdToSevenSegment(uint8_t bcd)
{
	uint8_t sev_seg = DIGIT_SEG_INVALID;

	if(bcd < DIGIT_NUM_CHARS) {
		sev_seg = DigitSegLut[bcd];
	}

	return sev_seg;
}
