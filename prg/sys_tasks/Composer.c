/*
 * Composer.c
 *
 *  Created on: 1 aug. 2018
 *      Author: Dorus
 */
#include "Composer.h"

#include "PriorRTOS.h"

/* System configuration. */
#include "sys_config.h"

#include "gpio.h"

/* Drivers. */
#include "button.h"
#include "storage.h"
#include "pump.h"
#include "time.h"
#include "rgb_led.h"
#include "seven_seg.h"
#include "seven_seg_port.h"

/* Tasks. */
#include "IrrigationController.h"
#include "UiController.h"
#include "ScheduleManager.h"
#include "SystemManager.h"
#include "SystemEvg.h"

#include <stdbool.h>

LOG_FILE_NAME("Composer");

#if COMPOSER_CONFIG_ENABLE_MODULE_SEVEN_SEG==1
struct SevenSegDisplay SevenSegConfig = {
	.num_digits = SEVEN_SEG_CONFIG_NUM_DIGITS,
	.max_value = SEVEN_SEG_CONFIG_MAX_VALUE,
	.min_value = SEVEN_SEG_CONFIG_MIN_VALUE,
	.mode = SEVEN_SEG_MODE_DEC,
	.refresh_rate_hz = SEVEN_SEG_CONFIG_REFRESH_RATE
};
#endif

#if COMPOSER_CONFIG_ENABLE_MODULE_SCHEDULE_MANAGER==1
ScheduleManagerConfig_t ScheduleManagerConfig = {
	.mbox_schedule = ID_INVALID,
	.mbox_irrigation = ID_INVALID,
	.evg_sys = ID_INVALID
};
#endif

#if COMPOSER_CONFIG_ENABLE_MODULE_STORAGE==1
const U32_t FileSizes[FILE_NUM] = {
	FILE_SIZE_SCHEDULE,
	FILE_SIZE_TIME,
};
#endif

static void ComposerTask(const void *p_arg, U32_t v_arg);

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

static void ComposerTask(const void *p_arg, U32_t v_arg)
{

	Id_t mbox_irrigation = ID_INVALID;
	Id_t mbox_schedule = ID_INVALID;

	SysResult_t res = SYS_RESULT_OK;
	
	/* Initialize System Eventgroup. */
	if(SystemEvgInit() != OS_RES_OK) {
		res = SYS_RESULT_ERROR;
	}

	/***** Drivers. *****/

#if COMPOSER_CONFIG_ENABLE_MODULE_RGB_LED==1
	/* Initialize RGB LED driver. */
	if(res == SYS_RESULT_OK) {
		if(RgbLedInit() != RGB_LED_OK) {
			res = SYS_RESULT_ERROR;
		} else {
			RgbLedColorSet(RGB_LED_COLOR_RED);
			RgbLedModeSet(RGB_LED_MODE_ON);
		}
	}
#endif
	
#if COMPOSER_CONFIG_ENABLE_MODULE_SEVEN_SEG==1
	/* Initialize Seven Segment display driver. */
	if(res == SYS_RESULT_OK) {
		SevenSegPortBind(&SevenSegConfig.hal);
		if(SevenSegDisplayInit(&SevenSegConfig) != SEVEN_SEG_RES_OK) {
			res = SYS_RESULT_ERROR;
		}
	}
#endif
	
#if COMPOSER_CONFIG_ENABLE_MODULE_TIME==1
	/* Initialize Time driver. */
	if(res == SYS_RESULT_OK) {
		res = TimeInit();
	}
#endif
	
#if COMPOSER_CONFIG_ENABLE_MODULE_PUMP==1
	/* Initialize Pump driver. */
	if(res == SYS_RESULT_OK) {
		res = PumpInit(IComposerCallbackOnPumpStopped);
	}
#endif
	
#if COMPOSER_CONFIG_ENABLE_MODULE_STORAGE==1
	/* Initialize Storage driver. */
	res = StorageMount();
	if(res == SYS_RESULT_FAIL) {
		LOG_DEBUG_NEWLINE("Formatting storage.");
		res = StorageFormat((uint32_t *)FileSizes);
		if(res == SYS_RESULT_OK) {
			LOG_DEBUG_NEWLINE("Storage mounted.");
			res = StorageMount();
		} else {
			LOG_ERROR_NEWLINE("Storage initialization error.");
		}
	} else if(res == SYS_RESULT_OK) {
		LOG_DEBUG_NEWLINE("Storage mounted.");
	} else {
		LOG_ERROR_NEWLINE("Storage initialization error.");
	}
#endif
	
#if COMPOSER_CONFIG_ENABLE_MODULE_BUTTON==1
	/* Initialize Button driver. */
	if(res == SYS_RESULT_OK) {
		if(ButtonInit() != BUTTON_RESULT_OK) {
			res = SYS_RESULT_ERROR;
		}
	}
#endif

	/***** System tasks. *****/
#if COMPOSER_CONFIG_ENABLE_MODULE_SYSTEM_MANAGER==1
	SystemManagerInit();
#endif

#if COMPOSER_CONFIG_ENABLE_MODULE_IRRIGATION_CONTROLLER==1
	/* Initialize Irrigation Controller. */
	if(res == SYS_RESULT_OK) {
		res = IrrigationControllerInit(&mbox_irrigation);
	}
#endif
	
#if COMPOSER_CONFIG_ENABLE_MODULE_SCHEDULE_MANAGER==1
	/* Initialize Schedule Manager. */
	if(res == SYS_RESULT_OK) {
		ScheduleManagerConfig.mbox_irrigation = mbox_irrigation;
		res = ScheduleManagerInit(&ScheduleManagerConfig);
		mbox_schedule = ScheduleManagerConfig.mbox_schedule;
	}
#endif
	
#if COMPOSER_CONFIG_ENABLE_MODULE_UI_CONTROLLER==1
	/* Initialize UI Controller. */
	if(res == SYS_RESULT_OK) {
		res = UiControllerInit(mbox_irrigation, mbox_schedule);
	}
#endif

	if(res != SYS_RESULT_OK) {
		GpioRgbLedInit();
		GpioRgbLedGreenStateSet(0);
		GpioRgbLedBlueStateSet(0);
		GpioRgbLedRedStateSet(1);
	}

	TaskDelete(NULL);
}

void ComposerCallbackSetOnPumpStopped(PumpCallback_t cb)
{
	OnPumpStopped = cb;
}


void IComposerCallbackOnPumpStopped(void)
{
	if(OnPumpStopped != NULL) {
		OnPumpStopped();
	}
}
