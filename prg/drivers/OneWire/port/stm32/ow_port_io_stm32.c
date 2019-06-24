/*
 * ow_port_io_stm32.c
 *
 *  Created on: Jun 15, 2019
 *      Author: dorus
 */

#include "ow_port_io_stm32.h"

#include "ow_hal_io.h"
#include "gpio.h"

static void ow_port_io_set_input(void);
static void ow_port_io_set_output(void);
static void ow_port_io_write(uint8_t bit);
static uint8_t ow_port_io_read(void);

static const ow_hal_io_t ow_port_io_stm32 = {
	.input = ow_port_io_set_input,
	.output = ow_port_io_set_output,
	.write = ow_port_io_write,
	.read = ow_port_io_read,
};

ow_hal_io_t *ow_port_io_stm32_get(void)
{
	return (ow_hal_io_t *)&ow_port_io_stm32;
}

static void ow_port_io_set_input(void)
{
	GpioOneWireTxStateSet(0);
}

static void ow_port_io_set_output(void)
{

}

static void ow_port_io_write(uint8_t bit)
{
	if(bit) {
		GpioOneWireTxStateSet(0);
	} else {
		GpioOneWireTxStateSet(1);
	}
}
static uint8_t ow_port_io_read(void)
{
	return GpioOneWireRxStateGet();
}
