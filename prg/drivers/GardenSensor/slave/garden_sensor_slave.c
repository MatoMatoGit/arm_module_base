/*
 * garden_sensor_slave.c
 *
 *  Created on: Jun 16, 2019
 *      Author: dorus
 */


#include <GardenSensor/slave/garden_sensor_slave.h>
#include <ow_common.h>
#include <OneWire/slave/ow_cmd_slave.h>
#include <OneWire/slave/ow_hal_slave.h>
#include <string.h>
#include <sys/_stdint.h>

static uint16_t OwCmdReceiveTransmit(uint8_t rx_data, uint8_t **tx_data);
static void OwError(void);
static void OwReset(void);

static ow_hal_slave_t *OwHal;
static ow_cmd_slave_cbs_t OwCbs;
static GardenSensorCbRead_t CbRead;
static GardenSensorCbStatus_t CbStatus;

static volatile SensorCommand_e Cmd = SENSOR_COMMAND_INVALID;
static volatile SensorType_e Type = SENSOR_TYPE_INVALID;
static volatile uint8_t Data[sizeof(uint16_t)] = {0};

SensorStatus_e GardenSensorInit(GardenSensorCbRead_t cb_read, GardenSensorCbStatus_t cb_status)
{
	if(cb_read == NULL || cb_status == NULL) {
		return SENSOR_STATUS_ERROR;
	}

	SensorStatus_e res = SENSOR_STATUS_OK;

	CbRead = cb_read;
	CbStatus = cb_status;

	OwHal = ow_port_slave_avr8_get();
	OwCbs.receive_transmit_data = OwCmdReceiveTransmit;
	OwCbs.error = OwError;
	OwCbs.reset = OwReset;

	if(ow_cmd_slave_init(OwHal, &OwCbs) != OW_OK) {
		res = SENSOR_STATUS_ERROR;
	} else {
		ow_cmd_slave_start();
	}

	return res;
}

static uint16_t OwCmdReceiveTransmit(uint8_t rx_data, uint8_t **tx_data)
{
	uint16_t len = 0;
	uint16_t sensor_value = 0;

	if(Cmd == SENSOR_COMMAND_INVALID) {
		Cmd = rx_data;
	} else {
		if(Type == SENSOR_TYPE_NONE) {
			Type = rx_data;
		} else {
			switch(Cmd) {
			case SENSOR_COMMAND_READ_STATUS:
				Data[0] = (uint8_t)CbStatus();
				len = 1;
				break;

			case SENSOR_COMMAND_READ_SENSOR:
				sensor_value = CbRead(Type);
				Data[0] = (uint8_t)((sensor_value  >> 8) & 0x00FF);
				Data[1] = (uint8_t)(sensor_value & 0x00FF);
				len = sizeof(Data);
				break;
			}
		}
	}

	*tx_data = Data;
	return len;
}

static void OwError(void)
{
	Cmd = SENSOR_COMMAND_INVALID;
	Type = SENSOR_TYPE_NONE;
	memset(Data, 0, sizeof(Data));
}

static void OwReset(void)
{
	Cmd = SENSOR_COMMAND_INVALID;
	Type = SENSOR_TYPE_NONE;
	memset(Data, 0, sizeof(Data));
}
