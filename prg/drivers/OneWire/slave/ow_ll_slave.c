
#include "ow_ll_slave.h"
#include "ow_hal_slave.h"
#include "ow_port_macro.h" 

#include <avr/io.h>

static void ow_ll_slave_write_bit(uint8_t bit);

static ow_hal_slave_t *slave_hal = NULL;
static ow_ll_slave_cbs_t *slave_cbs = NULL;

uint8_t ow_ll_slave_init(ow_hal_slave_t *hal, ow_ll_slave_cbs_t *cbs)
{
	if(hal == NULL || cbs == NULL) {
		return OW_INV_ARG;
	}

	slave_hal = hal;
	slave_cbs = cbs;

	slave_hal->hal_io->input();
	slave_hal->hal_int->int_init();

	return OW_OK;
}

void ow_ll_slave_start(void)
{
	slave_hal->hal_int->int_set_falling();
	slave_hal->hal_int->int_enable();
}

void ow_ll_slave_stop(void)
{
	slave_hal->hal_int->int_disable();
}

void ow_hal_int_handler(void)
{
	uint8_t bit = 0;
	PORTB |= (1 << PINB4);
	slave_hal->hal_int->int_disable();
	//PORTB |= (1 << PINB4);
	//OW_PORT_WAIT_US(100);
	//PORTB &= ~(1 << PINB4);

	/* Wait for T One max. and check the 
	 * IO state. If the bus has been pulled
	 * up again the received bit is a 1. */
	PORTB |= (1 << PINB2);
	OW_PORT_WAIT_US(OW_TO_MAX);
	bit = slave_hal->hal_io->read();
	PORTB &= ~(1 << PINB2);
	if(bit) {
		bit = slave_cbs->receive_transmit_bit(1);
		//PORTB |= (1 << PINB4);
		//OW_PORT_WAIT_US(250);
		PORTB &= ~(1 << PINB4);
		//ow_ll_slave_write_bit(1);
		/* Upon receiving a 1 the slave may transmit
		 * a bit in response. */
		ow_ll_slave_write_bit(bit);
	
		goto exit_int;
	}
	
	/* Wait for Delta-T One-Zero max. and check the 
	 * IO state. If the bus has been pulled
	 * up again the received bit is a 0. */	
	PORTB |= (1 << PINB2);
	OW_PORT_WAIT_US(OW_DTOZ_MAX);
	bit = slave_hal->hal_io->read();
	PORTB &= ~(1 << PINB2);
	if(bit) {
		slave_cbs->receive_transmit_bit(0);
		//PORTB |= (1 << PINB4);
		//OW_PORT_WAIT_US(100);
		PORTB &= ~(1 << PINB4);
		goto exit_int;
	}
	
	/* Wait for Delta-T Zero-Reset max. and check the 
	 * IO state. If the bus has been pulled
	 * up again the received pulse is a reset. */
	PORTB |= (1 << PINB2);
	OW_PORT_WAIT_US(OW_DTZR_MAX);
	bit = slave_hal->hal_io->read();
	PORTB &= ~(1 << PINB2);
	if(bit) {
		//PORTB |= (1 << PINB4);
		//OW_PORT_WAIT_US(100);
		PORTB &= ~(1 << PINB4);
		ow_ll_slave_write_bit(0); /* Presence pulse. */
		slave_cbs->reset();
		
		goto exit_int;
	}


exit_int:
	slave_hal->hal_int->int_enable();
}

static void ow_ll_slave_write_bit(uint8_t bit)
{
	/* Only pull the line low if the slave
	 * wants to transmit a 0. */
	if(bit == 0) {
		slave_hal->hal_io->output();
		slave_hal->hal_io->write(0);
		OW_PORT_WAIT_US(OW_TZ_MIN - OW_TS);
		slave_hal->hal_io->input();
	}
}