/*
 * ow_port_master.c
 *
 *  Created on: Jun 15, 2019
 *      Author: dorus
 */

#include "ow_port_master.h"

#include "ow_port_io_stm32.h"

static ow_hal_master_t ow_port_master_stm32;

ow_hal_master_t *ow_port_master_stm32_get(void)
{
	ow_port_master_stm32.hal_io = ow_port_io_stm32_get();

	return &ow_port_master_stm32;
}
