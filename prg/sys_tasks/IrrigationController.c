/*
 * IrrigationController.c
 *
 *  Created on: 21 mei 2018
 *      Author: Dorus
 */

/* Self include. */
#include "IrrigationController.h"

/* System tasks includes. */
#include "Composer.h"
#include "SystemManager.h"

/* Driver includes. */
#include "Pump/pump.h"
#include "LevelSensor/level_sensor.h"
#include "MoistureSensor/moisture_sensor.h"

/* OS includes. */
#include "PriorRTOS.h"

/* Standard includes. */
#include <stdlib.h>
#include <string.h>
#include "SystemEvg.h"

LOG_FILE_NAME("IrrigationController");

#define PUMP_RUN_DELAY_MS	5000
#define PUMP_DURATION_HOUR	3600
#define LITER_TO_MILLILITER(ltr) (ltr * 1000)

#define MBOX_IRRIGATION_NUM_ADDR 2

static U32_t PumpAmountMl = 0;
static volatile U16_t MoistureThreshold = 0;
static MoistureSensor_t MoistureSensor = NULL;

static Id_t MboxIrrigation = ID_INVALID;
static volatile Id_t TmrIrrigationDelay = ID_INVALID;
static Id_t TmrMoistureSensorCheck = ID_INVALID;

static void ICallbackPumpStopped(void);
static void ICallbackDelayedPumpRun(Id_t timer_id, void *context);
static void ICallbackLevelSensorCheck(LevelSensorState_t state);
static void ICallbackMoistureSensorCheck(Id_t timer_id, void *context);

static void IrrigationControllerTask(const void *p_arg, U32_t v_arg);

SysResult_t IrrigationControllerInit(Id_t *mbox_irrigation)
{
	SysResult_t res = SYS_RESULT_OK;
	Id_t tsk_irrigation_controller = ID_INVALID;

	tsk_irrigation_controller = TaskCreate(IrrigationControllerTask, TASK_CAT_HIGH, 5,
		(TASK_PARAMETER_ESSENTIAL), 0, NULL, 0);
	*mbox_irrigation = MailboxCreate(MBOX_IRRIGATION_NUM_ADDR, &tsk_irrigation_controller, 1);
	TmrMoistureSensorCheck = TimerCreate(IRRIGATION_CONTROLLER_CONFIG_MOISTURE_CHECK_INTERVAL_MS, (TIMER_PARAMETER_PERIODIC | TIMER_PARAMETER_ON),
			ICallbackMoistureSensorCheck, MoistureSensor);
	if(tsk_irrigation_controller == ID_INVALID || *mbox_irrigation == ID_INVALID) {
		LOG_ERROR_NEWLINE("Failed to create IrrigationController task or Irrigation mailbox");
		res = SYS_RESULT_ERROR;
	}

	if(res == SYS_RESULT_OK) {
		MboxIrrigation = *mbox_irrigation;
		LOG_DEBUG_NEWLINE("IrrigationController initialized.");
		ComposerCallbackSetOnPumpStopped(ICallbackPumpStopped);
		PumpEnable(1);
		TaskResume(tsk_irrigation_controller);
	}
	
	return res;
}


