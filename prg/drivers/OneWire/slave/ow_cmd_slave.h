/*
 * ow_cmd_slave.h
 *
 *  Created on: May 2, 2019
 *      Author: dorus
 */

#ifndef ONEWIRE_CMD_SLAVE_H_
#define ONEWIRE_CMD_SLAVE_H_

#include "ow_common.h"
#include "ow_hal_slave.h"

typedef uint8_t (*ow_cmd_slave_cb_rtx_t) (uint8_t rx_data, uint8_t *tx_data);

typedef struct {
	ow_cb_generic_t reset;
	ow_cb_generic_t	error;
	ow_cmd_slave_cb_rtx_t receive_transmit_data;
} ow_cmd_slave_cbs_t;

uint8_t ow_cmd_slave_init(ow_hal_slave_t *hal, ow_cmd_slave_cbs_t *cbs);

void ow_cmd_slave_start(void);

void ow_cmd_slave_stop(void);


#endif /* ONEWIRE_CMD_SLAVE_H_ */
