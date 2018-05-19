/*
 * Time.h
 *
 *  Created on: 11 mei 2018
 *      Author: Dorus
 */

#ifndef TIME_H_
#define TIME_H_

#include "SystemResult.h"

#include "OsTypes.h"

SysResult_t TimeInit(Id_t mbox_time, U8_t hour_addr);

void TimeAlarmSet(U8_t hour);

void TimeAlarmEnable(U8_t val);

U32_t TimeUptimeGet(void);

#endif /* TIME_TIME_H_ */
