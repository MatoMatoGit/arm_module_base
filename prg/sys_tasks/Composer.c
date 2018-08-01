/*
 * Composer.c
 *
 *  Created on: 1 aug. 2018
 *      Author: Dorus
 */
#include "SystemResult.h"

#include "PriorRTOS.h"

#include "IrrigationController.h"
#include "ScheduleManager.h"
#include "UiController.h"

#define EVG_SYSTEM_FLAG_ALARM			0x00
#define EVG_SYSTEM_FLAG_PUMP_RUNNING	0x01
#define EVG_SYSTEM_FLAG_ERROR			0x02
#define EVG_SYSTEM_FLAG_ERROR_CRIT		0x03

static void TaskComposer(void *p_arg, U32_t v_arg)
{
	Id_t evg_system = EventgroupCreate();
	Id_t mbox_irrigation = MailboxCreate(2, NULL, 1);
	Id_t mbox_schedule = MailboxCreate(2, NULL, 1);

	SysResult_t res = SYS_RESULT_ERROR;

	res = TimeInit(evg_system, EVG_SYSTEM_FLAG_ALARM);

	res = IrrigationControllerInit(mbox_irrigation);

	ScheduleManagerConfig_t config = {
		.mbox_schedule = mbox_schedule,
		.mbox_irrigation = mbox_irrigation,
		.evg_alarm = evg_system,
		.evg_alarm_flag = EVG_SYSTEM_FLAG_ALARM,
	};
	res = ScheduleManagerInit(&config);

	res = UiControllerInit(mbox_irrigation, mbox_schedule, evg_system);

}
