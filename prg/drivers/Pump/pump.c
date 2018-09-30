/*
 * pump.c
 *
 *  Created on: 16 apr. 2018
 *      Author: Dorus
 */


#include "pump.h"

#include "gpio.h"
#include "Timer.h"

#include <stdlib.h>
#include <math.h>

static Id_t PumpTmr;

static volatile U8_t Running;
static volatile U8_t Enabled;

static PumpCallback_t OnPumpStopped = NULL;

#define SEC_PER_MIN		60
#define MSEC_PER_SEC	1e3
#define USEC_PER_SEC	1e6
#define MSEC_TO_USEC(msec) (msec * 1e3)
#define PUMP_MSEC_PER_ML (U32_t)ceil( 1 / (double)( (double)PUMP_CONFIG_ML_PER_MIN / (SEC_PER_MIN * MSEC_PER_SEC) ) ) /* Time in milliseconds to pump 1 mL*/
#define ML_TO_MSEC(ml) (PUMP_MSEC_PER_ML * ml)

static void ITimerCallbackPump(Id_t timer_id, void *context);

SysResult_t PumpInit(PumpCallback_t on_stopped)
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


void PumpEnable(U8_t en)
{
	if(en) {
		Enabled = 1;
	} else {
		Enabled = 0;
		if(Running) {
			PumpStop();
		}
	}
}

SysResult_t PumpRunForDuration(U32_t duration_s)
{
	SysResult_t res = SYS_RESULT_ERROR;

	if(!Running && Enabled) {
		Running = 1;
		TimerIntervalSet(PumpTmr, USEC_PER_SEC * duration_s);
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
		TimerIntervalSet(PumpTmr, MSEC_TO_USEC(ML_TO_MSEC(amount_ml)));
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

SysResult_t PumpRun(void)
{
	SysResult_t res = SYS_RESULT_ERROR;

	if(!Running && Enabled) {
		Running = 1;
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
	if(OnPumpStopped != NULL) {
		OnPumpStopped();
	}
}