static void IrrigationControllerTask(const void *p_arg, U32_t v_arg)
{
	U16_t threshold = 0;
	U16_t amount = 0;
	OsResult_t res = OS_RES_ERROR;
	SysResult_t pump_res = SYS_RESULT_OK;
	U16_t trigger = 0;

	TASK_INIT_BEGIN() {
		mbox_irrigation = (Id_t)v_arg;
	} TASK_INIT_END();

	res = MailboxPend(MboxIrrigation, IRRIGATION_MBOX_ADDR_MOISTURE_THRESHOLD, &threshold, OS_TIMEOUT_INFINITE);
	/* Update the moisture threshold if the pend operation
	 * was successful. */
	if(res == OS_RES_OK || res == OS_RES_EVENT) {
		MoistureThreshold = threshold;
	}

	res = MailboxPend(MboxIrrigation, IRRIGATION_MBOX_ADDR_TRIGGER, &trigger, OS_TIMEOUT_INFINITE);
	/* If a mailbox event was received, check the Level Sensor state.
	 * The Level Sensor state must be closed, which indicates the pump
	 * is submerged. */
	if(res == OS_RES_OK || res == OS_RES_EVENT) {
		if(LevelSensorStateGet() == LEVEL_SENSOR_STATE_OPEN) {
			SystemRaiseError(SYSTEM_COMP_APP_IRRIGATION_CONTROLLER, SYSTEM_ERROR, ERROR_WATER_LEVEL);
			pump_res = SYS_RESULT_ERROR;

		} else {
			pump_res = SYS_RESULT_OK;
		}
	}

	if(pump_res == SYS_RESULT_OK) {
		switch(trigger) {

			/* If the trigger is manual on the pump is ran indefinitely until
			 * a manual off trigger is received. */
			case IRRIGATION_TRIGGER_MANUAL_ON: {
				LOG_DEBUG_NEWLINE("Received trigger: manual, on.");
				pump_res = PumpRun();
				if(pump_res == SYS_RESULT_OK) {
					LevelSensorProbeStart();
					EventgroupFlagsSet(SystemEvgGet(), SYSTEM_FLAG_PUMP_RUNNING);
					LOG_DEBUG_NEWLINE("Pump turned on.");
				} else {
					LOG_ERROR_NEWLINE("Pump could not be turned on.");
				}
				break;
			}

			/* If the trigger is manual off the pump is stopped. */
			case IRRIGATION_TRIGGER_MANUAL_OFF: {
				LOG_DEBUG_NEWLINE("Received trigger: manual, off.");
				PumpStop();
				LevelSensorProbeStop();
				EventgroupFlagsClear(SystemEvgGet(), SYSTEM_FLAG_PUMP_RUNNING);
				LOG_DEBUG_NEWLINE("Pump turned off.");
				break;
			}

			/* If the trigger is the schedule the pump is ran for the amount received
			 * in the mailbox. */
			case IRRIGATION_TRIGGER_SCHEDULE: {
				LOG_DEBUG_NEWLINE("Received trigger: schedule.");

				/* Update the amount if specified in the mailbox. */
				res = MailboxPend(MboxIrrigation, IRRIGATION_MBOX_ADDR_AMOUNT, &amount, OS_TIMEOUT_NONE);
				if(res == OS_RES_OK || res == OS_RES_EVENT) {
					LOG_DEBUG_NEWLINE("Received amount: %u L.", amount);
					PumpAmountMl = (U32_t)LITER_TO_MILLILITER(amount); /* Convert liters to milliliters and store it. */

				}

				/* If the pump is already running due to a manual trigger, the
				 * scheduled run is delayed. This is because the scheduled
				 * run must still occur. */
				if(PumpIsRunning()) {
					LOG_DEBUG_NEWLINE("Pump is already running. Delaying scheduled run.");
					if(TmrIrrigationDelay == ID_INVALID) {
						TmrIrrigationDelay = TimerCreate(PUMP_RUN_DELAY_MS, (TIMER_PARAMETER_PERIODIC | TIMER_PARAMETER_ON),
								ICallbackDelayedPumpRun, NULL);
						if(TmrIrrigationDelay == ID_INVALID) {
							LOG_ERROR_NEWLINE("Delay timer was not created.");
						}
					}
				} else {
					/* If the pump is not running, run it for the specified amount. */
					pump_res = PumpRunForAmount(PumpAmountMl);
					if(pump_res == SYS_RESULT_OK) {
						LevelSensorProbeStart();
						EventgroupFlagsSet(SystemEvgGet(), SYSTEM_FLAG_PUMP_RUNNING);
						LOG_DEBUG_NEWLINE("Pump turned on.");
					} else {
						LOG_ERROR_NEWLINE("Pump could not be turned on.");
						SystemRaiseError(SYSTEM_COMP_APP_IRRIGATION_CONTROLLER, SYSTEM_ERROR, ERROR_PUMP_ACTIVATION);
					}
				}

				break;
			}

			/* Invalid trigger value. */
			default: {
				LOG_ERROR_NEWLINE("Received trigger: invalid.");
				break;
			}
		}
	}

	TaskSleep(1000);
}

static void ICallbackPumpStopped(void)
{
	LevelSensorProbeStop();
	EventgroupFlagsClear(SystemEvgGet(), SYSTEM_FLAG_PUMP_RUNNING);
	LOG_DEBUG_NEWLINE("Pump stopped.");
}

static void ICallbackDelayedPumpRun(Id_t timer_id, void *context)
{
	OS_ARG_UNUSED(context);
	OS_ARG_UNUSED(timer_id);

	/* If the pump is still running after the delay, the delay timer is reset.
	 * If the pump is not running anymore, the pump is ran to pump the scheduled
	 * amount. */
	if(PumpIsRunning()) {
		LOG_DEBUG_NEWLINE("Delay expired. Pump is still running. Delay again.");
		TimerReset(TmrIrrigationDelay);
	} else {
		if(PumpRunForAmount(PumpAmountMl) == SYS_RESULT_OK) {
			LevelSensorProbeStart();
			EventgroupFlagsSet(SystemEvgGet(), SYSTEM_FLAG_PUMP_RUNNING);
			LOG_DEBUG_NEWLINE("Delay expired. Pump turned on.");
		} else {
			LOG_ERROR_NEWLINE("Pump could not be turned on.");
			SystemRaiseError(SYSTEM_COMP_APP_IRRIGATION_CONTROLLER, SYSTEM_ERROR, ERROR_PUMP_ACTIVATION);
		}
		PumpAmountMl = 0;
		TimerDelete((Id_t *)&TmrIrrigationDelay);
	}
}

static void ICallbackLevelSensorCheck(LevelSensorState_t state)
{
	/* If the Level Sensor is open the pump is no longer
	 * submerged. The pump is stopped and disabled.
	 * The Level Sensor probe must be restarted to re-enable
	 * the pump when the tank has been refilled. */
	if(state == LEVEL_SENSOR_STATE_OPEN) {
		PumpStop();
		PumpEnable(0);
		SystemRaiseError(SYSTEM_COMP_APP_IRRIGATION_CONTROLLER, SYSTEM_ERROR, ERROR_WATER_LEVEL);
		LevelSensorProbeStart();
	} else {
		LevelSensorProbeStop();
		PumpEnable(1);
	}
}

static void ICallbackMoistureSensorCheck(Id_t timer_id, void *context)
{
	U16_t trigger = IRRIGATION_TRIGGER_SCHEDULE;
	U32_t value = MoistureSensorRead((MoistureSensor_t *)context);

	/* Check if the value is below the set threshold, if it is
	 * trigger a irrigation cycle. */
	if((U16_t)value <= MoistureThreshold) {
		MailboxPost(MboxIrrigation, IRRIGATION_MBOX_ADDR_AMOUNT, &trigger, OS_TIMEOUT_NONE);
		TimerIntervalSet(timer_id, IRRIGATION_CONTROLLER_CONFIG_MOISTURE_BACKOFF_INTERVAL_MS);
	}
}

