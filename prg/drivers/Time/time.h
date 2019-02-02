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

/* 1 hour becomes 1 second, for debugging. */
#define HOUR_IS_SECOND 0

/* 1 hour becomes 1 minute, for debugging. */
#define HOUR_IS_MINUTE 1

typedef struct {
	U8_t hours;
	U8_t minutes;
	U8_t seconds;
} Time_t;

SysResult_t TimeInit(void);

/* When the alarm expires the evg_alarm_flag is set in evg_alarm. */
void TimeAlarmSet(U8_t hour);

void TimeAlarmEnable(U8_t val);

U32_t TimeUptimeHoursGet(void);

void TimeGet(Time_t *time);

void TimeSet(Time_t *time);

#endif /* TIME_H_ */
