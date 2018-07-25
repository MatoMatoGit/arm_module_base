/*
 * GFsm.c
 *
 *  Created on: 19 apr. 2018
 *      Author: Dorus
 */


#include "GFsm.h"

#include <stdlib.h>
#include <stdint.h>

static int IStateTransitionIsAllowed(GFsm_t *fsm, FsmState_t new_state);
static void IFsmTransition(GFsm_t *fsm);

/* Run the FSM, transitions it to the next state if possible.
 * FAIL if the guard for the next state was not evaluated as true. */
static int IFsmRun(GFsm_t *fsm);

int GFsmInit(GFsm_t *fsm, FsmStateSpec_t *state_spec, uint8_t n_states, FsmState_t start_state)
{
	int res = GFSM_OK;

	if(fsm != NULL && state_spec != NULL && n_states != 0) {
		fsm->SystemStateSpec = state_spec;
		fsm->n_states = n_states;
		fsm->next_state = start_state; /* Set next state for first transition. */
		fsm->current_state = FSM_STATE_NUM;
		fsm->prev_state = FSM_STATE_NUM;
		IFsmTransition(fsm);
	}

	return res;
}

int GFsmTransition(GFsm_t *fsm, FsmState_t new_state)
{
	int res = GFSM_ERR;

	if(fsm != NULL && new_state != FSM_STATE_NUM) { /* Check inputs. */
		if(fsm->next_state == FSM_STATE_NUM) { /* Make sure next state has not been set. */
			if(IStateTransitionIsAllowed(fsm, new_state) == GFSM_OK) { /* Is the state transition allowed? */
				if(fsm->SystemStateSpec[(unsigned int)fsm->current_state].on_guard == NULL) { /* If there is no guard function -> go. */
					fsm->next_state = new_state;
					res = GFSM_OK;
				} else {
					if(fsm->SystemStateSpec[(unsigned int)fsm->current_state].on_guard(fsm->current_state, new_state)) { /* Guard passed. */
						fsm->next_state = new_state;
						res = GFSM_OK;
					}
				}
			}
		} else {
			res = GFSM_FAIL;
		}
	}

	return res;
}

FsmState_t GFsmStateCurrentGet(GFsm_t *fsm)
{
	FsmState_t state = FSM_STATE_NUM;

	if(fsm != NULL) {
		state = fsm->current_state;
	}

	return state;
}
FsmState_t GFsmStateNextGet(GFsm_t *fsm)
{
	FsmState_t state = FSM_STATE_NUM;

	if(fsm != NULL) {
		state = fsm->next_state;
	}

	return state;
}

FsmState_t GFsmStatePrevGet(GFsm_t *fsm)
{
	FsmState_t state = FSM_STATE_NUM;

	if(fsm != NULL) {
		state = fsm->prev_state;
	}

	return state;
}

/***** Internal functions. *****/

static void IFsmTransition(GFsm_t *fsm)
{
	fsm->prev_state = fsm->current_state;
	fsm->current_state = fsm->next_state;
	fsm->next_state = FSM_STATE_NUM;
	if(fsm->prev_state != FSM_STATE_NUM) {
		if(fsm->SystemStateSpec[(unsigned int)fsm->prev_state].on_exit != NULL) {
			fsm->SystemStateSpec[(unsigned int)fsm->prev_state].on_exit(fsm->prev_state, fsm->current_state);
		}
	}
	if(fsm->SystemStateSpec[(unsigned int)fsm->current_state].on_enter != NULL)
	{
		fsm->SystemStateSpec[(unsigned int)fsm->current_state].on_enter(fsm->prev_state, fsm->current_state);
	}
}

static int IStateTransitionIsAllowed(GFsm_t *fsm, FsmState_t new_state)
{
	int res = GFSM_ERR;

	FsmState_t *allowed_state =	fsm->SystemStateSpec[(unsigned int)fsm->current_state].allowed_states;
	uint8_t n_allowed_states = fsm->SystemStateSpec[(unsigned int)fsm->current_state].n_allowed_states;

	for(uint8_t i = 0; i < n_allowed_states; i++) {
		if(allowed_state[i] == new_state) {
			res = GFSM_OK;
			break;
		}
	}

	return res;
}

static int IFsmRun(GFsm_t *fsm)
{
	int res = GFSM_ERR;

	if(fsm->next_state != FSM_STATE_NUM)
	{
		IFsmTransition(fsm);
		res = GFSM_OK;
	}

	return res;
}



