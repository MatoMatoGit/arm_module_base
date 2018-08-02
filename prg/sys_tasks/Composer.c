/*
 * Composer.c
 *
 *  Created on: 1 aug. 2018
 *      Author: Dorus
 */
#include "Composer.h"

#include "PriorRTOS.h"

#include "IrrigationController.h"
#include "ScheduleManager.h"
#include "UiController.h"

#define EVG_SYSTEM_FLAG_ALARM			0x00
#define EVG_SYSTEM_FLAG_PUMP_RUNNING	0x01
#define EVG_SYSTEM_FLAG_ERROR			0x02
#define EVG_SYSTEM_FLAG_ERROR_CRIT		0x03

SysResult_t ComposerInit(void)
{
	SysResult_t res = SYS_RESULT_INV_ARG;

	res = SYS_RESULT_ERROR;

	if(TaskCreate(TaskComposer, TASK_CAT_HIGH, 5, TASK_PARAMETER_START, 0, NULL, 0) != ID_INVALID) {
		res = SYS_RESULT_OK;
	}

	return res;
}

static void TaskComposer(void *p_arg, U32_t v_arg)
{
	Id_t evg_system = ID_INVALID;
	Id_t mbox_irrigation = ID_INVALID;
	Id_t mbox_schedule = ID_INVALID;

	SysResult_t res = SYS_RESULT_OK;

	evg_system = EventgroupCreate();
	if(evg_system == ID_INVALID) {
		res = SYS_RESULT_ERROR;
	}

	if(res == SYS_RESULT_OK) {
		res = TimeInit(evg_system, EVG_SYSTEM_FLAG_ALARM);
	}

	if(res == SYS_RESULT_OK) {
		res = IrrigationControllerInit(*mbox_irrigation);
	}

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
	
	if(res == SYS_RESULT_OK) {
		res = UiControllerInit(mbox_irrigation, mbox_schedule, evg_system);
	}


}
