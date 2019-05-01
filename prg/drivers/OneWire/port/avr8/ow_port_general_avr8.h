/*
 * ow_port_general_avr8.h
 *
 *  Created on: May 1, 2019
 *      Author: dorus
 */

#ifndef ONEWIRE_PORT_GENERAL_AVR8_H_
#define ONEWIRE_PORT_GENERAL_AVR8_H_

#include "ow_hal_general.h"

const ow_hal_general_t ow_port_general_avr8 = {
	.wait_us = ow_port_general_wait_us,
};


#endif /* ONEWIRE_PORT_GENERAL_AVR8_H_ */
