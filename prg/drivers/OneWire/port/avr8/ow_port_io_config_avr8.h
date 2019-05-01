/*
 * ow_port_io_config_avr8.h
 *
 *  Created on: Apr 28, 2019
 *      Author: dorus
 */

#ifndef ONEWIRE_PORT_IO_CONFIG_AVR8_H_
#define ONEWIRE_PORT_IO_CONFIG_AVR8_H_

#include <avr/io.h>

#define ONEWIRE_PORT_CONFIG_USE_SINGLE_PIN

#ifdef ONEWIRE_PORT_CONFIG_USE_SINGLE_PIN

#define ONEWIRE_PORT_IO_CONFIG_DDR_INPUT	DDRB
#define ONEWIRE_PORT_IO_CONFIG_PORT_INPUT	PORTB
#define ONEWIRE_PORT_IO_CONFIG_PIN_INPUT	PINB0

#define ONEWIRE_PORT_IO_CONFIG_DDR_OUTPUT	DDRB
#define ONEWIRE_PORT_IO_CONFIG_PORT_OUTPUT	PORTB
#define ONEWIRE_PORT_IO_CONFIG_PIN_OUTPUT	PINB0

#else

#define ONEWIRE_PORT_IO_CONFIG_DDR_INPUT	DDRB
#define ONEWIRE_PORT_IO_CONFIG_PORT_INPUT	PORTB
#define ONEWIRE_PORT_IO_CONFIG_PIN_INPUT	PINB0

#define ONEWIRE_PORT_IO_CONFIG_DDR_OUTPUT	ONEWIRE_PORT_IO_CONFIG_DDR_INPUT
#define ONEWIRE_PORT_IO_CONFIG_PORT_OUTPUT	ONEWIRE_PORT_IO_CONFIG_PORT_INPUT
#define ONEWIRE_PORT_IO_CONFIG_PIN_OUTPUT	ONEWIRE_PORT_IO_CONFIG_PIN_INPUT

#endif

#endif /* ONEWIRE_PORT_IO_CONFIG_AVR8_H_ */
