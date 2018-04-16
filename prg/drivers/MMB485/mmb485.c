/*
 * mmb485.c
 *
 *  Created on: 3 apr. 2018
 *      Author: Dorus
 */
#include "mmb485.h"
#include "mmb485_hal.h"

#include <PriorRTOS.h> /* Using PriorRTOS soft-timers, logging and ring-buffers. */

#include <math.h>

LOG_FILE_NAME("mmb485.c")

#define BUS_STATE_MAX_STRING_LENGTH 11

#define ARBITRATION_DELAY_BASE_US		300e3
#define ARBITRATION_DELAY_PRIO_LEVEL_US	50e3

#define DATA_RECEIVED_FLAG 0x01

/* |MSG_PRIO_ID|MSG_PRIO| MSG_PRIO transmitted as uint8_t (4 bytes). */
const uint8_t MsgPrioId[] = "MMB485P"; /* Message Prio identifier. */
#define MSG_PRIO_ID_SIZE sizeof(MsgPrioId)
#define MSG_PRIO_SIZE 1

#define MSG_PRIO_ID_OFFSET 0
#define MSG_PRIO_OFFSET MSG_PRIO_ID_SIZE

typedef enum {
	BUS_MODE_HIGH_Z,
	BUS_MODE_TRANSMIT,
	BUS_MODE_RECEIVE,
}BusMode_t;

static int IArbitrationStart(MmbDescriptor_t *desc, MsgPrio_t prio, uint32_t timeout_us);
static int ITransmitData(MmbDescriptor_t *desc);
static int IBusStateTrySet(MmbDescriptor_t *desc, BusState_t state);
static BusState_t IStateGet(MmbDescriptor_t *desc);
static void ITimeoutStart(MmbDescriptor_t *desc, uint32_t timeout_us);
static void IArbitrationTimerStart(MmbDescriptor_t *desc, uint32_t timeout_us);
static void IBusModeSet(MmbDescriptor_t *desc, BusMode_t mode);
static char *IBusStateToString(BusState_t state);

void IArbitrationTimerHandler(Id_t timer, void *context);
void ITimeoutTimerHandler(Id_t timer, void *context);

int Mmb485Init(MmbDescriptor_t *desc)
{
	int res = MMB485_RESULT_ERR;

	if(desc != NULL) {
		desc->state = BUS_STATE_UNINIT;

		if(desc->on_transmit_result != NULL && desc->on_data_received != NULL) {
			/* Initialize hardware and disable the receiver and transmitter.
			 * This sets the device in high impedance mode. */
			if(desc->hal.init != NULL) {
				desc->hal.init(desc->baud_rate);
			}
			IBusModeSet(desc, BUS_MODE_HIGH_Z);
			desc->state = BUS_STATE_HIGH_Z;

			desc->prio = 0;
			desc->tx_data = NULL;
			desc->bytes_left = 0;
			desc->timeout_us = 0;
			desc->always_listen = false;
			desc->tx_max_bytes = (uint32_t)floor(floor(desc->baud_rate / 1e3) * ARBITRATION_DELAY_BASE_US * 0.8f);

			/* Create timers and ring-buffer. */
			desc->timeout_tmr = TimerCreate(1, (TIMER_PARAMETER_PERIODIC), ITimeoutTimerHandler, (void *)desc);
			desc->arb_tmr = TimerCreate(1, (TIMER_PARAMETER_PERIODIC), IArbitrationTimerHandler, (void *)desc);
			desc->rx_ringbuf = RingbufCreate(NULL, RX_BUFFER_SIZE_BYTES);

			if(desc->timeout_tmr != OS_ID_INVALID && desc->arb_tmr != OS_ID_INVALID && desc->rx_ringbuf != OS_ID_INVALID) {
				res = MMB485_RESULT_OK;
			}

		}
	}

	return res;
}




