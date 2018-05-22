/*
 * pump.c
 *
 *  Created on: 16 apr. 2018
 *      Author: Dorus
 */


#include "pump.h"

#include "gpio.h"
#include <Timer.h>

static Id_t PumpTmr;

static U8_t Running;
static U8_t Enabled;

static CallbackPumpStopped_t OnPumpStopped;

#define PUMP_TIME_CONSTANT_ML (U32_t)(PUMP_CONFIG_ML_PER_MIN / 60) * 1e4 /* Time in microseconds to pump 1 mL*/

#define SEC_TO_USEC(sec) (sec * 1e6)

static void ITimerCallbackPump(Id_t timer_id, void *context);

SysResult_t PumpInit(CallbackPumpStopped_t on_stopped)
{
	SysResult_t res = SYS_RESULT_ERROR;

	if(on_stopped != NULL) {
		OnPumpStopped = on_stopped;
		Running = 0;
		Enabled = 0;
		PumpTmr = TimerCreate(1, (TIMER_PARAMETER_PERIODIC | TIMER_PARAMETER_AR), ITimerCallbackPump, NULL);
		if(PumpTmr != ID_INVALID) {
			GpioPumpInit();

#ifdef PUMP_CONFIG_CONTROL_INVERTED
		GpioPumpStateSet(1);
#else
		GpioPumpStateSet(0);
#endif
			res = SYS_RESULT_OK;
		}
	}

	return res;
}


void PumpEnable(U8_t val)
{
	if(Running && !val) {
		PumpStop();
	}

	Enabled = val;
}

SysResult_t PumpRunForDuration(U32_t duration_s)
{
	SysResult_t res = SYS_RESULT_ERROR;

	if(!Running && Enabled) {
		Running = 1;
		TimerIntervalSet(PumpTmr, SEC_TO_USEC(duration_s));
		TimerStart(PumpTmr);
#ifdef PUMP_CONFIG_CONTROL_INVERTED
		GpioPumpStateSet(0);
#else
		GpioPumpStateSet(1);
#endif
		res = SYS_RESULT_OK;
	}

	return res;
}

SysResult_t PumpRunForAmount(U32_t amount_ml)
{
	SysResult_t res = SYS_RESULT_ERROR;

	if(!Running && Enabled) {
		Running = 1;
		TimerIntervalSet(PumpTmr, (PUMP_TIME_CONSTANT_ML * amount_ml));
		TimerStart(PumpTmr);
#ifdef PUMP_CONFIG_CONTROL_INVERTED
		GpioPumpStateSet(0);
#else
		GpioPumpStateSet(1);
#endif
		res = SYS_RESULT_OK;
	}

	return res;
}

void PumpStop(void)
{
	Running = 0;
#ifdef PUMP_CONFIG_CONTROL_INVERTED
		GpioPumpStateSet(1);
#else
		GpioPumpStateSet(0);
#endif
}

U8_t PumpIsRunning(void)
{
	return Running;
}

static void ITimerCallbackPump(Id_t timer_id, void *context)
{
	PumpStop();
	OnPumpStopped();
}
