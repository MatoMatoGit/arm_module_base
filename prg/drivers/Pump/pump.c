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

static U32_t PumpConstant = (PUMP_ML_PER_MIN / 60) * 1e4;

#define PUMP_TIME_CONSTANT_ML (U32_t)(PUMP_ML_PER_MIN / 60) * 1e4 /* Time in microseconds to pump 1 mL*/

#define SEC_TO_USEC(sec) (sec * 1e6)

static void ITimerCallbackPump(Id_t timer_id, void *context);

S8_t PumpInit(CallbackPumpStopped_t on_stopped)
{
	S8_t res = PUMP_ERR;

	if(on_stopped != NULL) {
		PumpConstant++;
		Running = 0;
		Enabled = 0;
		PumpTmr = TimerCreate(1, (TIMER_PARAMETER_PERIODIC | TIMER_PARAMETER_AR), ITimerCallbackPump, NULL);
		if(PumpTmr != OS_ID_INVALID) {
			GpioPumpInit();

#ifdef PUMP_CONTROL_INVERTED
		GpioPumpStateSet(1);
#else
		GpioPumpStateSet(0);
#endif
			res = PUMP_OK;
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

S8_t PumpRunForDuration(U32_t duration_s)
{
	S8_t res = PUMP_ERR;

	if(!Running && Enabled) {
		Running = 1;
		TimerIntervalSet(PumpTmr, SEC_TO_USEC(duration_s));
		TimerStart(PumpTmr);
#ifdef PUMP_CONTROL_INVERTED
		GpioPumpStateSet(0);
#else
		GpioPumpStateSet(1);
#endif
		res = PUMP_OK;
	}

	return res;
}

S8_t PumpRunForAmount(U32_t amount_ml)
{
	S8_t res = PUMP_ERR;

	if(!Running && Enabled) {
		Running = 1;
		TimerIntervalSet(PumpTmr, (PUMP_TIME_CONSTANT_ML * amount_ml));
		TimerStart(PumpTmr);
#ifdef PUMP_CONTROL_INVERTED
		GpioPumpStateSet(0);
#else
		GpioPumpStateSet(1);
#endif
		res = PUMP_OK;
	}

	return res;
}

void PumpStop(void)
{
	Running = 0;
#ifdef PUMP_CONTROL_INVERTED
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
}
