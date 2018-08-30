/*
 * pump.h
 *
 *  Created on: 16 apr. 2018
 *      Author: Dorus
 */

#ifndef PUMP_H_
#define PUMP_H_

#include "pump_config.h"

#include <OsTypes.h>
#include "SystemResult.h"

/* Called when the duration/amount was pumped or stopped manually. */
typedef void (*PumpCallback_t)(void);

SysResult_t PumpInit(PumpCallback_t on_stopped);

void PumpEnable(U8_t val);

SysResult_t PumpRunForDuration(U32_t duration_s);

SysResult_t PumpRunForAmount(U32_t amount_ml);

SysResult_t PumpRun(void);

void PumpStop(void);

U8_t PumpIsRunning(void);

#endif /* PUMP_H_ */
