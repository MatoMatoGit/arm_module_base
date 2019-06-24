/*
 * garden_sensor_common.h
 *
 *  Created on: Jun 8, 2019
 *      Author: dorus
 */

#ifndef GARDEN_SENSOR_COMMON_H_
#define GARDEN_SENSOR_COMMON_H_

typedef enum {
	SENSOR_COMMAND_READ_STATUS	= 0x00,
	SENSOR_COMMAND_READ_SENSOR	= 0x01,
	SENSOR_COMMAND_INVALID		= 0xFF,
} SensorCommand_e;

typedef enum {
	SENSOR_STATUS_OK 	= 0x00,
	SENSOR_STATUS_BUSY 	= 0x01,
	SENSOR_STATUS_ERROR = 0xFF
} SensorStatus_e;

typedef enum {
	SENSOR_TYPE_MOISTURE	= 0x00,
	SENSOR_TYPE_LIGHT		= 0x01,
	SENSOR_TYPE_TEMP		= 0x02,
	SENSOR_TYPE_NONE		= 0xFF,
} SensorType_e;


#endif /* GARDEN_SENSOR_COMMON_H_ */
