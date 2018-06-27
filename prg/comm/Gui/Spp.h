/*
 * Spp.h
 *
 *  Created on: 24 jun. 2018
 *      Author: Dorus
 */

#ifndef SPP_H_
#define SPP_H_

#include <stdint.h>

/* Simple Point-to-Point protocol. */

#define SPP_MTU_SIZE_BYTES	256
#define SPP_CONN_TIMEOUT_MS	500

typedef void (*SppCallbackDataReceived_t)(uint8_t *data, uint32_t size);
typedef void (*SppCallbackGeneric_t)(void);

MpsResult_t SppInit(SppCallbackGeneric_t on_connect, SppCallbackGeneric_t on_disconnect, SppCallbackDataReceived_t on_data);

MpsResult_t SppConnect(void);

MpsResult_t SppDisconnect(void);

MpsResult_t SppTransmit(uint8_t *data, uint32_t size);


#endif /* SPP_H_ */
