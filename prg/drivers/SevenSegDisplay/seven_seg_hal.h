#ifndef SEVEN_SEG_HAL_H_
#define SEVEN_SEG_HAL_H_

#include <stdint.h>

typedef void (*SevenSegHalGeneric_t) (void);

typedef int (*SevenSegHalInit_t) (uint32_t interval_ms);

typedef void (*SevenSegHalDigitSelect_t) (uint8_t digit_num);

typedef void (*SevenSegHalDigitSet_t) (uint8_t digit_num, uint8_t segments);

/* Must be called periodically at the initialized interval. */
extern void SevenSegHalCallbackDisplayUpdate(void);

typedef struct {
	/* Initialize the driver HAL. */
	SevenSegHalInit_t init;

	/* Start the digit multiplexing timer. */
	SevenSegHalGeneric_t timer_start;

	/* Select a digit. */
	SevenSegHalDigitSelect_t digit_select;

	/* De-select a digit. */
	SevenSegHalDigitSelect_t digit_deselect;

	/* Set a digit to a specified value. The segment
	 * bits that are set in the 'segments' argument must be
	 * turned on.
	 * The 0th digit is the least significant one. */
	SevenSegHalDigitSet_t digit_set;

}SevenSegHal_t;


#endif

