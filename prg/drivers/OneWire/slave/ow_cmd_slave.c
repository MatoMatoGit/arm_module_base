/*
 * ow_cmd_slave.c
 *
 *  Created on: May 2, 2019
 *      Author: dorus
 */

#include "ow_cmd_slave.h"
#include "ow_ll_slave.h"
#include "crc8.h"

#include <string.h> /* For memset. */

typedef enum {
	CMD_STATE_RECEIVE = 0,
	CMD_STATE_RESPOND,
	CMD_STATE_CRC,
} ow_cmd_state_e;

typedef struct {
	ow_cmd_slave_cbs_t *cbs;
	ow_cmd_state_e state;
	uint8_t rx_data;
	uint8_t *tx_data;
	uint16_t tx_cnt;
	uint16_t tx_len;
	uint8_t bit_cnt;	
	uint8_t crc;
} ow_cmd_slave_t;

static uint8_t ow_cmd_slave_receive_transmit_bit(uint8_t bit);
static uint16_t ow_cmd_dispatch(uint8_t rx_data, uint8_t **tx_data);
static void ow_ll_error(void);
static void ow_ll_reset(void);
static void ow_cmd_slave_reset(void);
static void ow_cmd_slave_error(void);

static void shift_bit_in(uint8_t *byte, uint8_t bit, uint8_t n);
static uint8_t shift_bit_out(uint8_t byte, uint8_t n);

static ow_ll_slave_cbs_t ow_ll_slave_cbs = {
	.receive_transmit_bit = ow_cmd_slave_receive_transmit_bit,
};

static ow_cmd_slave_t ow_cmd_slave;

uint8_t ow_cmd_slave_init(ow_hal_slave_t *hal, ow_cmd_slave_cbs_t *cbs)
{
	if(hal == NULL || cbs == NULL) {
		return OW_ERROR;
	}
	
	memset(&ow_cmd_slave, 0, sizeof(ow_cmd_slave));
	ow_ll_slave_cbs.error = ow_ll_error;
	ow_ll_slave_cbs.reset = ow_ll_reset;
	ow_cmd_slave.cbs = cbs;
	
	return ow_ll_slave_init(hal, &ow_ll_slave_cbs);
}

void ow_cmd_slave_start(void)
{
	ow_ll_slave_start();
}

void ow_cmd_slave_stop(void)
{
	ow_ll_slave_stop();
}

uint8_t ow_cmd_slave_receive_transmit_bit(uint8_t bit)
{	
	switch(ow_cmd_slave.state) {
		/* Receive data or a command.
		 * This is typically a single byte. */
		case CMD_STATE_RECEIVE: {
			/* Shift the receives bits in to a byte. */
			shift_bit_in(&ow_cmd_slave.rx_data, bit, ow_cmd_slave.bit_cnt);
			ow_cmd_slave.bit_cnt++;
			if(ow_cmd_slave.bit_cnt >= OW_CMD_SIZE) {
				ow_cmd_slave.bit_cnt = 0;
				
				ow_cmd_slave.tx_len = ow_cmd_dispatch(ow_cmd_slave.rx_data, &ow_cmd_slave.tx_data);
				/* If tx_len is greater than zero go to respond state. */
				if(ow_cmd_slave.tx_len) {
					ow_cmd_slave.state = CMD_STATE_RESPOND;
				}
			}
			break;
		}
		
		/* Transmit data as a response to
		 * a received command. */
		case CMD_STATE_RESPOND: {
			/* A bit can only be transmitted when a 1 has
			 * been received from the master node. */
			if(bit == 1) {
				/* Shift a bit out of the current tx_data byte. */
				bit = shift_bit_out(ow_cmd_slave.tx_data[ow_cmd_slave.tx_cnt], ow_cmd_slave.bit_cnt);
				ow_cmd_slave.bit_cnt++;
				if(ow_cmd_slave.bit_cnt >= OW_CMD_SIZE) {
					ow_cmd_slave.tx_cnt++;
					ow_cmd_slave.bit_cnt = 0;
					
					/* If all data has been transmitted,
					 * transition to the CRC state. */
					if(ow_cmd_slave.tx_cnt >= ow_cmd_slave.tx_len) {
						ow_cmd_slave.tx_cnt = 0;
						ow_cmd_slave.tx_len = 0;
						ow_cmd_slave.tx_data = NULL;
						ow_cmd_slave.state = CMD_STATE_CRC;
					}
				}			
			} else {
				ow_cmd_slave_error();
			}
			break;
		}
		
		/* Transmit the CRC calculate over the tx data. */
		case CMD_STATE_CRC: {
			/* A bit can only be transmitted when a 1 has
			 * been received from the master node. */
			if(bit == 1) {
				/* Shift a bit out of the current tx_data byte. */
				bit = shift_bit_out(ow_cmd_slave.crc, ow_cmd_slave.bit_cnt);
				ow_cmd_slave.bit_cnt++;
				if(ow_cmd_slave.bit_cnt >= OW_CMD_SIZE) {
					ow_cmd_slave.bit_cnt = 0;
					ow_cmd_slave.state = CMD_STATE_RECEIVE;
				}			
			} else {
				ow_cmd_slave_error();
			}
			break;
		}
		
		default: {
			ow_cmd_slave.state = CMD_STATE_RECEIVE;
			break;
		}
	}
	
	return bit;
}

static void shift_bit_in(uint8_t *byte, uint8_t bit, uint8_t n)
{
	*byte |= (bit << n);
}

static uint8_t shift_bit_out(uint8_t byte, uint8_t n)
{
	uint8_t mask = (1 << n);
	uint8_t bit = byte & mask;
	bit = bit >> n;
	
	return bit;
}

static uint16_t ow_cmd_dispatch(uint8_t rx_data, uint8_t **tx_data)
{
	uint16_t tx_len = 0;
	
	/* Pass the received byte to the registered callback,
	 * the callee can change the tx_data pointer and return 
	 * the number of bytes to transmit. */
	switch(rx_data) {
		default: {
			tx_len = ow_cmd_slave.cbs->receive_transmit_data(rx_data, tx_data);
			break;
		}
	}
	
	if(tx_len) {
		ow_cmd_slave.crc = crc8(*tx_data, tx_len);
	}
	
	return tx_len;	
}

static void ow_ll_error(void)
{
	ow_cmd_slave_error();
}

static void ow_ll_reset(void)
{
	ow_cmd_slave_reset();
	ow_cmd_slave.cbs->reset();
}

static void ow_cmd_slave_reset(void)
{
	ow_cmd_slave.state = CMD_STATE_RECEIVE;
	ow_cmd_slave.bit_cnt = 0;
	ow_cmd_slave.crc = 0;
	ow_cmd_slave.rx_data = 0;
	ow_cmd_slave.tx_data = NULL;
	ow_cmd_slave.tx_len = 0;
	ow_cmd_slave.tx_cnt = 0;
}

static void ow_cmd_slave_error(void)
{
	ow_cmd_slave_reset();
	ow_cmd_slave.cbs->error();
}