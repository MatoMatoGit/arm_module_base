/*
 * mmb485.c
 *
 *  Created on: 3 apr. 2018
 *      Author: Dorus
 */
#include "mmb485.h"
#include "mmb485_hal.h"

#include <PriorRTOS.h> /* Using PriorRTOS soft-timers. */

#define ARBITRATION_DELAY_INITIAL		10
#define ARBITRATION_DELAY_PRIO_LEVEL	60

static int IBusStateTrySet(MmbDescriptor_t *desc, BusState_t state);
static BusState_t IStateGet(MmbDescriptor_t *desc);
static void ITimeoutStart(MmbDescriptor_t *desc, uint32_t timeout_ms);
static void IArbitrationStart(MmbDescriptor_t *desc, uint32_t timeout_ms);

void IArbitrationTimerHandler(Id_t timer, void *context);
void ITimeoutTimerHandler(Id_t timer, void *context);

int Mmb485Init(MmbDescriptor_t *desc)
{
	int res = MMB485_RESULT_ERR;

	if(desc != NULL) {
		desc->state = BUS_STATE_UNINIT;

		if(desc->on_arbitration_result != NULL && desc->on_data_received != NULL) {
			desc->timeout_ms = 0;
			desc->always_listen = false;

			/* Initialize hardware and disable the receiver and transmitter.
			 * This sets the device in high impedance mode. */
			desc->hal.init(desc->baud_rate);
			desc->hal.receiver_en(0);
			desc->hal.transmitter_en(0);
			desc->state = BUS_STATE_HIGH_Z;

			/* Create timers. */
			desc->timeout_tmr = TimerCreate(1, (TIMER_PARAMETER_PERIODIC | TIMER_PARAMETER_AR), ITimeoutTimerHandler, (void *)desc);
			desc->arb_tmr = TimerCreate(1, (TIMER_PARAMETER_PERIODIC | TIMER_PARAMETER_AR), IArbitrationTimerHandler, (void *)desc);

			if(desc->timeout_tmr != OS_ID_INVALID && desc->arb_tmr != OS_ID_INVALID) {
				res = MMB485_RESULT_OK;
			}

		}
	}

	return res;
}


int Mmb485ArbitrationStart(MmbDescriptor_t *desc, MsgPrio_t prio, uint32_t timeout_ms)
{
	int res = MMB485_RESULT_ERR;

	if(desc != NULL && timeout_ms != 0) {
		res = IBusStateTrySet(desc, BUS_STATE_ARBITRATION);

		if(res == MMB485_RESULT_OK) {
			ITimeoutStart(desc, timeout_ms);
			IArbitrationStart(desc, ARBITRATION_DELAY_INITIAL + prio * ARBITRATION_DELAY_PRIO_LEVEL);
		}
	}

	return res;
}

int Mmb485Transmit(MmbDescriptor_t *desc, uint8_t *data, uint32_t size)
{
	int res = MMB485_RESULT_ERR;

	if(desc != NULL && data != NULL && size != 0) {
		res = IBusStateTrySet(desc, BUS_STATE_TRANSMIT);

		if(res == MMB485_RESULT_OK) {
			desc->hal.transmit(data, size);
		}
	}

	return res;
}

int Mmb485Listen(MmbDescriptor_t *desc, uint32_t timeout_ms)
{
	int res = MMB485_RESULT_ERR;

	if(desc != NULL && timeout_ms != 0) {
		res = IBusStateTrySet(desc, BUS_STATE_LISTEN);

		if(res == MMB485_RESULT_OK) {
			desc->bytes_left = 0;
			desc->hal.transmitter_en(0);
			desc->hal.receiver_en(1);
		}
	}

	return res;
}

int Mmb485ListenNBytes(MmbDescriptor_t *desc, uint32_t n_bytes, uint32_t timeout_ms)
{
	int res = MMB485_RESULT_ERR;

	if(desc != NULL && n_bytes != 0 &&timeout_ms != 0) {
		res = IBusStateTrySet(desc, BUS_STATE_LISTEN);

		if(res == MMB485_RESULT_OK) {
			desc->bytes_left = n_bytes;
			desc->hal.transmitter_en(0);
			desc->hal.receiver_en(1);
		}
	}

	return res;
}

