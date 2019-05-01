/*
 * ow_ll_slave.h
 *
 *  Created on: May 1, 2019
 *      Author: dorus
 */

#ifndef ONEWIRE_LL_SLAVE_H_
#define ONEWIRE_LL_SLAVE_H_

#include "ow_hal_slave.h"

typedef void (*ow_ll_slave_cb_generic_t) (void);
typedef uint8_t (*ow_ll_slave_cb_rtx_t) (uint8_t bit);

typedef struct {
	ow_ll_slave_cb_generic_t	reset;
	ow_ll_slave_cb_rtx_t		receive_transmit_bit;
} ow_ll_slave_cbs_t;

void ow_ll_slave_init(ow_hal_slave_t *hal, ow_ll_slave_cbs_t *cbs);

void ow_ll_slave_int_handler(void);

#endif /* ONEWIRE_LL_SLAVE_H_ */
