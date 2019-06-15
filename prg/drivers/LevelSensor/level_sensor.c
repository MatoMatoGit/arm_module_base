/*
 * level_sensor.c
 *
 *  Created on: Feb 2, 2019
 *      Author: dorus
 */


#include "level_sensor.h"

#include "gpio.h"

SysResult_t LevelSensorInit(void)
{
	SysResult_t res = SYS_RESULT_OK;

	GpioLevelSensorInit();

	return res;
}

LevelSensorState_t LevelSensorStateGet(void)
{
	int8_t avg_state = 0;

	for(uint8_t i = 0; i < LEVEL_SENSOR_CONFIG_NUM_SAMPLES; i++) {
		if(GpioLevelSensorStateGet()) {
			avg_state++;
		} else {
			avg_state--;
		}
	}

	if(avg_state > 0) {
		return LEVEL_SENSOR_STATE_CLOSED;
	} else {
		return LEVEL_SENSOR_STATE_OPEN;
	}
}
