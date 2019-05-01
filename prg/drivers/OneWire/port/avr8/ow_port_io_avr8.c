/*
 * ow_port_avr8.c
 *
 *  Created on: Apr 28, 2019
 *      Author: dorus
 */


#include "ow_port_io_avr8.h"
#include "ow_port_io_config_avr8.h"

void ow_port_io_set_input(void)
{
	/* Set the input pin and the output pin as inputs. The output pin no longer
	 * actively drives the bus low, the ext. pull up resistor will pull the bus
	 * high. */
	ONEWIRE_PORT_IO_CONFIG_DDR_INPUT &= ~(1 << ONEWIRE_PORT_IO_CONFIG_PIN_INPUT);
	ONEWIRE_PORT_IO_CONFIG_DDR_OUTPUT &= ~(1 << ONEWIRE_PORT_IO_CONFIG_PIN_OUTPUT);
}

void ow_port_io_set_output(void)
{
	ONEWIRE_PORT_IO_CONFIG_DDR_OUTPUT |= (1 << ONEWIRE_PORT_IO_CONFIG_PIN_OUTPUT);
}

void ow_port_io_write(uint8_t bit)
{
	if(bit) {
		ONEWIRE_PORT_IO_CONFIG_PORT_OUTPUT |= (1 << ONEWIRE_PORT_IO_CONFIG_PIN_OUTPUT);
	} else {
		ONEWIRE_PORT_IO_CONFIG_PORT_OUTPUT &= ~(1 << ONEWIRE_PORT_IO_CONFIG_PIN_OUTPUT);
	}
}
uint8_t ow_port_io_read(void)
{
	return (ONEWIRE_PORT_IO_CONFIG_PORT_INPUT & ONEWIRE_PORT_IO_CONFIG_PIN_INPUT);
}

