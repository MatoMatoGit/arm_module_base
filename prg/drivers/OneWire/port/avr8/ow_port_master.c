/*
 * ow_port_slave.c
 *
 * Created: 02-05-2019 17:07:43
 *  Author: dorspi
 */ 

#include "ow_port_master.h"
#include "ow_port_io_avr8.h"
#include "ow_port_general_avr8.h"

static ow_hal_master_t ow_port_master_avr8;

ow_hal_master_t *ow_port_master_avr8_get(void)
{
	ow_port_master_avr8.hal_io = ow_port_io_avr8_get();
	ow_port_master_avr8.hal_gen = ow_port_general_avr8_get();
	
	return &ow_port_master_avr8;
}