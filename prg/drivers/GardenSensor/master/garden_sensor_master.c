/*
 * garden_sensor_master.c
 *
 *  Created on: Jun 8, 2019
 *      Author: dorus
 */


#include "garden_sensor_master.h"

#include "crc8.h"
#include "ow_hal_master.h"
#include "ow_ll_master.h"
#include "ow_port_master.h"

#include <stdbool.h>


static bool ReceiveAndVerify(uint8_t *data, uint16_t size);
static uint16_t SensorRead(SensorType_e type);

ow_hal_master_t *OwHal;

SensorStatus_e GardenSensorInit(void)
{
	OwHal = ow_port_master_stm32_get();
	if(ow_ll_master_init(OwHal) == OW_OK) {
		return SENSOR_STATUS_OK;
	}

	return SENSOR_STATUS_ERROR;
}

SensorStatus_e GardenSensorStatusRead(void)
{
	uint8_t status = 0;

	/* Send the read status command. */
	ow_ll_master_transmit_byte(SENSOR_COMMAND_READ_STATUS);

	/* Read the status from the sensor. */
	status = ow_ll_master_receive_byte();

	if(ReceiveAndVerify(&status, sizeof(status))) {
		return status;
	}

	return SENSOR_STATUS_ERROR;
}


uint16_t GardenSensorMoistureRead(void)
{
	return SensorRead(SENSOR_TYPE_MOISTURE);
}

uint16_t GardenSensorLightRead(void)
{
	return SensorRead(SENSOR_TYPE_LIGHT);
}

uint16_t GardenSensorTempRead(void)
{
	return SensorRead(SENSOR_TYPE_TEMP);
}

static bool ReceiveAndVerify(uint8_t *data, uint16_t size)
{
	uint8_t rx_crc = 0;
	uint8_t crc = 0;

	/* Read the CRC. */
	rx_crc = ow_ll_master_receive_byte();

	/* Calculate the CRC over the data. */
	crc = crc8(data, size);

	if(crc != rx_crc) {
		return false;
	} else {
		return true;
	}
}

static uint16_t SensorRead(SensorType_e type)
{
	uint8_t byte = 0;
	uint16_t value = 0;

	if(ow_ll_master_reset() != 0) {
		return 0;
	}

	/* Send the read sensor command. */
	ow_ll_master_transmit_byte(SENSOR_COMMAND_READ_SENSOR);

	/* Send sensor type. */
	ow_ll_master_transmit_byte(type);

	/* Read sensor value bytes. */
	byte = ow_ll_master_receive_byte();
	value |= (byte << 8);

	/* Read sensor value bytes. */
	byte = ow_ll_master_receive_byte();
	value |= byte;

	if(ReceiveAndVerify((uint8_t *)&value, sizeof(value))) {
		return value;
	}

	return 0;
}

