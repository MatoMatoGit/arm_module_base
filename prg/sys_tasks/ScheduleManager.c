/*
 * ScheduleManager.c
 *
 *  Created on: 21 mei 2018
 *      Author: Dorus
 */

/* Self include. */
#include "ScheduleManager.h"

/* System tasks includes. */
#include "IrrigationController.h"

/* Driver includes. */
#include "Time/time.h"
#include "Storage/storage.h"

/* OS includes. */
#include "PriorRTOS.h"

/* Standard includes. */
#include <stdlib.h>
#include <string.h>

LOG_FILE_NAME("ScheduleManager");

#define MBOX_SCHEDULE_NUM_ADDR 2
#define HOURS_IN_DAY 24

typedef struct {
	ScheduleManagerConfig_t config;
	IrrigationData_t data;
}ScheduleManager_t;

ScheduleManager_t ScheduleManager;

static void TaskScheduleManager(const void *p_arg, U32_t v_arg);

static void IScheduleNextIrrigation(void);
static SysResult_t IScheduleStore(void);
static SysResult_t IScheduleLoad(void);
static SysResult_t ITimeStore(Time_t *t);
static SysResult_t ITimeLoad(Time_t *t);

SysResult_t ScheduleManagerInit(ScheduleManagerConfig_t *config)
{
	SysResult_t res = SYS_RESULT_OK;
	Id_t tsk_schedule_manager = ID_INVALID;

	if(config == NULL) {
		res = SYS_RESULT_INV_ARG;
	}

	if(res == SYS_RESULT_OK) {
		tsk_schedule_manager = TaskCreate(TaskScheduleManager, TASK_CAT_MEDIUM, 5,
			(TASK_PARAMETER_START | TASK_PARAMETER_ESSENTIAL), 0, NULL, 0);
		config->mbox_schedule = MailboxCreate(MBOX_SCHEDULE_NUM_ADDR, &tsk_schedule_manager, 1);
		if(tsk_schedule_manager == ID_INVALID || config->mbox_schedule == ID_INVALID) {
			res = SYS_RESULT_ERROR;
		}
	}

	/* Read irrigation amount and frequency from non-volatile storage. */
	if(res == SYS_RESULT_OK) {
		/* Initialize irrigation data. */
		memset(&ScheduleManager.data, 0, sizeof(ScheduleManager.data));

		res = IScheduleLoad();
		Time_t t;
		res = ITimeLoad(&t);
		TimeSet(&t);

		/* If the irrigation data is valid, acquire current time and
		 * set an alarm for the next irrigation. */
		if(ScheduleManager.data.irg_amount_l != 0 && ScheduleManager.data.irg_freq != 0
			&& ScheduleManager.data.irg_interval != 0) {
			IScheduleNextIrrigation();
		} else {
			TimeAlarmEnable(0);
		}
	}

	/* Copy the schedule manager config. */
	if(res == SYS_RESULT_OK) {
		ScheduleManager.config = *config;
	}

	return res;
}

SysResult_t ScheduleManagerIrrigationDataGet(IrrigationData_t *data)
{
	SysResult_t res = SYS_RESULT_INV_ARG;

	if(data != NULL) {
		*data = ScheduleManager.data;
		res = SYS_RESULT_OK;
	}

	return res;
}



