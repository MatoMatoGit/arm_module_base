/*
 * level_sensor.c
 *
 *  Created on: Feb 2, 2019
 *      Author: dorus
 */


#include "level_sensor.h"

#include "Timer.h"

#include "gpio.h"

static void ICallbackTmrProbe(Id_t timer_id);

static LevelSensorCbStateChanged_t OnStateChange = NULL;
static Id_t TmrProbe = ID_INVALID;

SysResult_t LevelSensorInit(uint32_t probe_interval_ms, LevelSensorCbStateChanged_t on_state_change)
{
	SysResult_t res = SYS_RESULT_OK;

	TmrProbe = TimerCreate(probe_interval_ms, (TIMER_PARAMETER_PERIODIC), ICallbackTmrProbe);
	if(TmrProbe == ID_INVALID) {
		res = SYS_RESULT_ERROR;
	}

	OnStateChange = on_state_change;

	GpioLevelSensorInit();

	return res;
}

SysResult_t LevelSensorProbeStart(void)
{
	TimerReset(TmrProbe);
	TimerStart(TmrProbe);
}

SysResult_t LevelSensorProbeStop(void)
{
	TimerStop(TmrProbe);
}

LevelSensorState_t LevelSensorStateGet(void)
{
	if(GpioLevelSensorStateGet()) {
		return LEVEL_SENSOR_STATE_CLOSED;
	} else {
		return LEVEL_SENSOR_STATE_OPEN;
	}
}


static void ICallbackTmrProbe(Id_t timer_id)
{
	static LevelSensorState_t cur_state = LEVEL_SENSOR_STATE_CLOSED;
	LevelSensorState_t new_state = LEVEL_SENSOR_STATE_CLOSED;

	/* Check the Level Sensor state, if the new state
	 * is different from the current state: call
	 * the OnStateChange callback. */
	new_state = LevelSensorStateGet();
	if(cur_state != new_state) {
		cur_state = new_state;
		if(OnStateChange != NULL) {
			OnStateChange(cur_state);
		}
	}
}
