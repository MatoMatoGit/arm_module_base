/**
 * @file onewire_hal_io.h
 * @brief 1-Wire IO hardware abstraction.
 * @author Dorus van de Spijker
 */

#ifndef ONEWIRE_HAL_IO_H_
#define ONEWIRE_HAL_IO_H_

#include "ow_hal_common.h"

#include <stdint.h>

typedef void (*ow_hal_fn_io_write_t)(uint8_t bit);
typedef uint8_t (*ow_hal_fn_io_read_t)(void);

typedef struct {
	ow_hal_fn_generic_t	input;
	ow_hal_fn_generic_t output;
	ow_hal_fn_io_write_t write;
	ow_hal_fn_io_read_t	read;
} ow_hal_io_t;

#endif /* ONEWIRE_HAL_IO_H_ */
