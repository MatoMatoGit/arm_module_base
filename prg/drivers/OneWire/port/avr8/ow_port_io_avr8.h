/*
 * ow_port_io_avr8.h
 *
 *  Created on: Apr 28, 2019
 *      Author: dorus
 */

#ifndef ONEWIRE_PORT_IO_AVR8_H_
#define ONEWIRE_PORT_IO_AVR8_H_

#include "ow_hal_io.h"

#include <stdint.h>

void ow_port_io_set_input(void);
void ow_port_io_set_output(void);
void ow_port_io_write(uint8_t bit);
uint8_t ow_port_io_read(void);
void ow_port_wait_us(uint32_t us);

const ow_hal_io_t ow_port_io_avr8 = {
	.input = ow_port_io_set_input,
	.output = ow_port_io_set_output,
	.write = ow_port_io_write,
	.read = ow_port_io_read,
};

#endif /* ONEWIRE_PORT_IO_AVR8_H_ */