int Mmb485Transmit(MmbDescriptor_t *desc, MsgPrio_t prio, uint8_t *data, uint32_t size, uint32_t timeout_ms)
{
	int res = MMB485_RESULT_ERR;

	if(desc != NULL && data != NULL && size != 0) {
		if(size <= desc->tx_max_bytes) {
			desc->bytes_left = size;
			desc->tx_data = data;
			IArbitrationStart(desc, prio, timeout_ms * 1000);
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
			RingbufFlush(desc->rx_ringbuf);
			IBusModeSet(desc, BUS_MODE_RECEIVE);
			ITimeoutStart(desc, timeout_ms * 1000);
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
			RingbufFlush(desc->rx_ringbuf);
			IBusModeSet(desc, BUS_MODE_RECEIVE);
			ITimeoutStart(desc, timeout_ms * 1000);
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
		BusState_t state = IStateGet(desc);

		/* In the Listen state all data is delegated to the user. */
		if(state == BUS_STATE_LISTEN) {

			if(desc->bytes_left) { /* If listening for a N bytes. */
				if(desc->bytes_left >= size) {
					desc->bytes_left -= size;
					TimerReset(desc->timeout_tmr); /* Reset receive timeout since data was received. */
				} else {
					desc->bytes_left = 0;
				}
			}
			desc->on_data_received(desc, data, size);
		} else if(state == BUS_STATE_WAIT_IDLE || state == BUS_STATE_TRANSMIT) {
			/* In the Wait Idle state no data is saved, instead a flag is set in the buffer.  */
			RingbufBase_t flag = DATA_RECEIVED_FLAG;
			uint32_t len = sizeof(flag);
			RingbufFlush(desc->rx_ringbuf);
			RingbufWrite(desc->rx_ringbuf, &flag, &len, OS_TIMEOUT_NONE);
		} else if(state == BUS_STATE_ARBITRATION) {
			/* In the Arbitration state all data is saved. */
			uint32_t len = size;
			RingbufWrite(desc->rx_ringbuf, (RingbufBase_t *)data, &len, OS_TIMEOUT_NONE);
		}
	}
}


static int IArbitrationStart(MmbDescriptor_t *desc, MsgPrio_t prio, uint32_t timeout_us)
{
	int res = MMB485_RESULT_ERR;

	if(desc != NULL && timeout_us != 0) {
		res = IBusStateTrySet(desc, BUS_STATE_WAIT_IDLE);

		if(res == MMB485_RESULT_OK) {
			uint32_t arb_time = ARBITRATION_DELAY_BASE_US + prio * ARBITRATION_DELAY_PRIO_LEVEL_US;
			desc->timeout_us = timeout_us;
			desc->prio = (uint8_t)prio;
			ITimeoutStart(desc, timeout_us);
			IArbitrationTimerStart(desc, arb_time);
			LOG_DEBUG_NEWLINE("[%p] : Arbitration started with timeout %u.", desc, timeout_us);
			LOG_DEBUG_NEWLINE("[%p] : Waiting %u before bus idle check.", desc, arb_time);
		}
	}

	return res;
}

static void IArbitrationTimerStart(MmbDescriptor_t *desc, uint32_t timeout_us)
{
	TimerStop(desc->arb_tmr);
	TimerIntervalSet(desc->arb_tmr, timeout_us);
	TimerStart(desc->arb_tmr);
}


void IArbitrationTimerHandler(Id_t timer, void *context)
{
	MmbDescriptor_t *desc = (MmbDescriptor_t *)context;

	RingbufBase_t buf[MSG_PRIO_ID_SIZE + MSG_PRIO_SIZE];
	uint32_t len = 1;
	uint8_t arb_complete = 1;

	switch(desc->state) {
	case BUS_STATE_WAIT_IDLE: {
		desc->h_prio = 1;

		RingbufRead(desc->rx_ringbuf, buf, &len, OS_TIMEOUT_NONE);
		/* If anything was received,
		 * restart the wait timer. Otherwise
		 * transmit the message prio and go to the
		 * arbitration state. */
		if(len) {
			LOG_DEBUG_NEWLINE("[%p] : Bus is not idle. Waiting..", desc);
			RingbufFlush(desc->rx_ringbuf);
			IArbitrationTimerStart(desc, ARBITRATION_DELAY_BASE_US + desc->prio * ARBITRATION_DELAY_PRIO_LEVEL_US); /* Restart arbitration timer. */
		} else {
			LOG_DEBUG_NEWLINE("[%p] : Bus is idle. Transmitting msg prio %u", desc, (uint)desc->prio);
			/* Transmit message prio and immediately back to receive mode. */
			IBusModeSet(desc, BUS_MODE_TRANSMIT);
			desc->hal.transmit((uint8_t *)MsgPrioId, MSG_PRIO_ID_SIZE);
			desc->hal.transmit((uint8_t *)&desc->prio, MSG_PRIO_SIZE);
			IBusModeSet(desc, BUS_MODE_RECEIVE);
			IArbitrationTimerStart(desc, ARBITRATION_DELAY_BASE_US);
			IBusStateTrySet(desc, BUS_STATE_ARBITRATION);
		}
		break;
	}

	case BUS_STATE_ARBITRATION: {
		len = MSG_PRIO_ID_SIZE + MSG_PRIO_SIZE;


		do {
			RingbufRead(desc->rx_ringbuf, buf, &len, OS_TIMEOUT_NONE);

			/* If data was received, check if the Message ID is present and compare
			 * Message priorities.
			 * This process is repeated until no data is received on the bus. */
			if(len == MSG_PRIO_ID_SIZE + MSG_PRIO_SIZE) {
				arb_complete = 0;
				IArbitrationTimerStart(desc, ARBITRATION_DELAY_BASE_US);

				if(memcmp((void *)&buf[MSG_PRIO_ID_OFFSET], MsgPrioId, MSG_PRIO_ID_SIZE) == 0) { /* Compare Identifier. */

					LOG_DEBUG_NEWLINE("[%p] : Received msg prio %u", desc, (uint)buf[MSG_PRIO_OFFSET]);
					if(buf[MSG_PRIO_OFFSET] < desc->prio) { /* Lost arbitration. */
						desc->h_prio = 0;
						arb_complete = 1;
						break;
					} else if((buf[MSG_PRIO_OFFSET] == desc->prio)) {
						/* Set random timer, try again. */
					}

				} else {
					break;
				}

			} else if(len == 0) {
				arb_complete = 1;
				break;
			} else {
				IArbitrationTimerStart(desc, ARBITRATION_DELAY_BASE_US);
				break;
			}
		} while(1);

		/* If the message has the highest prio the bus has been claimed for transmission.
		 * Transmit the data and return to the High Z state.
		 * Otherwise restart the arbitration timer with its initial delay and go to the
		 * Wait Idle state. */
		if(desc->h_prio == 1 && arb_complete) {
			LOG_DEBUG_NEWLINE("[%p] : Msg has highest prio.", desc);
			TimerStop(desc->timeout_tmr);
			ITransmitData(desc);
			/* TODO: Go to listen mode if always listen is true. */
			/* If the state has not changed during transmission, change to non-transmitting state. */
			if(desc->state == BUS_STATE_TRANSMIT) {
				IBusStateTrySet(desc ,BUS_STATE_HIGH_Z);
				IBusModeSet(desc, BUS_MODE_HIGH_Z);
			}
		} else if(desc->h_prio == 0 && arb_complete) {
			LOG_DEBUG_NEWLINE("[%p] : Msg did not have highest prio.", desc);
			IBusModeSet(desc, BUS_MODE_HIGH_Z); /* TODO: Listen if always listen is enabled. */
			IBusStateTrySet(desc, BUS_STATE_WAIT_IDLE);
			IArbitrationTimerStart(desc, ARBITRATION_DELAY_BASE_US + desc->prio * ARBITRATION_DELAY_PRIO_LEVEL_US); /* Restart arbitration timer. */
		}
		break;
	}

	default: {
		break;
	}

	}
}


static int ITransmitData(MmbDescriptor_t *desc)
{
	int res = MMB485_RESULT_ERR;

	res = IBusStateTrySet(desc, BUS_STATE_TRANSMIT);

	if(res == MMB485_RESULT_OK) {
		IBusModeSet(desc, BUS_MODE_TRANSMIT);
		/* Only transmit if the rx buffer is empty. */
		if(RingbufDataCountGet(desc->rx_ringbuf) == 0) {
			desc->hal.transmit(desc->tx_data, desc->bytes_left);
		}
		desc->on_transmit_result(desc, MMB485_RESULT_OK);
	}

	return res;
}


static void ITimeoutStart(MmbDescriptor_t *desc, uint32_t timeout_us)
{
	TimerStop(desc->timeout_tmr);
	TimerIntervalSet(desc->timeout_tmr, timeout_us);
	TimerStart(desc->timeout_tmr);
}

void ITimeoutTimerHandler(Id_t timer, void *context)
{
	MmbDescriptor_t *desc = (MmbDescriptor_t *)context;

	LOG_DEBUG_NEWLINE("[%p] : Timeout expired.", desc);

	switch(desc->state) {

	case BUS_STATE_TRANSMIT: {
		IBusStateTrySet(desc, BUS_STATE_HIGH_Z);
		IBusModeSet(desc, BUS_MODE_HIGH_Z); /* TODO: Listen if always listen is enabled. */
		break;
	}

	case BUS_STATE_LISTEN: {
		IBusStateTrySet(desc, BUS_STATE_HIGH_Z);
		IBusModeSet(desc, BUS_MODE_HIGH_Z); /* TODO: Listen if always listen is enabled. */
		break;
	}

	case BUS_STATE_ARBITRATION:
	case BUS_STATE_WAIT_IDLE: {
		IBusStateTrySet(desc, BUS_STATE_HIGH_Z);
		IBusModeSet(desc, BUS_MODE_HIGH_Z); /* TODO: Listen if always listen is enabled. */
		desc->on_transmit_result(desc, MMB485_RESULT_FAIL);
		break;
	}

	default: {
		break;
	}

	}
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

	BusState_t prev_state = BUS_STATE_HIGH_Z;

	if(desc != NULL) {
		prev_state = desc->state;

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
		 * Allowed to go the High Z, Listen or Wait Idle state. */
		case BUS_STATE_HIGH_Z: {
			if(state == BUS_STATE_HIGH_Z || state == BUS_STATE_LISTEN || state == BUS_STATE_WAIT_IDLE) {
				desc->state = state;
				res = MMB485_RESULT_OK;
			}
			break;
		}

		/* Wait Idle state. Wait until the bus is idle i.e. no received messages.
		 * Allowed to go the Wait Idle, High Z, Arbitration or Listen state. */
		case BUS_STATE_WAIT_IDLE: {
			if(state == BUS_STATE_HIGH_Z || state == BUS_STATE_LISTEN || state == BUS_STATE_WAIT_IDLE || state == BUS_STATE_ARBITRATION) {
				desc->state = state;
				res = MMB485_RESULT_OK;
			}
			break;
		}

		/* Arbitration state.
		 * Allowed to go to the High Z, Transmit, Listen or Wait Idle state. */
		case BUS_STATE_ARBITRATION: {
			if(state == BUS_STATE_HIGH_Z || state == BUS_STATE_TRANSMIT || state == BUS_STATE_WAIT_IDLE || state == BUS_STATE_LISTEN) {
				desc->state = state;
				res = MMB485_RESULT_OK;
			}
			break;
		}

		/* Listen state. Receiver is enabled.
		 * Allowed to go to the Listen, High Z or Wait Idle state. */
		case BUS_STATE_LISTEN: {
			if(state == BUS_STATE_HIGH_Z || state == BUS_STATE_LISTEN || state == BUS_STATE_WAIT_IDLE) {
				desc->state = state;
				res = MMB485_RESULT_OK;
			}
			break;
		}

		/* Transmit state. Transmitter is enabled.
		 * All states except Uninit or Arbitration state are allowed.. */
		case BUS_STATE_TRANSMIT: {
			if(state != BUS_STATE_UNINIT && state != BUS_STATE_ARBITRATION) {
				desc->state = state;
				res = MMB485_RESULT_OK;
			}
			break;
		}

		default: {
			break;
		}

		}

	}

	LOG_DEBUG_NEWLINE("[%p] : State %s -> ", desc, IBusStateToString(prev_state));
	LOG_DEBUG_APPEND("%s", IBusStateToString(desc->state));

	return res;
}

