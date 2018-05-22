/*
 * IrrigationController.c
 *
 *  Created on: 21 mei 2018
 *      Author: Dorus
 */

#include "IrrigationController.h"


#include <Task.h>
#include "pump.h"
#include "SystemState.h"

#include <stdlib.h>

#define PUMP_DURATION_HOUR 3600

U16_t AmountMl;
Id_t EvgTrigger;

static void ICallbackPumpStopped(void);
static void ICallbackOnEventTriggerButton(Id_t object_id, U32_t event);

SysResult_t IrrigationControllerInit(Id_t evg_trigger)
{
	SysResult_t res = SYS_RESULT_ERROR;

	res = PumpInit(ICallbackPumpStopped);

	if(res == SYS_RESULT_OK) {
		res = SystemStateEventRegister(EvgTrigger, EVENTGROUP_EVENT_FLAG_SET(IRRIGATION_CONTROLLER_TRIGGER_BUTTON), ICallbackOnEventTriggerButton);
	}

	return res;
}

SysResult_t IrrigationControllerAmountSet(U16_t amount_ml)
{
	SysResult_t res = SYS_RESULT_BUSY;

	if(!PumpIsRunning()) {
		AmountMl = amount_ml;
		res = SYS_RESULT_OK;
	}

	return res;
}

/***** Internal functions *****/

static void ICallbackOnEventTriggerButton(Id_t object_id, U32_t event)
{
	if(event == EVENTGROUP_EVENT_FLAG_SET(IRRIGATION_CONTROLLER_TRIGGER_BUTTON)) {
		SystemStateEventUnregister(EvgTrigger, EVENTGROUP_EVENT_FLAG_SET(IRRIGATION_CONTROLLER_TRIGGER_BUTTON));
		SystemStateEventRegister(EvgTrigger, EVENTGROUP_EVENT_FLAG_CLEAR(IRRIGATION_CONTROLLER_TRIGGER_BUTTON), ICallbackOnEventTriggerButton);
		SystemStateTransition(SYS_STATE_PUMPING);
	} else if(EVENTGROUP_EVENT_FLAG_CLEAR(IRRIGATION_CONTROLLER_TRIGGER_BUTTON)) {
		SystemStateEventUnregister(EvgTrigger, EVENTGROUP_EVENT_FLAG_CLEAR(IRRIGATION_CONTROLLER_TRIGGER_BUTTON));
		SystemStateEventRegister(EvgTrigger, EVENTGROUP_EVENT_FLAG_SET(IRRIGATION_CONTROLLER_TRIGGER_BUTTON), ICallbackOnEventTriggerButton);
		PumpStop();
	}
}

static void ICallbackPumpStopped(void)
{
	SystemStateTransition(SYS_STATE_RUNNING);
}

void StatePumpingOnEnter(FsmState_t prev_state, FsmState_t curr_state)
{
	PumpEnable(1);
	if(EventgroupFlagsGet(EvgTrigger, IRRIGATION_CONTROLLER_TRIGGER_BUTTON)) {
		PumpRunForDuration(PUMP_DURATION_HOUR);
	} else {
		PumpRunForAmount(AmountMl);
	}
}

void StatePumpingOnExit(FsmState_t curr_state, FsmState_t next_state)
{
	PumpEnable(0);
}

bool StatePumpingOnGuard(FsmState_t curr_state, FsmState_t next_state)
{
	return (PumpIsRunning() ? false : true);
}

bool StatePumpingOnValidate(FsmState_t curr_state, FsmState_t next_state)
{
	return true;
}
