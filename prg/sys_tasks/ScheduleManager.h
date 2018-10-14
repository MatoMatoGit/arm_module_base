/*
 * ScheduleManager.h
 *
 *  Created on: 21 mei 2018
 *      Author: Dorus
 */

#ifndef SCHEDULE_MANAGER_H_
#define SCHEDULE_MANAGER_H_

#include "SystemResult.h"
#include "OsTypes.h"

#define SCHEDULE_MBOX_ADDR_AMOUNT	0x00
#define SCHEDULE_MBOX_ADDR_FREQ		0x01

typedef struct {
	Id_t mbox_schedule;

	Id_t mbox_irrigation;

	Id_t evg_sys;
}ScheduleManagerConfig_t;

typedef struct {
	U16_t irg_amount_l; /* Irrigation amount in Liters. */
	U16_t irg_freq;		/* Irrigation frequency (per day). */
	U8_t irg_interval;	/* Irrigation interval (hours). */
	U8_t irg_time;		/* Next moment of irrigation (hour of day). */
}IrrigationData_t;

SysResult_t ScheduleManagerInit(ScheduleManagerConfig_t *config);

SysResult_t ScheduleManagerIrrigationDataGet(IrrigationData_t *data);

#endif /* SCHEDULE_MANAGER_H_ */
