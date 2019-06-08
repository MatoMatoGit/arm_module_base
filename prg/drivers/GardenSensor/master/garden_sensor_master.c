/*
 * garden_sensor_master.c
 *
 *  Created on: Jun 8, 2019
 *      Author: dorus
 */


#include "garden_sensor_master.h"

static bool ReceiveAndVerify(uint8_t *data, uint16_t size);
static uint16_t SensorRead(SensorType_e type);

SensorStatus_e GardenSensorStatusRead(void)
{
	uint8_t status = 0;

	/* Send the read status command. */
	ow_ll_master_transmit_byte(SENSOR_COMMAND_READ_STATUS);

	/* Read the status from the sensor. */
	status = ow_ll_master_receive_byte();

	if(ReceiveAndVerify(&status, sizeof(status)) {
		return SENSOR_STATUS_ERROR;
	} else {
		return (SensorStatus_e) status;
	}
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
	crc = crc8(&status, sizeof(status));

	if(crc != rx_crc) {
		return false;
	} else {
		return true;
	}
}

static uint16_t SensorRead(SensorType_e type)
{
	uint8_t data = 0;

	if(ow_ll_master_reset() != 0) {
		return 0;
	}

	/* Send the read sensor command. */
	ow_ll_master_transmit_byte(SENSOR_COMMAND_READ_SENSOR);

	/* Send sensor type. */
	ow_ll_master_transmit_byte(type);

	/* Read sensor value bytes. */
	data = ow_ll_master_receive_byte();

	if(ReceiveAndVerify(&data, sizeof(data)) {
		return 0;
	} else {
		return data;
	}
}

