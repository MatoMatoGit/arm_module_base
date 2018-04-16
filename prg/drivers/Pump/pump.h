/*
 * pump.h
 *
 *  Created on: 16 apr. 2018
 *      Author: Dorus
 */

#ifndef PUMP_H_
#define PUMP_H_

#include <Types.h>

#define PUMP_ML_PER_MIN 600

#define PUMP_OK		0
#define PUMP_ERR	-1

#define PUMP_CONTROL_INVERTED

/* Called when the duration/amount was pumped or stopped manually. */
typedef void (*CallbackPumpStopped_t)(void);

S8_t PumpInit(CallbackPumpStopped_t on_stopped);

void PumpEnable(U8_t val);

S8_t PumpRunForDuration(U32_t duration_s);

S8_t PumpRunForAmount(U32_t amount_ml);

void PumpStop(void);

U8_t PumpIsRunning(void);

#endif /* PUMP_H_ */
