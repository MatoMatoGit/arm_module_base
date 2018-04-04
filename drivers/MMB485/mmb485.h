/*
 * mmb485.h
 *
 *  Created on: 2 apr. 2018
 *      Author: Dorus
 */

#ifndef MMB485_H_
#define MMB485_H_

#include <stdint.h>
#include <stdbool.h>

#include <Types.h> /* PriorRTOS types. */

/* RS485 Mulit-Master Bus driver library for half-duplex configurations. */

/* Time-window in milliseconds to transmit
 * after winning arbitration.  */
#define MMB485_ACCESS_WINDOW_MS 1000

#define MMB485_RESULT_OK	0
#define MMB485_RESULT_FAIL	-1
#define MMB485_RESULT_ERR	-2

#define MMB485_TIMEOUT_INF	0xFFFFFFFF

typedef enum {
	MSG_PRIO_CRITICAL = 0,
	MSG_PRIO_HIGH,
	MSG_PRIO_MEDIUM,
	MSG_PRIO_LOW
}MsgPrio_t;

typedef enum  {
	BUS_STATE_UNINIT = -2,
	BUS_STATE_HIGH_Z = -1, /* High impedance state. */
	BUS_STATE_LISTEN = 0,
	BUS_STATE_ARBITRATION,
	BUS_STATE_BUS_CLAIMED,
	BUS_STATE_TRANSMIT
}BusState_t;

struct MmbDescriptor;
typedef struct MmbDescriptor MmbDescriptor_t;

typedef void (*CallbackArbitrationResult_t)(MmbDescriptor_t *desc, int result);
typedef void (*CallbackDataReceived_t)(MmbDescriptor_t *desc, uint8_t *data, uint32_t size);


typedef void (*HalInit_t)(uint32_t baud_rate);

typedef void (*HalReceiverEnable_t)(uint8_t val);

typedef void (*HalTransmitterEnable_t)(uint8_t val);

typedef void (*HalTransmit_t)(uint8_t *data, uint32_t size);


typedef struct MmbDescriptor_t {
	/* Initializes the USART at the given baud rate (in bps)
	 * and the necessary GPIO pins.
	 * Caller: Driver lib. */
	HalInit_t init;

	/* Drives the Receiver Enable pin (if available) and
	 * enable/disable the DataReceived callback.
	 * Caller: Driver lib. */
	HalReceiverEnable_t receiver_en;

	/* Drives the Transmitter Enable pin (if available).
	 * Caller: Driver lib. */
	HalTransmitterEnable_t transmitter_en;

	/* Transmits the data of given size over the RS485 lines.
	 * Caller: Driver lib. */
	HalTransmit_t transmit;
}MmbHal_t;

/* Data is received on the RS485 lines.
 * Caller: HAL. */
void Mmb485HalCallbackDataReceived(MmbDescriptor_t *desc, uint8_t *data, uint32_t size);

/* MMB Descriptor. */
typedef struct MmbDescriptor{
	/***** Set before Init. *****/

	uint32_t baud_rate;

	/* Called when the result of the started arbitration round is known.
	 * result is equal to MMB485_RESULT_OK when the round is won.
	 * result is equal to MMB485_RESULT_FAIL when the round is lost. */
	CallbackArbitrationResult_t on_arbitration_result;

	/* Called when data was received. */
	CallbackDataReceived_t on_data_received;

	MmbHal_t hal;

	/***** Read-only. *****/
	BusState_t state;	 	/* Bus state. */
	uint32_t timeout_ms; 	/* Listen/Transmit/Arbitration Timeout in ms. */
	uint32_t rem_window_ms; /* Remaining window in ms. */
	uint32_t bytes_left;	/* Number of bytes left to transmit or listen for. */
	bool always_listen; 	/* Automatically return to the 'listen' state. */
	Id_t arb_tmr;			/* Arbitration timer. */
	Id_t timeout_tmr;		/* Listen/Transmit/Arbitration timeout timer. */
}MmbDescriptor_t;

int Mmb485Init(MmbDescriptor_t *desc);

/* Request bus access to transmit data with given priority.
 * This is done through an arbitration round.
 * The ArbitrationResult callback is called when the arbitration
 * result is known or when the timeout is reached, in which case
 * it failed by default. */
int Mmb485ArbitrationStart(MmbDescriptor_t *desc, MsgPrio_t prio, uint32_t timeout_ms);

int Mmb485Transmit(MmbDescriptor_t *desc, uint8_t *data, uint32_t size);

int Mmb485Listen(MmbDescriptor_t *desc, uint32_t timeout_ms);

int Mmb485ListenNBytes(MmbDescriptor_t *desc, uint32_t n_bytes, uint32_t timeout_ms);

/* val = 1 to enable, 0 to disable. */
int Mmb485AlwaysListenEnable(MmbDescriptor_t *desc, uint8_t val);

#endif /* MMB485_H_ */
