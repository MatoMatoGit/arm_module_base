#ifndef GARDEN_SENSOR_MASTER_H_
#define GARDEN_SENSOR_MASTER_H_

#include "garden_sensor_common.h"

#include <stdint.h>

#define GARDEN_SENSOR_CONFIG_NUM_RETRIES 15

SensorStatus_e GardenSensorInit(void);

SensorStatus_e GardenSensorStatusRead(void);

uint16_t GardenSensorMoistureRead(void);

uint16_t GardenSensorLightRead(void);

uint16_t GardenSensorTempRead(void);

#endif
