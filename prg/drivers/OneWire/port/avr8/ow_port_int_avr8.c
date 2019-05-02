/*
 * ow_port_int_avr8.c
 *
 *  Created on: May 2, 2019
 *      Author: dorus
 */


#include "ow_port_int_avr8.h"
#include "ow_port_io_avr8.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "ow_ll_slave.h"

#define OW_PORT_INT_PCINT	PCINT0
#define OW_PORT_INT_VECTOR 	PCINT0_vect

#define EDGE_FALLING 0
#define EDGE_RISING 1

static volatile uint8_t edge = EDGE_FALLING;
static ow_hal_io_t *hal_io = NULL;

void ow_port_int_init(void);
void ow_port_int_enable(void);
void ow_port_int_disable(void);
void ow_port_int_set_rising(void);
void ow_port_int_set_falling(void);

static const ow_hal_int_t ow_port_int_avr8 = {
		.int_init = ow_port_int_init,
		.int_enable = ow_port_int_enable,
		.int_disable = ow_port_int_disable,
		.int_set_rising = ow_port_int_set_rising,
		.int_set_falling = ow_port_int_set_falling
};

ow_hal_int_t *ow_port_int_avr8_get(void)
{
	return (ow_hal_int_t *)&ow_port_int_avr8;
}

void ow_port_int_init(void)
{
	hal_io = ow_port_io_avr8_get();
}

void ow_port_int_enable(void)
{
	cli();
	PCMSK |= (1 << OW_PORT_INT_PCINT);
	sei();
}

void ow_port_int_disable(void)
{
	cli();
	PCMSK &= ~(1 << OW_PORT_INT_PCINT);
	sei();
}


void ow_port_int_set_rising(void)
{
	edge = EDGE_RISING;
}

void ow_port_int_set_falling(void)
{
	edge = EDGE_FALLING;
}

ISR(OW_PORT_INT_VECTOR)
{
	static uint8_t prev_state = 0;
	uint8_t curr_state;

	curr_state = hal_io->read();

	if(prev_state != curr_state) {
		if(prev_state == 0 && edge == EDGE_RISING) {
			ow_ll_slave_int_handler();
		} else if(prev_state == 1 && edge == EDGE_FALLING) {
			ow_ll_slave_int_handler();
		}
	}
}
