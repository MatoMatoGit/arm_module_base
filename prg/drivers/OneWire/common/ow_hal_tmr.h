/*
 * ow_hal_tmr.h
 *
 *  Created on: May 1, 2019
 *      Author: dorus
 */

#ifndef ONEWIRE_HAL_TMR_H_
#define ONEWIRE_HAL_TMR_H_

#include "ow_hal_common.h"
#include <stdint.h>

typedef uint32_t (*ow_hal_tmr_fn_time_t) (void);

typedef struct {
	ow_hal_fn_generic_t		tmr_init;
	ow_hal_tmr_fn_time_t	tmr_get_us;
} ow_hal_tmr_t;

#endif /* ONEWIRE_HAL_TMR_H_ */
