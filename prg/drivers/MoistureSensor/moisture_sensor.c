/*
 * moisture_sensor.c
 *
 *  Created on: Feb 11, 2019
 *      Author: dorus
 */

#include "moisture_sensor.h"

#if MOISTURE_SENSOR_CONFIG_NUM_SENSORS >= 255
#error "MoistureSensor: Number of sensor is higher than allowed maximum (254)."
#endif

typedef struct {
	uint32_t value;
	MoistureSensorFn_t read_sensor;
}MoistureSensorDesc_t;

MoistureSensorDesc_t MoistureSensors[MOISTURE_SENSOR_CONFIG_NUM_SENSORS] = {0};

MoistureSensor_t MoistureSensorInit(MoistureSensorFn_t read_sensor)
{
	MoistureSensor_t handle = NULL;

	for(uint8_t i = 0; i < MOISTURE_SENSOR_CONFIG_NUM_SENSORS; i++) {
		if(MoistureSensors[i].read_sensor != NULL) {
			handle = &MoistureSensors[i];
			break;
		}
	}

	if(handle != NULL) {
		handle->read_sensor = read_sensor;
	}

	return handle;
}

uint32_t MoistureSensorRead(MoistureSensor_t handle)
{
	handle->value = handle->read_sensor();

	return handle->value;
}
