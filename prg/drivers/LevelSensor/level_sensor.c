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
	if(GpioLevelSensorStateGet()) {
		return LEVEL_SENSOR_STATE_CLOSED;
	} else {
		return LEVEL_SENSOR_STATE_OPEN;
	}
}
