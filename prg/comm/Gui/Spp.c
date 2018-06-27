/*
 * Spp.c
 *
 *  Created on: 24 jun. 2018
 *      Author: Dorus
 */

#include "Spp.h"

#include <string.h>

const uint8_t MsgSof = 0xAD;
#define MSG_SOF_SIZE sizeof(uint8_t)
#define MSG_SOF_OFFSET 0

const uint8_t MsgTypeConnect	= 0x01;
const uint8_t MsgTypeDisconnect	= 0x02;
const uint8_t MsgTypeData		= 0x03;

#define MSG_TYPE_SIZE		sizeof(uint8_t)
#define MSG_TYPE_OFFSET		MSG_SOF_OFFSET + MSG_SOF_SIZE

#define MSG_SIZE_SIZE		sizeof(uint32_t)
#define MSG_SIZE_OFFSET		MSG_TYPE_OFFSET + MSG_TYPE_SIZE

#define MSG_DATA_OFFSET		MSG_SIZE_OFFSET + MSG_SIZE_SIZE


SppCallbackDataReceived_t OnData;
SppCallbackGeneric_t OnConnect;
SppCallbackGeneric_t OnDisconnect;

uint8_t RxBuffer[SPP_MTU_SIZE_BYTES];

MpsResult_t SppInit(SppCallbackGeneric_t on_connect, SppCallbackGeneric_t on_disconnect, SppCallbackDataReceived_t on_data)
{
	OnData = on_data;
}

MpsResult_t SppConnect(void)
{
	return IMessageTransmit(MsgTypeConnect, NULL, 0);
}

MpsResult_t SppDisconnect(void)
{
	return IMessageTransmit(MsgTypeDisconnect, NULL, 0);
}

MpsResult_t SppTransmit(uint8_t *data, uint32_t size)
{
	return IMessageTransmit(MsgTypeData, data, size);
 }



static MpsResult_t IMessageTransmit(uint8_t type, uint8_t *data, uint32_t size)
{

}
