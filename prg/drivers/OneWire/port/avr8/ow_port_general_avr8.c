/*
 * ow_port_general_avr8.c
 *
 *  Created on: May 1, 2019
 *      Author: dorus
 */

#include "ow_port_general_avr8.h"

#include <util/delay.h>

static void ow_port_general_wait_us(uint32_t us);

const ow_hal_general_t ow_port_general_avr8 = {
	.wait_us = ow_port_general_wait_us,
};

ow_hal_general_t *ow_port_general_avr8_get(void)
{
	return (ow_hal_general_t *)&ow_port_general_avr8;
}

static void ow_port_general_wait_us(uint32_t us)
{
	uint32_t t = us / 50;
	while(t > 0) {
		_delay_us(50);
		t--;
	}
	
}
