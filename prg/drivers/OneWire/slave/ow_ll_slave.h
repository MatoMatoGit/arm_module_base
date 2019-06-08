/*
 * ow_ll_slave.h
 *
 *  Created on: May 1, 2019
 *      Author: dorus
 */

#ifndef ONEWIRE_LL_SLAVE_H_
#define ONEWIRE_LL_SLAVE_H_

#include "ow_hal_slave.h"
#include "ow_hal_common.h"
#include "ow_common.h"

typedef uint8_t (*ow_ll_slave_cb_rtx_t) (uint8_t bit);

typedef struct {
	ow_cb_generic_t			reset;
	ow_cb_generic_t			error;
	ow_ll_slave_cb_rtx_t	receive_transmit_bit;
} ow_ll_slave_cbs_t;

uint8_t ow_ll_slave_init(ow_hal_slave_t *hal, ow_ll_slave_cbs_t *cbs);

void ow_ll_slave_start(void);

void ow_ll_slave_stop(void);

#endif /* ONEWIRE_LL_SLAVE_H_ */
