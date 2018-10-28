/*
 * SystemEvg.h
 *
 *  Created on: 6 okt. 2018
 *      Author: Dorus
 */

#ifndef SYSTEM_EVG_H_
#define SYSTEM_EVG_H_

#include "include/OsTypes.h"

#define SYSTEM_FLAG_ALARM				0x01
#define SYSTEM_FLAG_PUMP_RUNNING		0x02
#define SYSTEM_FLAG_ERROR				0x04
#define SYSTEM_FLAG_ERROR_CRIT			0x08
#define SYSTEM_FLAG_SCHED_LOADED		0x10
#define SYSTEM_FLAG_PUMP_TRIGGER		0x20

OsResult_t SystemEvgInit(void);

Id_t SystemEvgGet(void);

#endif /* SYSTEM_EVG_H_ */
