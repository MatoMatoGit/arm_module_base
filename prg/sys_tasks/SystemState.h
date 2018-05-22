/*
 * SystemState.h
 *
 *  Created on: 16 apr. 2018
 *      Author: Dorus
 */

#ifndef SYSTEM_STATE_H_
#define SYSTEM_STATE_H_

#include "GFsm.h"
#include "SystemResult.h"

#include <stdlib.h>

#define SYSTEM_CONFIG_STATUS_BLINK_FAST_INTERVAL_MS 100
#define SYSTEM_CONFIG_STATUS_BLINK_SLOW_INTERVAL_MS 800

#define SYSTEM_CONFIG_EVENTS_MAX 10

typedef void (*CallbackOnEvent_t)(Id_t object_id, U32_t event);

/* Initializes the FSM and System State task.
 * Returns SYSTEM_STATE_OK if successful.
 * */
SysResult_t SystemStateInit(void);

/* Attempt to transition the FSM to the new state.
 * The transition will take place when the System State
 * task is run.
 * Returns SYSTEM_STATE_OK if successful.
 * Returns SYSTEM_STATE_FAIL if a new state has
 * already been set.
 * Returns SYSTEM_STATE_ERR if the new state is
 * illegal in the current state.
 * */
SysResult_t SystemStateTransition(FsmState_t new_state);

SysResult_t SystemStateEventRegister(Id_t object_id, U32_t event, CallbackOnEvent_t on_event);

SysResult_t SystemStateEventUnregister(Id_t object_id, U32_t event);

/* Return the current system state. */
FsmState_t SystemStateGet(void);


/* State Callbacks. Each of these prototypes has a weak implementation. */

/* Initialization state. */
void StateInitOnEnter(FsmState_t prev_state, FsmState_t curr_state);
void StateInitOnExit(FsmState_t curr_state, FsmState_t next_state);
bool StateInitOnGuard(FsmState_t curr_state, FsmState_t next_state);
bool StateInitOnValidate(FsmState_t curr_state, FsmState_t next_state);

/* Idle state. */
void StateIdleOnEnter(FsmState_t prev_state, FsmState_t curr_state);
void StateIdleOnExit(FsmState_t curr_state, FsmState_t next_state);
bool StateIdleOnGuard(FsmState_t curr_state, FsmState_t next_state);
bool StateIdleOnValidate(FsmState_t curr_state, FsmState_t next_state);

/* Running state, */
void StateRunningOnEnter(FsmState_t prev_state, FsmState_t curr_state);
void StateRunningOnExit(FsmState_t curr_state, FsmState_t next_state);
bool StateRunningOnGuard(FsmState_t curr_state, FsmState_t next_state);
bool StateRunningOnValidate(FsmState_t curr_state, FsmState_t next_state);

/* Connected state. */
void StateConnectedOnEnter(FsmState_t prev_state, FsmState_t curr_state);
void StateConnectedOnExit(FsmState_t curr_state, FsmState_t next_state);
bool StateConnectedOnGuard(FsmState_t curr_state, FsmState_t next_state);
bool StateConnectedOnValidate(FsmState_t curr_state, FsmState_t next_state);

/* Transfer state. */
void StateTransferOnEnter(FsmState_t prev_state, FsmState_t curr_state);
void StateTransferOnExit(FsmState_t curr_state, FsmState_t next_state);
bool StateTransferOnGuard(FsmState_t curr_state, FsmState_t next_state);
bool StateTransferOnValidate(FsmState_t curr_state, FsmState_t next_state);

/* Pumping state. */
void StatePumpingOnEnter(FsmState_t prev_state, FsmState_t curr_state);
void StatePumpingOnExit(FsmState_t curr_state, FsmState_t next_state);
bool StatePumpingOnGuard(FsmState_t curr_state, FsmState_t next_state);
bool StatePumpingOnValidate(FsmState_t curr_state, FsmState_t next_state);

/* Error state. */
void StateErrorOnEnter(FsmState_t prev_state, FsmState_t curr_state);
void StateErrorOnExit(FsmState_t curr_state, FsmState_t next_state);
bool StateErrorOnGuard(FsmState_t curr_state, FsmState_t next_state);
bool StateErrorOnValidate(FsmState_t curr_state, FsmState_t next_state);

/* Critical Error state. */
void StateCritErrorOnEnter(FsmState_t prev_state, FsmState_t curr_state);
void StateCritErrorOnExit(FsmState_t curr_state, FsmState_t next_state);
bool StateCritErrorOnGuard(FsmState_t curr_state, FsmState_t next_state);
bool StateCritErrorOnValidate(FsmState_t curr_state, FsmState_t next_state);

#endif /* SYSTEM_STATE_H_ */
