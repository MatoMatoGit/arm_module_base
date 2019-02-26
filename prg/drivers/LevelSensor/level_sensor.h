/*
 * level_sensor.h
 *
 *  Created on: Feb 2, 2019
 *      Author: dorus
 */


#ifndef LEVEL_SENSOR_H_
#define LEVEL_SENSOR_H_

#include "SystemResult.h"

typedef enum {
	LEVEL_SENSOR_STATE_CLOSED	= 0,
	LEVEL_SENSOR_STATE_OPEN		= 1,
}LevelSensorState_t;

SysResult_t LevelSensorInit(void);

LevelSensorState_t LevelSensorStateGet(void);

#endif
