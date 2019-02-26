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
#define SENSOR_CONTROLLER_CONFIG_POLL_INTERVAL_MS	 1000

typedef enum {
	SENSOR_TYPE_WATER_LEVEL = 0,
	SENSOR_TYPE_SOIL_MOISTURE,
}SensorType_t;

typedef enum {
	SENSOR_TRIGGER_BELOW_THRESHOLD = 0,
	SENSOR_TRIGGER_ABOVE_THRESHOLD,
	SENSOR_TRIGGER_CROSS_THRESHOLD,
}SensorTrigger_t;

SysResult_t SensorControllerInit(Id_t msg_queue);

SysResult_t SensorControllerAdd(SensorType_t sensor, U32_t poll_interval_s, SensorTrigger_t trigger, U32_t threshold);

SysResult_t SensorControllerPollEnable(SensorType_t sensor, bool en);


#endif /* SENSOR_CONTROLLER_H_ */
