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

typedef void (*LevelSensorCbStateChanged_t)(LevelSensorState_t state)

SysResult_t LevelSensorInit(uint32_t probe_interval_ms, LevelSensorCbStateChanged_t on_state_change);

SysResult_t LevelSensorProbeStart(void);

SysResult_t LevelSensorProbeStop(void);

LevelSensorState_t LevelSensorStateGet(void);

#endif
