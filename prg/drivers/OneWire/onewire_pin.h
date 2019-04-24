/**
 * @file onewire_pin.h
 * @brief Pin abstraction struct.
 * @author Dorus van de Spijker
 */

#ifndef ONE_WIRE_PIN_H_
#define ONE_WIRE_PIN_H_

#include <stdint.h>

typedef void (*ow_pin_fn_generic_t)(void);
typedef void (*ow_pin_fn_write_t)(uint8_t b);
typedef uint8_t (*ow_pin_fn_read_t)(void);

typedef struct {
	ow_pin_fn_generic_t	input;
	ow_pin_fn_generic_t output;
	ow_pin_fn_write_t	write;
	ow_pin_fn_read_t	read;
} ow_pin_t;

#endif /* ONE_WIRE_PIN_H_ */
