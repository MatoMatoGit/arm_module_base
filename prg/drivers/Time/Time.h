/*
 * time.h
 *
 *  Created on: 11 mei 2018
 *      Author: Dorus
 */

#ifndef TIME_H_
#define TIME_H_

#include "SystemResult.h"

#include "OsTypes.h"

typedef struct {
	U8_t hours;
	U8_t minutes;
	U8_t seconds;
} Time_t;

/* evg_alarm must be an eventgroup. */
SysResult_t TimeInit(Id_t evg_alarm, U8_t evg_alarm_flag);

/* When the alarm expires the evg_alarm_flag is set in evg_alarm. */
void TimeAlarmSet(U8_t hour);

void TimeAlarmEnable(U8_t val);

U32_t TimeUptimeHoursGet(void);

void TimeGet(Time_t *time);

void TimeSet(Time_t *time);

#endif /* TIME_H_ */