/* val = 1 to enable, 0 to disable. */
int Mmb485AlwaysListenEnable(MmbDescriptor_t *desc, uint8_t val)
{
	int res = MMB485_RESULT_ERR;

	if(desc != NULL) {
		if(val) {
			if(desc->always_listen) {
				res = MMB485_RESULT_FAIL;
			} else {
				desc->always_listen = true;
				res = MMB485_RESULT_OK;
			}
		} else {
			if(!desc->always_listen) {
				res = MMB485_RESULT_FAIL;
			} else {
				desc->always_listen = false;
				res = MMB485_RESULT_OK;
			}
		}

	}

	return res;
}



void Mmb485HalCallbackDataReceived(MmbDescriptor_t *desc, uint8_t *data, uint32_t size)
{
	if(desc != NULL && data != NULL && size != 0) {
		if(IStateGet(desc) == BUS_STATE_LISTEN) {
			if(desc->bytes_left >= size) {
				desc->bytes_left -= size;
			} else {
				desc->bytes_left = 0;
			}
			desc->on_data_received(desc, data, size);
		}
	}
}

static void IArbitrationStart(MmbDescriptor_t *desc, uint32_t timeout_ms)
{
	TimerIntervalSet(desc->arb_tmr, timeout_ms);
	TimerStart(desc->arb_tmr);
}


void IArbitrationTimerHandler(Id_t timer, void *context)
{
}




static void ITimeoutStart(MmbDescriptor_t *desc, uint32_t timeout_ms)
{
	TimerIntervalSet(desc->timeout_tmr, timeout_ms);
	TimerStart(desc->timeout_tmr);
}

void ITimeoutTimerHandler(Id_t timer, void *context)
{

}

static BusState_t IStateGet(MmbDescriptor_t *desc)
{
	BusState_t state = BUS_STATE_UNINIT;

	if(desc != NULL) {
		state = desc->state;
	}

	return state;
}



static int IBusStateTrySet(MmbDescriptor_t *desc, BusState_t state)
{
	int res = MMB485_RESULT_FAIL;

	if(desc != NULL) {

		switch(desc->state) {

		/* Uninitialized state.
		 * Allowed to go to the High Z state from here.  */
		case BUS_STATE_UNINIT: {
			if(state == BUS_STATE_HIGH_Z) {
				desc->state = state;
				res = MMB485_RESULT_OK;
			}
			break;
		}

		/* High Impedance state. Transmitter and Receiver are both disabled.
		 * Allowed to go the High Z, Listen or Arbitration state. */
		case BUS_STATE_HIGH_Z: {
			if(state == BUS_STATE_HIGH_Z || state == BUS_STATE_LISTEN || state == BUS_STATE_ARBITRATION) {
				desc->state = state;
				res = MMB485_RESULT_OK;
			}
			break;
		}

		/* Arbitration state.
		 * Allowed to go to the Bus Claimed state. */
		case BUS_STATE_ARBITRATION: {
			break;
		}


		case BUS_STATE_BUS_CLAIMED: {
			break;
		}

		/* Listen state. Receiver is enabled.
		 * Allowed to go to the Listen, High Z or Arbitration state. */
		case BUS_STATE_LISTEN: {
			if(state == BUS_STATE_HIGH_Z || state == BUS_STATE_LISTEN || state == BUS_STATE_ARBITRATION) {
				desc->state = state;
				res = MMB485_RESULT_OK;
			}
			break;
		}

		/* Transmit state. Transmitter is enabled.
		 * All states except Uninit are allowed.. */
		case BUS_STATE_TRANSMIT: {
			if(state != BUS_STATE_UNINIT) {
				desc->state = state;
				res = MMB485_RESULT_OK;
			}
			break;
		}


		}

	}

	return res;
}

