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
#include "SystemManager.h"

/* Driver includes. */
#include "Time/time.h"
#include "Storage/storage.h"

/* OS includes. */
#include "PriorRTOS.h"

/* Standard includes. */
#include <stdlib.h>
#include <string.h>
#include "SystemEvg.h"

LOG_FILE_NAME("ScheduleManager");

#define MBOX_SCHEDULE_NUM_ADDR 2
#define HOURS_IN_DAY 24

typedef struct {
	ScheduleManagerConfig_t config;
	IrrigationData_t data;
}ScheduleManager_t;

ScheduleManager_t ScheduleManager;

static void ScheduleManagerTask(const void *p_arg, U32_t v_arg);

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
		tsk_schedule_manager = TaskCreate(ScheduleManagerTask, TASK_CAT_MEDIUM, 5,
			(TASK_PARAMETER_START | TASK_PARAMETER_ESSENTIAL), 0, NULL, 0);
		config->mbox_schedule = MailboxCreate(MBOX_SCHEDULE_NUM_ADDR, &tsk_schedule_manager, 1);
		if(tsk_schedule_manager == ID_INVALID || config->mbox_schedule == ID_INVALID) {
			LOG_ERROR_NEWLINE("Failed to create the ScheduleManager task and/or the Schedule mailbox.");
			res = SYS_RESULT_ERROR;
		}
	}

	/* Read irrigation amount and frequency from non-volatile storage. */
	if(res == SYS_RESULT_OK) {
		LOG_DEBUG_NEWLINE("Created ScheduleManager task and the Schedule mailbox.");

		/* Initialize irrigation data. */
		memset(&ScheduleManager.data, 0, sizeof(ScheduleManager.data));

		res = IScheduleLoad();
		if(res == SYS_RESULT_FAIL) {
			res = SYS_RESULT_OK;
			LOG_DEBUG_NEWLINE("No schedule available.");
		} else if(res != SYS_RESULT_OK) {
			LOG_ERROR_NEWLINE("Failed to load schedule.");
		}

		Time_t t;
		res = ITimeLoad(&t);
		if(res == SYS_RESULT_FAIL) {
			res = SYS_RESULT_OK;
			LOG_DEBUG_NEWLINE("No time available.");
		} else if(res != SYS_RESULT_OK) {
			LOG_ERROR_NEWLINE("Failed to load time.");
		} else {
			TimeSet(&t);
		}

		LOG_DEBUG_NEWLINE("\nCurrent schedule:\nAmount [L]:\t%u\nFreq [/day]:\t%u\nIval [H]:\t\t%u\nTime [H]:\t%u",
				ScheduleManager.data.irg_amount_l, ScheduleManager.data.irg_freq,
				ScheduleManager.data.irg_interval, ScheduleManager.data.irg_time);

		/* If the irrigation data is valid, acquire current time and
		 * set an alarm for the next irrigation.
		 * Otherwise make sure the alarm is disabled. */
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
		ScheduleManager.config.evg_sys = SystemEvgGet();
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



static void ScheduleManagerTask(const void *p_arg, U32_t v_arg)
{
	OsResult_t res = OS_RES_ERROR;
	U16_t amount = 0;
	U16_t freq = 0;
	static U8_t sched_synced = 0;

	//LOG_DEBUG_NEWLINE("ScheduleManagerTask running.");

	TASK_INIT_BEGIN() {
		LOG_DEBUG_NEWLINE("Synchronized current schedule values.");
		EventgroupFlagsSet(ScheduleManager.config.evg_sys, SYSTEM_FLAG_SCHED_LOADED);
	} TASK_INIT_END();

	if(sched_synced == 0) {
		res = EventgroupFlagsRequireCleared(ScheduleManager.config.evg_sys, SYSTEM_FLAG_SCHED_LOADED, OS_TIMEOUT_INFINITE);
		if(res == OS_RES_OK || res == OS_RES_EVENT) {
			LOG_DEBUG_NEWLINE("Schedule synchronized.");
			sched_synced = 1;
		}
	}

	if(sched_synced == 1) {
		res = MailboxPend(ScheduleManager.config.mbox_schedule, SCHEDULE_MBOX_ADDR_AMOUNT, &amount, OS_TIMEOUT_INFINITE);
		if(res == OS_RES_OK || res == OS_RES_EVENT) {
			LOG_DEBUG_NEWLINE("Schedule mailbox received %u Liters.",amount);
			/* If the received amount is not equal to the current amount and the amount is a valid value:
			 * Sync it with the schedule data and write the value to non-volatile storage. */
			if(ScheduleManager.data.irg_amount_l != amount && amount != 0) {
				LOG_DEBUG_NEWLINE("Value is different, storing new value.");
				ScheduleManager.data.irg_amount_l = amount;
				/* Write to storage. */
				if(IScheduleStore() != SYS_RESULT_OK) {
					LOG_ERROR_NEWLINE("Schedule store error.");
					SystemRaiseError(SYSTEM_COMP_APP_SCHEDULE_MANAGER, SYSTEM_ERROR, ERROR_SCHEDULE_STORE);
				}
			}
		}

		res = MailboxPend(ScheduleManager.config.mbox_schedule, SCHEDULE_MBOX_ADDR_FREQ, &freq, OS_TIMEOUT_INFINITE);
		if(res == OS_RES_OK || res == OS_RES_EVENT) {
			LOG_DEBUG_NEWLINE("Schedule mailbox received %u Times per day.", freq);
			/* If the received frequency is not equal to the current frequency and the frequency is a valid value:
			 * Sync it with the schedule data and write the value to non-volatile storage. */
			if(ScheduleManager.data.irg_freq != freq && freq != 0) {
				LOG_DEBUG_NEWLINE("Value is different, storing new value.");
				ScheduleManager.data.irg_freq = freq;
				ScheduleManager.data.irg_interval = HOURS_IN_DAY / freq;
				LOG_DEBUG_NEWLINE("New interval is %u hours.", ScheduleManager.data.irg_interval);
				/* Write to storage. */
				IScheduleNextIrrigation();
			}
		}

		res = EventgroupFlagsRequireSet(ScheduleManager.config.evg_sys, SYSTEM_FLAG_ALARM, OS_TIMEOUT_INFINITE);
		if(res == OS_RES_OK || res == OS_RES_EVENT) {
			LOG_DEBUG_NEWLINE("Received Alarm event.");
			LOG_DEBUG_NEWLINE("Posting amount (%u Liters) in Irrigation mailbox.", ScheduleManager.data.irg_amount_l);
			res = MailboxPost(ScheduleManager.config.mbox_irrigation, IRRIGATION_MBOX_ADDR_AMOUNT, ScheduleManager.data.irg_amount_l, OS_TIMEOUT_NONE);

			if(res == OS_RES_OK) {
				LOG_DEBUG_NEWLINE("Posting scheduled trigger in Irrigation mailbox.", ScheduleManager.data.irg_amount_l);
				res = MailboxPost(ScheduleManager.config.mbox_irrigation, IRRIGATION_MBOX_ADDR_TRIGGER, IRRIGATION_TRIGGER_SCHEDULE, OS_TIMEOUT_NONE);
			}
			if(res == OS_RES_OK) {
				EventgroupFlagsClear(ScheduleManager.config.evg_sys, SYSTEM_FLAG_ALARM);
				IScheduleNextIrrigation();
			}
		}
	}

	TaskSleep(1000);
}


static void IScheduleNextIrrigation(void)
{

	Time_t t;
	TimeGet(&t);
	ScheduleManager.data.irg_time = t.hours + ScheduleManager.data.irg_interval;
	if(ScheduleManager.data.irg_time > HOURS_IN_DAY) {
		ScheduleManager.data.irg_time -= HOURS_IN_DAY;
	}
	LOG_DEBUG_NEWLINE("Scheduling next irrigation hour: %u.", ScheduleManager.data.irg_time);
	TimeAlarmSet(ScheduleManager.data.irg_time);
	TimeAlarmEnable(1);
	if(IScheduleStore() != SYS_RESULT_OK) {
		LOG_ERROR_NEWLINE("Schedule store error.");
	}
	if(ITimeStore(&t) != SYS_RESULT_OK) {
		LOG_ERROR_NEWLINE("Time store error.");
		SystemRaiseError(SYSTEM_COMP_APP_SCHEDULE_MANAGER, SYSTEM_ERROR, ERROR_TIME_STORE);
	}

}

static SysResult_t ITimeStore(Time_t *t)
{
	SysResult_t res = SYS_RESULT_OK;

	LOG_DEBUG_NEWLINE("Storing time.");
#ifndef SCHEDULE_MANAGER_DEBUG_DISABLE_STORAGE_WRITE
	res = StorageFileWrite(FILE_TIME, (void *)t, sizeof(Time_t));
#endif
	return res;
}

static SysResult_t ITimeLoad(Time_t *t)
{
	SysResult_t res = SYS_RESULT_ERROR;

	LOG_DEBUG_NEWLINE("Loading time.");
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
	} else {
		LOG_ERROR_NEWLINE("Storage read error.");
	}

	return res;
}

static SysResult_t IScheduleStore(void)
{
	SysResult_t res = SYS_RESULT_OK;

	LOG_DEBUG_NEWLINE("Storing schedule.");
#ifndef SCHEDULE_MANAGER_DEBUG_DISABLE_STORAGE_WRITE
	res = StorageFileWrite(FILE_SCHEDULE, (void *)&ScheduleManager.data, sizeof(ScheduleManager.data));
#endif

	return res;
}

static SysResult_t IScheduleLoad(void)
{
	SysResult_t res = SYS_RESULT_ERROR;

	LOG_DEBUG_NEWLINE("Loading schedule.");
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
	} else {
		LOG_ERROR_NEWLINE("Storage read error.");
	}

	return res;
}
