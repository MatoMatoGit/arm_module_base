/*
 * time.c
 *
 *  Created on: 11 mei 2018
 *      Author: Dorus
 */

#include "time.h"
#include "Eventgroup.h"
#include "Logger.h"
#include "SystemEvg.h"

#include "rtc.h"
#include "stm32f1xx_hal.h"


LOG_FILE_NAME("Time");

#define SECONDS_IN_MINUTE	60
#define MINUTES_IN_HOUR		60
#define HOURS_IN_DAY		24

static void ITimeIncrement(uint8_t sec);

typedef struct {
	Id_t evg;
	U8_t evg_alarm_flag;

	U8_t alarm_en;
	U8_t alarm_hour;

	Time_t time;
	U32_t cont_hours;
}TimeData_t;

TimeData_t TimeData;

SysResult_t TimeInit(void)
{
	SysResult_t res = SYS_RESULT_ERROR;

	TimeData.evg = SystemEvgGet();
	TimeData.evg_alarm_flag = SYSTEM_FLAG_ALARM;
	TimeData.alarm_en = 0;
	TimeData.alarm_hour = 0;
	TimeData.cont_hours = TimeData.time.hours = TimeData.time.minutes = TimeData.time.seconds = 0;
	RtcCallbackSetOnSecond(ITimeIncrement);
	RtcInit();

	return res;
}

void TimeAlarmSet(U8_t hour)
{
	TimeData.alarm_hour = hour;
	LOG_DEBUG_NEWLINE("Alarm set [H]:%u", TimeData.alarm_hour);
}

void TimeAlarmEnable(U8_t val)
{
	TimeData.alarm_en = val;
}

U32_t TimeUptimeHoursGet(void)
{
	return TimeData.cont_hours;
}

void TimeGet(Time_t *time)
{
	*time = TimeData.time;
}

void TimeSet(Time_t *time)
{
	TimeData.time = *time;
}

static void ITimeIncrement(uint8_t sec)
{
#if HOUR_IS_SECOND==1
	TimeData.time.minutes = MINUTES_IN_HOUR;
#elif HOUR_IS_MINUTE==1
	TimeData.time.seconds = SECONDS_IN_MINUTE;
#else
	TimeData.time.seconds++;
#endif

	if(TimeData.time.seconds >= SECONDS_IN_MINUTE) {
		TimeData.time.minutes++;
		TimeData.time.seconds = 0;
	}

	if(TimeData.time.minutes >= MINUTES_IN_HOUR) {
		TimeData.time.hours++;
		TimeData.cont_hours++;
		TimeData.time.minutes = 0;
		if(TimeData.alarm_en && TimeData.time.hours == TimeData.alarm_hour) {
			LOG_DEBUG_NEWLINE("Alarm [H]:%u", TimeData.alarm_hour);
			EventgroupFlagsSet(TimeData.evg, TimeData.evg_alarm_flag);
		}
	}

	if(TimeData.time.hours >= HOURS_IN_DAY) {
		TimeData.time.hours = 0;
	}
}
