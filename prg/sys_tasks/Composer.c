/*
 * Composer.c
 *
 *  Created on: 1 aug. 2018
 *      Author: Dorus
 */
#include <config/sys_config.h>
#include "Composer.h"

#include "PriorRTOS.h"

/* System configuration. */
#include "sys_config.h"

/* Drivers. */
#include "storage.h"
#include "pump.h"
#include "time.h"
#include "rgb_led.h"
#include "seven_seg.h"
#include "seven_seg_port.h"

/* Tasks. */
#include "IrrigationController.h"
#include "ScheduleManager.h"
#include "UiController.h"

struct SevenSegDisplay SevenSegConfig = {
	.num_digits = DISPLAY_NUM_DIGITS,
	.max_value = DISPLAY_MAX_VALUE,
	.mode = SEVEN_SEG_MODE_DEC,
	.refresh_rate_hz = DISPLAY_REFRESH_RATE
};

ScheduleManagerConfig_t ScheduleManagerConfig = {
	.mbox_schedule = ID_INVALID,
	.mbox_irrigation = ID_INVALID,
	.evg_alarm = ID_INVALID,
	.evg_alarm_flag = EVG_SYSTEM_FLAG_ALARM,
};

const U32_t FileSizes[FILE_NUM] = {
	FILE_SIZE_LOG,
	FILE_SIZE_SCHEDULE,
};

static void ComposerTask(void *p_arg, U32_t v_arg);
static void IComposerCallbackOnPumpStopped(void);

static PumpCallback_t OnPumpStopped = NULL;

SysResult_t ComposerInit(void)
{
	SysResult_t res = SYS_RESULT_INV_ARG;

	res = SYS_RESULT_ERROR;

	if(TaskCreate(ComposerTask, TASK_CAT_HIGH, 5, TASK_PARAMETER_START, 0, NULL, 0) != ID_INVALID) {
		res = SYS_RESULT_OK;
	}

	return res;
}

static void ComposerTask(void *p_arg, U32_t v_arg)
{
	Id_t evg_system = ID_INVALID;
	Id_t mbox_irrigation = ID_INVALID;
	Id_t mbox_schedule = ID_INVALID;

	SysResult_t res = SYS_RESULT_OK;
	
	/* Create common eventgroup. */
	evg_system = EventgroupCreate();
	if(evg_system == ID_INVALID) {
		res = SYS_RESULT_ERROR;
	}
	
	/***** Drivers. *****/
	
	/* Initialize RGB LED driver. */
	if(res == SYS_RESULT_OK) {
		if(RgbLedInit(void) != RGB_LED_OK) {
			res = SYS_RESULT_ERROR;
		}
	}
	
	/* Initialize Seven Segment display driver. */
	if(res == SYS_RESULT_OK) {
		SevenSegPortBind(&SevenSegConfig.hal);
		if(SevenSegDisplayInit(&SevenSegConfig) != SEVEN_SEG_RES_OK) {
			res = SYS_RESULT_ERROR;
		}
	}
	
	/* Initialize Time driver. */
	if(res == SYS_RESULT_OK) {
		res = TimeInit(evg_system, EVG_SYSTEM_FLAG_ALARM);
	}
	
	/* Initialize Pump driver. */
	if(res == SYS_RESULT_OK) {
		res = PumpInit(IComposerCallbackOnPumpStopped);
	}	
	
	/* Initialize Storage driver. */
	if(res == SYS_RESULT_OK) {
		res = StorageMount();
		if(res == SYS_RESULT_FAIL) { /* Storage still has to be formatted. */
			res = StorageFormat(FileSizes);
		}
	}
	

	/***** System tasks. *****/
	
	/* Initialize Irrigation Controller. */
	if(res == SYS_RESULT_OK) {
		res = IrrigationControllerInit(&mbox_irrigation);
	}
	
	/* Initialize Schedule Manager. */
	if(res == SYS_RESULT_OK) {
		ScheduleManagerConfig.mbox_irrigation = mbox_irrigation;
		ScheduleManagerConfig.evg_alarm = evg_system;
		res = ScheduleManagerInit(&ScheduleManagerConfig);
		mbox_schedule = ScheduleManagerConfig.mbox_schedule;
	}
	
	/* Initialize UI Controller. */
	if(res == SYS_RESULT_OK) {
		res = UiControllerInit(mbox_irrigation, mbox_schedule, evg_system);
	}


}

void ComposerCallbackSetOnPumpStopped(CallbackPumpStopped_t cb)
{
	OnPumpStopped = cb;
}


static void IComposerCallbackOnPumpStopped(void)
{
	if(OnPumpStopped != NULL) {
		OnPumpStopped();
	}
}
