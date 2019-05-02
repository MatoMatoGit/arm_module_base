/*
 * ow_port_tmr_avr8.c
 *
 * Created: 02-05-2019 16:55:27
 *  Author: dorspi
 */ 

#include "ow_port_tmr_avr8.h"

#include "oc_timer.h"

#include <stdint.h>

#define TIMER_CALLBACK_INTERVAL_US 2500

#define TIMER_TICKS_TO_US(us) (us / 1000)

static void ow_port_tmr_init(void);
static uint32_t ow_port_tmr_get_us(void);
static void ow_port_tmr_callback(void);

static const ow_hal_tmr_t ow_port_tmr_avr8  = {
	.tmr_init = ow_port_tmr_init,
	.tmr_get_us = ow_port_tmr_get_us,
};

static uint32_t total_us = 0;

ow_hal_tmr_t *ow_port_tmr_avr8_get(void)
{
	return (ow_hal_tmr_t *)&ow_port_tmr_avr8;
}

static void ow_port_tmr_init(void)
{
	OcTimerCallbackRegister(ow_port_tmr_callback);
}

static uint32_t ow_port_tmr_get_us(void)
{
	return total_us + TIMER_TICKS_TO_US(OcTimerGet());
}

void ow_port_tmr_callback(void)
{
	total_us += TIMER_CALLBACK_INTERVAL_US;
}