/*
 * Time.c
 *
 *  Created on: 11 mei 2018
 *      Author: Dorus
 */

#include "Time.h"

#include "Mailbox.h"

#include "rtc.h"
#include "stm32f1xx_hal.h"

#define SECONDS_IN_MINUTE	60
#define MINUTES_IN_HOUR		60
#define HOURS_IN_DAY		24

static void ITimeIncrement(void);

typedef struct {
	Id_t mbox;
	U8_t hour_addr;

	U8_t alarm_en;
	U8_t alarm_hour;

	U32_t cont_hours;
	U8_t hours;
	U8_t minutes;
	U8_t seconds;
}Time_t;

Time_t Time;

SysResult_t TimeInit(Id_t mbox_time, U8_t hour_addr)
{
	SysResult_t res = SYS_RESULT_ERROR;

	MX_RTC_Init();
	Time.mbox = mbox_time;
	Time.hour_addr = hour_addr;
	Time.alarm_en = 0;
	Time.alarm_hour = 0;
	Time.cont_hours = Time.hours = Time.minutes = Time.seconds = 0;

	return res;
}

void TimeAlarmSet(U8_t hour)
{
	Time.alarm_hour = hour;
}

void TimeAlarmEnable(U8_t val)
{
	Time.alarm_en = val;
}

U32_t TimeUptimeGet(void)
{
	return Time.cont_hours;
}

void HAL_RTCEx_RTCEventCallback(RTC_HandleTypeDef *hrtc)
{
	ITimeIncrement();
}

static void ITimeIncrement(void)
{
	Time.seconds++;

	if(Time.seconds >= SECONDS_IN_MINUTE) {
		Time.minutes++;
		Time.seconds = 0;
	}

	if(Time.minutes >= MINUTES_IN_HOUR) {
		Time.hours++;
		Time.cont_hours++;
		Time.minutes = 0;
		if(!Time.alarm_en || Time.hours == Time.alarm_hour) {
			MailboxPost(Time.mbox, Time.hour_addr, (MailboxBase_t)Time.hours, OS_TIMEOUT_NONE);
		}
	}

	if(Time.hours >= HOURS_IN_DAY) {
		Time.hours = 0;
	}
}
