/*
 * ow_hal_master.h
 *
 *  Created on: May 1, 2019
 *      Author: dorus
 */

#ifndef ONEWIRE_HAL_MASTER_H_
#define ONEWIRE_HAL_MASTER_H_

#include "ow_hal_io.h"
#include "ow_hal_general.h"

typedef struct {
	ow_hal_io_t			hal_io;
	ow_hal_general_t	hal_gen;
} ow_hal_master_t;

#endif /* ONEWIRE_HAL_MASTER_H_ */
