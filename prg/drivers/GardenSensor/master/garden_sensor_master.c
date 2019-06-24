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
#include "ow_port_macro.h"
#include "gpio.h"

#include <stdbool.h>


static bool ReceiveAndVerify(uint8_t *data, uint16_t size);
static bool SensorReadData(SensorType_e type, SensorCommand_e cmd, uint16_t len, uint8_t *data);
static uint16_t SensorReadValue(SensorType_e type);

static ow_hal_master_t *OwHal;

SensorStatus_e GardenSensorInit(void)
{
	GpioOneWireInit();
	OwHal = ow_port_master_stm32_get();
	if(ow_ll_master_init(OwHal) == OW_OK) {
		return SENSOR_STATUS_OK;
	}

	return SENSOR_STATUS_ERROR;
}

SensorStatus_e GardenSensorStatusRead(void)
{
	uint8_t status = 0;

	if(SensorReadData(SENSOR_TYPE_NONE, SENSOR_COMMAND_READ_STATUS, sizeof(status),
			&status) == true) {
		return (SensorStatus_e)status;
	}

	return SENSOR_STATUS_ERROR;
}


uint16_t GardenSensorMoistureRead(void)
{
	return SensorReadValue(SENSOR_TYPE_MOISTURE);

}

uint16_t GardenSensorLightRead(void)
{
	return SensorReadValue(SENSOR_TYPE_LIGHT);
}

uint16_t GardenSensorTempRead(void)
{
	return SensorReadValue(SENSOR_TYPE_TEMP);
}


/***** Internal functions *****/

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

static bool SensorReadData(SensorType_e type, SensorCommand_e cmd, uint16_t len, uint8_t *data)
{
	uint8_t n_retries = GARDEN_SENSOR_CONFIG_NUM_RETRIES;

	do {
		if(n_retries > 0) {
			n_retries--;
		}

		if(ow_ll_master_reset() != 0) {
			OW_PORT_WAIT_US(500);
			continue;
		}

		/* Send the sensor command. */
		ow_ll_master_transmit_byte(cmd);

		if(type != SENSOR_TYPE_NONE) {
			/* Send sensor type. */
			ow_ll_master_transmit_byte(type);
		}

		for(uint16_t i = 0; i < len; i++) {
			/* Read a byte from the sensor. */
			data[i] = ow_ll_master_receive_byte();
		}


		if(ReceiveAndVerify(data, len)) {
			return true;
		}
		OW_PORT_WAIT_US(500);

	} while(n_retries);

	return false;
}

static uint16_t SensorReadValue(SensorType_e type)
{
	uint16_t value = 0;

	if(SensorReadData(type, SENSOR_COMMAND_READ_SENSOR, sizeof(value),
			(uint8_t *)&value) == true) {
		return value;
	}

	return 0;
}
