/*
 * SensorController.c
 *
 *  Created on: Feb 26, 2019
 *      Author: dorus
 */


#include "SensorController.h"

#include "include/Timer.h"

#include "drivers/level_sensor.h"
#include "drivers/moisture_sensor.h"

#include <string.h> /* For memset. */

typedef struct {
	SensorType_t type;
	U32_t interval_s;
	U32_t passed_s;
	SensorTrigger_t trigger;
	U32_t threshold_value;
	U32_t current_value;
	bool enabled;
}Sensor_t;

void ICallbackSensorPoll(Id_t timer_id, void *context);

static Sensor_t Sensors[SENSOR_CONTROLLER_CONFIG_NUM_SENSORS];
static Id_t MsgQueue = ID_INVALID;
static Id_t TmrSensorPoll = ID_INVALID;

SysResult_t SensorControllerInit(Id_t msg_queue)
{
	memset(Sensors, 0, sizeof(Sensors));

	MsgQueue = msg_queue;

	TmrSensorPoll = TimerCreate(SENSOR_CONTROLLER_CONFIG_POLL_RESOLUTION_MS,
			(TIMER_PARAMETER_PERIODIC | TIMER_PARAMETER_ON),
			ICallbackSensorPoll, NULL);

}

SysResult_t SensorControllerAdd(SensorType_t sensor, U32_t poll_interval_s, SensorTrigger_t trigger, U32_t threshold_value)
{
	Sensor_t *sensor_inst = NULL;

	for(U8_t i = 0; i < SENSOR_CONTROLLER_CONFIG_NUM_SENSORS; i++) {
		if(Sensors[i].interval_s == 0) {
			sensor_inst = &Sensors[i];
			break;
		}
	}

	if(sensor_inst != NULL) {
		sensor_inst->type = sensor;
		sensor_inst->interval_s = poll_interval_s;
		sensor_inst->trigger = trigger;
		sensor_inst->threshold_value = threshold_value;
		sensor_inst->enabled = false;
		sensor_inst->passed_s = 0;
		sensor_inst->current_value = 0;
	}
}

SysResult_t SensorControllerPollEnable(SensorType_t sensor, bool en)
{
	for(U8_t i = 0; i < SENSOR_CONTROLLER_CONFIG_NUM_SENSORS; i++) {
		if(Sensors[i].type == sensor && Sensors[i].interval_s != 0) {
			Sensors[i].enabled = en;
		}
	}
}

void ICallbackSensorPoll(Id_t timer_id, void *context)
{
	bool above_threshold = false;
	bool send_message = false;

	for(U8_t i = 0; i < SENSOR_CONTROLLER_CONFIG_NUM_SENSORS; i++) {
		if(Sensors[i].enabled && Sensors[i].interval_s != 0) {
			Sensors[i].passed_s++;
			if(Sensors[i].passed_s >= Sensors[i].interval_s) {
				Sensors[i].passed_s = 0;

				/* Determine if sensor value was above the threshold in
				 * the previous cycle. */
				if(Sensors[i].current_value >= Sensors[i].threshold_value) {
					above_threshold = true;
				}

				/* Read the current sensor value. */
				switch(Sensors[i].type) {
				case SENSOR_TYPE_WATER_LEVEL: {
					Sensors[i].current_value = LevelSensorStateGet();
					break;
				}
				case SENSOR_TYPE_SOIL_MOISTURE: {
					Sensors[i].current_value = MoistureSensorRead();
					break;
				}
				}

				/* Determine if a sensor message should be sent based
				 * on the trigger and threshold value. */
				switch(Sensors[i].trigger) {
				case SENSOR_TRIGGER_ABOVE_THRESHOLD: {
					if(Sensors[i].current_value >= Sensors[i].threshold_value) {
						send_message = true;
					}
					break;
				}
				case SENSOR_TRIGGER_BELOW_THRESHOLD: {
					if(Sensors[i].current_value < Sensors[i].threshold_value) {
						send_message = true;
					}
					break;
				}
				case SENSOR_TRIGGER_CROSS_THRESHOLD: {
					/* Send a message if the sensor value was above threshold, and is
					 * now below the threshold.
					 * OR if the sensor value was below the threshold, and is now
					 * above the threshold. */
					if(Sensors[i].current_value < Sensors[i].threshold_value && above_threshold) {
						send_message = true;
					} else if(Sensors[i].current_value >= Sensors[i].threshold_value && !above_threshold) {
						send_message = true;
					}
					break;
				}

				}

				/* Send the sensor message. */
				if(send_message) {
					ISendSensorMessage(Sensors[i].type, Sensors[i].trigger);
				}
			}
		}
	}
}

static void ISendSensorMessage(SensorType_t sensor, SensorTrigger_t trigger)
{
	//MessageSend(MsgQueue, x);
}
