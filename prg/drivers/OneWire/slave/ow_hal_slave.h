/*
 * ow_hal_slave.h
 *
 *  Created on: May 1, 2019
 *      Author: dorus
 */

#ifndef ONEWIRE_HAL_SLAVE_H_
#define ONEWIRE_HAL_SLAVE_H_

#include "ow_hal_io.h"
#include "ow_hal_int.h"
#include "ow_hal_general.h"

typedef struct {
	ow_hal_int_t	*hal_int;
	ow_hal_io_t		*hal_io;
} ow_hal_slave_t;

#endif /* ONEWIRE_HAL_SLAVE_H_ */
