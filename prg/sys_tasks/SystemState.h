/*
 * SystemState.h
 *
 *  Created on: 16 apr. 2018
 *      Author: Dorus
 */

#ifndef SYSTEM_STATE_H_
#define SYSTEM_STATE_H_

#include "GFsm.h"

#include <stdlib.h>

#define SYSTEM_STATUS_BLINK_FAST_INTERVAL_MS 100
#define SYSTEM_STATUS_BLINK_SLOW_INTERVAL_MS 800

#define SYSTEM_STATE_OK		GFSM_OK
#define SYSTEM_STATE_FAIL	GFSM_FAIL
#define SYSTEM_STATE_ERR	GFSM_OK

int SystemStateInit(void);
int SystemStateTransition(FsmState_t new_state);

/* State Callbacks. */

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
