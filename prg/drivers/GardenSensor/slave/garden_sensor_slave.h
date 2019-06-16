/*
 * garden_sensor_slave.h
 *
 *  Created on: Jun 16, 2019
 *      Author: dorus
 */

#ifndef GARDEN_SENSOR_SLAVE_H_
#define GARDEN_SENSOR_SLAVE_H_

#include "garden_sensor_common.h"

#include <stdint.h>

typedef uint16_t (*GardenSensorCbRead_t) (SensorType_e);
typedef SensorStatus_e (*GardenSensorCbStatus_t) (void);

SensorStatus_e GardenSensorInit(GardenSensorCbRead_t cb_read, GardenSensorCbStatus_t cb_status);

#endif /* GARDEN_SENSOR_SLAVE_H_ */
