/*
 * time.c
 *
 *  Created on: 11 mei 2018
 *      Author: Dorus
 */

#include "time.h"
#include "Mailbox.h"

#include "rtc.h"
#include "stm32f1xx_hal.h"

#define SECONDS_IN_MINUTE	60
#define MINUTES_IN_HOUR		60
#define HOURS_IN_DAY		24

static void ITimeIncrement(void);

typedef struct {
	Id_t mbox;

	U8_t alarm_en;
	U8_t alarm_hour;

	Time_t time;
	U32_t cont_hours;
}TimeData_t;

TimeData_t TimeData;

SysResult_t TimeInit(Id_t mbox_time, U8_t hour_addr)
{
	SysResult_t res = SYS_RESULT_ERROR;

	MX_RTC_Init();
	TimeData.mbox = mbox_time;
	TimeData.alarm_en = 0;
	TimeData.alarm_hour = 0;
	TimeData.cont_hours = TimeData.time.hours = TimeData.time.minutes = TimeData.time.seconds = 0;

	return res;
}

void TimeAlarmSet(U8_t hour)
{
	TimeData.alarm_hour = hour;
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

void HAL_RTCEx_RTCEventCallback(RTC_HandleTypeDef *hrtc)
{
	ITimeIncrement();
}

static void ITimeIncrement(void)
{
	TimeData.time.seconds++;

	if(TimeData.time.seconds >= SECONDS_IN_MINUTE) {
		TimeData.time.minutes++;
		TimeData.time.seconds = 0;
	}

	if(TimeData.time.minutes >= MINUTES_IN_HOUR) {
		TimeData.time.hours++;
		TimeData.cont_hours++;
		TimeData.time.minutes = 0;
		if(!TimeData.alarm_en || TimeData.time.hours == TimeData.alarm_hour) {
			MailboxPost(TimeData.mbox, TIME_MBOX_ADDR_HOUR, (MailboxBase_t)TimeData.time.hours, OS_TIMEOUT_NONE);
		}
	}

	if(TimeData.time.hours >= HOURS_IN_DAY) {
		TimeData.time.hours = 0;
	}
}
