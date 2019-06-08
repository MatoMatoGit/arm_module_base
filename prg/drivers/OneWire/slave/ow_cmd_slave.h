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

/* Called by the Slave Command library when data has been received
 * on the bus. If the receiving slave wants to send data in response,
 * the tx_data pointer must be assigned to a buffer. This buffer must
 * be available until the tx_finished or error callback is called.
 * The return value of the RTX callback must be set to the number of bytes
 * to transmit. */
typedef uint16_t (*ow_cmd_slave_cb_rtx_t) (uint8_t rx_data, uint8_t **tx_data);

typedef struct {
	ow_cb_generic_t reset;
	ow_cb_generic_t	error;
	ow_cmd_slave_cb_rtx_t receive_transmit_data;
} ow_cmd_slave_cbs_t;

uint8_t ow_cmd_slave_init(ow_hal_slave_t *hal, ow_cmd_slave_cbs_t *cbs);

void ow_cmd_slave_start(void);

void ow_cmd_slave_stop(void);


#endif /* ONEWIRE_CMD_SLAVE_H_ */
