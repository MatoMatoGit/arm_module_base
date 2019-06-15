/*
 * SensorController.h
 *
 *  Created on: Feb 26, 2019
 *      Author: dorus
 */

#ifndef SENSOR_CONTROLLER_H_
#define SENSOR_CONTROLLER_H_

#include "SystemResult.h"
#include "OsTypes.h"

#include <stdbool.h>

#define SENSOR_CONTROLLER_CONFIG_NUM_SENSORS		 2 /* Max. 255 */
#define SENSOR_CONTROLLER_CONFIG_POLL_RESOLUTION_MS	 1000

typedef int32_t (*SensorReadFn_t)(void);

typedef enum {
	SENSOR_TYPE_WATER_LEVEL = 0,
	SENSOR_TYPE_SOIL_MOISTURE,
	SENSOR_TYPE_TEMPERATURE,
	SENSOR_TYPE_LIGHT_INTENSITY,
}SensorType_t;

typedef enum {
	SENSOR_TRIGGER_NONE = 0,
	SENSOR_TRIGGER_BELOW_THRESHOLD,
	SENSOR_TRIGGER_ABOVE_THRESHOLD,
	SENSOR_TRIGGER_CROSS_THRESHOLD,
}SensorTrigger_t;

typedef struct {
	SensorType_t sensor
	SensorReadFn_t read;
	U32_t poll_interval_s;
	SensorTrigger_t trigger;
	U32_t threshold;
} Sensor_t;

SysResult_t SensorControllerInit(Id_t msg_queue);

SysResult_t SensorControllerAdd(Sensor_t *sensor);

SysResult_t SensorControllerPollEnable(SensorType_t sensor, bool en);


#endif /* SENSOR_CONTROLLER_H_ */
