/*
 * Composer.c
 *
 *  Created on: 1 aug. 2018
 *      Author: Dorus
 */
#include "Composer.h"

#include "PriorRTOS.h"

/* Drivers. */
#include "Storage/storage.h"
#include "Pump/pump.h"
#include "Time/time.h"
#include "RgbLed/rgb_led.h"

/* Tasks. */
#include "IrrigationController.h"
#include "ScheduleManager.h"
#include "UiController.h"

#define EVG_SYSTEM_FLAG_ALARM			0x00
#define EVG_SYSTEM_FLAG_PUMP_RUNNING	0x01
#define EVG_SYSTEM_FLAG_ERROR			0x02
#define EVG_SYSTEM_FLAG_ERROR_CRIT		0x03

#define FILE_SIZE_LOG		0x200
#define FILE_SIZE_SCHEDULE 	0x200

struct SevenSegDisplay SevenSegConfig = {
	.num_digits = UI_NUM_DIGITS,
	.max_value = UI_MAX_VALUE,
	.mode = SEVEN_SEG_MODE_DEC,
	.refresh_rate_hz = UI_REFRESH_RATE
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
		if(SevenSegInit(&SevenSegConfig) != SEVEN_SEG_RES_OK) {
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
		ScheduleManagerConfig_t config = {
			.mbox_schedule = ID_INVALID,
			.mbox_irrigation = mbox_irrigation,
			.evg_alarm = evg_system,
			.evg_alarm_flag = EVG_SYSTEM_FLAG_ALARM,
		};
		res = ScheduleManagerInit(&config);
		mbox_schedule = config.mbox_schedule;
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
