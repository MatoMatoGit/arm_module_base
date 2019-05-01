/*
 * ow_hal_int.h
 *
 *  Created on: May 1, 2019
 *      Author: dorus
 */

#ifndef ONEWIRE_HAL_INT_H_
#define ONEWIRE_HAL_INT_H_

#include "ow_hal_common.h"

typedef struct {
	ow_hal_fn_generic_t	int_init;
	ow_hal_fn_generic_t	int_enable;
	ow_hal_fn_generic_t	int_disable;
	ow_hal_fn_generic_t	int_set_falling;
	ow_hal_fn_generic_t	int_set_rising;
} ow_hal_int_t;

#endif /* ONEWIRE_HAL_INT_H_ */
