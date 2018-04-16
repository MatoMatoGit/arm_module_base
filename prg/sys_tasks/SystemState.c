/*
 * SystemState.c
 *
 *  Created on: 16 apr. 2018
 *      Author: Dorus
 */

#include "SystemState.h"

#include <PriorRTOS.h>

Id_t TidSystemState;
Id_t MboxTriggers;

struct SystemStateTrigger {
	U8_t mbox_address;
	SystemState_t goto_state;
};

struct SystemStateTrigger triggers[SYSTEM_STATE_N_TRIGGERS];

static SystemStateTrigger_t ITriggerGet(void);

void SysTaskSystemState(const void *p_args, U32_t v_arg)
{
	TASK_INIT_BEGIN() {

	} TASK_INIT_END();
}

SysTaskResult_t SysTaskSystemStateInit(void)
{
	SysTaskResult_t res = SYS_TASK_ERR;

	TidSystemState = TaskCreate(SysTaskSystemState, TASK_CAT_HIGH, 5, TASK_PARAM_ESSENTIAL, 0, NULL, 0);
	if(TidSystemState != OS_ID_INVALID) {
		MboxTriggers = MailboxCreate(SYSTEM_STATE_N_TRIGGERS, &TidSystemState, 1);

		for(U8_t i = 0; i < SYSTEM_STATE_N_TRIGGERS; i++) {
			triggers[i].goto_state = SYS_STATE_INVALID;
			triggers[i].mbox_address = 0;
		}
	}

	if(MboxTriggers != OS_ID_INVALID) {
		res = SYS_TASK_OK;
	}

	return res;
}

SysTaskResult_t SystemStateTrigger(SystemStateTrigger_t trig, U8_t data)
{
	SysTaskResult_t res = SYS_TASK_ERR;

	if(trig != NULL) {
		if(MailboxPost(MboxTriggers, trig->mbox_address, (MailboxBase_t)data, OS_TIMEOUT_NONE) == OS_RES_OK) {
			res = SYS_TASK_OK;
		}
	}

	return res;
}

SystemStateTrigger_t SystemStateTriggerRegister(SystemState_t goto_state)
{
	SystemStateTrigger_t trig = ITriggerGet();

	trig->goto_state = goto_state;

	return trig;
}

static SystemStateTrigger_t ITriggerGet(void)
{
	SystemStateTrigger_t trig = NULL;
	for(U8_t i = 0; i < SYSTEM_STATE_N_TRIGGERS; i++) {
		if(triggers[i].goto_state == SYS_STATE_INVALID) {
			triggers[i].goto_state = SYS_STATE_INITIALIZATION;
			triggers[i].mbox_address = i;
			trig = &triggers[i];
		}
	}

	return trig;
}