static void IBusModeSet(MmbDescriptor_t *desc, BusMode_t mode)
{
	switch(mode) {

	default:
	case BUS_MODE_HIGH_Z: {
		desc->hal.transmitter_en(0);
		desc->hal.receiver_en(0);
		break;
	}

	case BUS_MODE_TRANSMIT: {
		desc->hal.receiver_en(0);
		desc->hal.transmitter_en(1);
		break;
	}

	case BUS_MODE_RECEIVE: {
		desc->hal.transmitter_en(0);
		desc->hal.receiver_en(1);
		break;
	}

	}
}


static char state_str[BUS_STATE_MAX_STRING_LENGTH];

static char *IBusStateToString(BusState_t state)
{

#define BUS_STATE_UNINIT_STRING "uninit"
#define BUS_STATE_HIGH_Z_STRING "high_z"
#define BUS_STATE_WAIT_IDLE_STRING "wait_idle"
#define BUS_STATE_ARBITRATION_STRING "arbitration"
#define BUS_STATE_TRANSMIT_STRING "transmit"
#define BUS_STATE_LISTEN_STRING "listen"

	switch(state) {
		case BUS_STATE_UNINIT: {
			strncpy(state_str, BUS_STATE_UNINIT_STRING, BUS_STATE_MAX_STRING_LENGTH);
			break;
		}

		case BUS_STATE_HIGH_Z: {
			strncpy(state_str, BUS_STATE_HIGH_Z_STRING, BUS_STATE_MAX_STRING_LENGTH);
			break;
		}

		case BUS_STATE_WAIT_IDLE: {
			strncpy(state_str, BUS_STATE_WAIT_IDLE_STRING, BUS_STATE_MAX_STRING_LENGTH);
			break;
		}

		case BUS_STATE_ARBITRATION: {
			strncpy(state_str, BUS_STATE_ARBITRATION_STRING, BUS_STATE_MAX_STRING_LENGTH);
			break;
		}

		case BUS_STATE_LISTEN: {
			strncpy(state_str, BUS_STATE_LISTEN_STRING, BUS_STATE_MAX_STRING_LENGTH);
			break;
		}

		case BUS_STATE_TRANSMIT: {
			strncpy(state_str, BUS_STATE_TRANSMIT_STRING, BUS_STATE_MAX_STRING_LENGTH);
			break;
		}

		default: {
			strncpy(state_str, "invalid", BUS_STATE_MAX_STRING_LENGTH);
			break;
		}
	}

	return state_str;
}

