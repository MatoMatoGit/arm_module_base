/*
 * ow_hal_general.h
 *
 *  Created on: Apr 30, 2019
 *      Author: dorus
 */

#ifndef ONEWIRE_HAL_GENERAL_H_
#define ONEWIRE_HAL_GENERAL_H_

typedef void (*ow_fn_wait_t) (uint32_t us);

typedef struct {
	ow_fn_wait_t wait_us;
} ow_hal_general_t;

#endif /* ONEWIRE_HAL_GENERAL_H_ */
