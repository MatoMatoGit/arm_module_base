/**
 * @file onewire_io.h
 * @brief 1-Wire IO abstraction.
 * @author Dorus van de Spijker
 */

#ifndef ONEWIRE_IO_H_
#define ONEWIRE_IO_H_

#include <stdint.h>

typedef void (*ow_io_fn_generic_t)(void);
typedef void (*ow_io_fn_write_t)(uint8_t bit);
typedef uint8_t (*ow_io_fn_read_t)(void);

typedef struct {
	ow_io_fn_generic_t	input;
	ow_io_fn_generic_t	output;
	ow_io_fn_write_t	write;
	ow_io_fn_read_t		read;
} ow_io_t;

#endif /* ONEWIRE_IO_H_ */
