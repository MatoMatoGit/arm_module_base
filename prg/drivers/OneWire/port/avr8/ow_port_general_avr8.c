/*
 * ow_port_general_avr8.c
 *
 *  Created on: May 1, 2019
 *      Author: dorus
 */

#include "ow_port_general_avr8.h"

#include <util/delay.h>

void ow_port_general_wait_us(uint32_t us)
{
	_delay_us(us);
}
