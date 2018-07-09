#ifdef SEVEN_SEG_HAL_H_
#define SEVEN_SEG_HAL_H_

typedef int (*SevenSegHalTimerInit_t) (uint32_t interval_ms);

typedef int (*SevenSegHalGpioInit_t) (void);

/* Called by the driver to select a digit. */
typedef int (*SevenSegHalDigitSelect_t) (uint8_t digit_num);

/* Called by the driver to write to a digit. The segment
 * bits that are set in the 'segments' argument must be
 * turned on.
 * The 0th digit is the least significant one. */
typedef int (*SevenSegHalDigitWrite_t) (uint8_t digit_num, uint8_t segments);

extern void SevenSegHalCallbackDisplayUpdate(void);

typedef struct {
	SevenSegHalTimerInit_t timer_init;
	SevenSegHalGpioInit_t gpio_init;
	SevenSegHalDigitSelect_t digit_select;
	SevenSegHalDigitWrite_t digit_write;
}SevenSegHal_t;


#endif

