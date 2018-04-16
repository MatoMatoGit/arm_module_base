/*
 * SystemState.h
 *
 *  Created on: 16 apr. 2018
 *      Author: Dorus
 */

#ifndef SYSTEM_STATE_H_
#define SYSTEM_STATE_H_

#include "SysTaskCommon.h"

#include <Types.h>

#define SYSTEM_STATE_N_TRIGGERS 5 /* Max. 254. */

struct SystemStateTrigger;
typedef struct SystemStateTrigger * SystemStateTrigger_t;

typedef enum {
	SYS_STATE_INITIALIZATION = 0, /* Must be 0. */
	SYS_STATE_IDLE,
	SYS_STATE_RUNNING,
	SYS_STATE_CONNECTED,
	SYS_STATE_PUMPING,
	SYS_STATE_INVALID /* Must be the last in the enum. */
}SystemState_t;

typedef struct {
	const SystemState_t state;

	const SystemState_t allowed_states[SYS_STATE_INVALID];
	const U8_t n_allowed_states;
}SystemStateSpec_t;

const SystemStateSpec_t SystemStateSpec[SYS_STATE_INVALID] = {
		{SYS_STATE_INITIALIZATION,
				{SYS_STATE_IDLE}, 1},
		{SYS_STATE_IDLE,
				{SYS_STATE_IDLE, SYS_STATE_RUNNING, SYS_STATE_CONNECTED, SYS_STATE_PUMPING}, 4},
		{SYS_STATE_RUNNING,
				{SYS_STATE_RUNNING, SYS_STATE_IDLE, SYS_STATE_CONNECTED, SYS_STATE_PUMPING}, 4},
		{SYS_STATE_CONNECTED,
				{SYS_STATE_IDLE, SYS_STATE_RUNNING}, 2},
		{SYS_STATE_PUMPING,
				{SYS_STATE_PUMPING, SYS_STATE_RUNNING}, 2}
};

Id_t TidSystemState;
void SysTaskSystemState(const void *p_args, U32_t v_arg);

SysTaskResult_t SysTaskSystemStateInit(void);

SysTaskResult_t SystemStateTrigger(SystemStateTrigger_t trig, U8_t data);

SystemStateTrigger_t SystemStateTriggerRegister(SystemState_t goto_state);

#endif /* SYSTEM_STATE_H_ */
