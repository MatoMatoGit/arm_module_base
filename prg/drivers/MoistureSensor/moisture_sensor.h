/*
 * moisture_sensor.h
 *
 *  Created on: Feb 11, 2019
 *      Author: dorus
 */

#ifndef MOISTURE_SENSOR_H_
#define MOISTURE_SENSOR_H_

#define MOISTURE_SENSOR_CONFIG_NUM_SENSORS 2 /* Max. 254 */

typedef struct MoistureSensorDesc_t* MoistureSensor_t;

typedef uint32_t (*MoistureSensorFn_t)(void);

MoistureSensor_t MoistureSensorInit(MoistureSensorFn_t read_sensor);

uint32_t MoistureSensorRead(MoistureSensor_t handle);


#endif /* MOISTURE_SENSOR_H_ */