static void TaskScheduleManager(const void *p_arg, U32_t v_arg)
{
	OsResult_t res = OS_RES_ERROR;
	U16_t amount = 0;
	U16_t freq = 0;

	TASK_INIT_BEGIN() {

	} TASK_INIT_END();

	res = MailboxPend(ScheduleManager.config.mbox_schedule, SCHEDULE_MBOX_ADDR_AMOUNT, &amount, OS_TIMEOUT_INFINITE);
	if(res == OS_RES_OK || res == OS_RES_EVENT) {
		/* If the received amount is not equal to the current amount and the amount is a valid value:
		 * Sync it with the schedule data and write the value to non-volatile storage. */
		if(ScheduleManager.data.irg_amount_l != amount && amount != 0) {
			ScheduleManager.data.irg_amount_l = amount;
			/* Write to storage. */
			res = IScheduleStore();
		}
	}

	res = MailboxPend(ScheduleManager.config.mbox_schedule, SCHEDULE_MBOX_ADDR_FREQ, &freq, OS_TIMEOUT_INFINITE);
	if(res == OS_RES_OK || res == OS_RES_EVENT) {
		/* If the received frequency is not equal to the current frequency and the frequency is a valid value:
		 * Sync it with the schedule data and write the value to non-volatile storage. */
		if(ScheduleManager.data.irg_freq != freq && freq != 0) {
			ScheduleManager.data.irg_freq = freq;
			ScheduleManager.data.irg_interval = HOURS_IN_DAY / freq;
			/* Write to storage. */
			res = IScheduleStore();
		}
	}

	res = EventgroupFlagsRequireSet(ScheduleManager.config.evg_alarm, ScheduleManager.config.evg_alarm_flag, OS_TIMEOUT_INFINITE);
	if(res == OS_RES_OK || res == OS_RES_EVENT) {
		res = MailboxPost(ScheduleManager.config.mbox_irrigation, IRRIGATION_MBOX_ADDR_TRIGGER, IRRIGATION_TRIGGER_SCHEDULE, OS_TIMEOUT_NONE);
		if(res == OS_RES_OK) {
			res = MailboxPost(ScheduleManager.config.mbox_irrigation, IRRIGATION_MBOX_ADDR_AMOUNT, ScheduleManager.data.irg_amount_l, OS_TIMEOUT_NONE);
		}
		if(res == OS_RES_OK) {
			EventgroupFlagsClear(ScheduleManager.config.evg_alarm, ScheduleManager.config.evg_alarm_flag);
			IScheduleNextIrrigation();
		}
	}
}


static void IScheduleNextIrrigation(void)
{
	Time_t t;
	TimeGet(&t);
	ScheduleManager.data.irg_time = t.hours + ScheduleManager.data.irg_interval;
	TimeAlarmSet(ScheduleManager.data.irg_time);
	TimeAlarmEnable(1);
	IScheduleStore();
	ITimeStore(&t);
}

static SysResult_t ITimeStore(Time_t *t)
{
	OsResult_t res = OS_RES_ERROR;

	res = StorageFileWrite(FILE_TIME, (void *)t, sizeof(Time_t));

	return res;
}

static SysResult_t ITimeLoad(Time_t *t)
{
	OsResult_t res = OS_RES_ERROR;

	/* Set the read offset to where the most recent copy of the time struct would be.
	 * Read from storage.*/
	uint32_t n = 0;
	res = StorageFileReadOffsetSet(FILE_TIME, sizeof(Time_t), OFFSET_OPT_RELATIVE_SUB);
	if(res == SYS_RESULT_OK) {
		do {
			res = StorageFileRead(FILE_TIME, (void *)t, sizeof(Time_t));
			if(res == SYS_RESULT_OK) {
				n++; /* Increment the number of successful reads. */
			}
		}
		while(res == SYS_RESULT_OK);
	}

	if(n > 0) {
		res = SYS_RESULT_OK;
	}

	return res;
}

static SysResult_t IScheduleStore(void)
{
	OsResult_t res = OS_RES_ERROR;

	res = StorageFileWrite(FILE_SCHEDULE, (void *)&ScheduleManager.data, sizeof(ScheduleManager.data));

	return res;
}

static SysResult_t IScheduleLoad(void)
{
	OsResult_t res = OS_RES_ERROR;

	/* Set the read offset to where the most recent copy of the ScheduleManager data would be.
	 * Read from storage.*/
	uint32_t n = 0;
	res = StorageFileReadOffsetSet(FILE_SCHEDULE, sizeof(ScheduleManager.data), OFFSET_OPT_RELATIVE_SUB);
	if(res == SYS_RESULT_OK) {
		do {
			res = StorageFileRead(FILE_SCHEDULE, (void *)&ScheduleManager.data, sizeof(ScheduleManager.data));
			if(res == SYS_RESULT_OK) {
				n++; /* Increment the number of successful reads. */
			}
		}
		while(res == SYS_RESULT_OK);
	}

	if(n > 0) {
		res = SYS_RESULT_OK;
	}

	return res;
}
