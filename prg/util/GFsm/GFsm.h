#ifndef GFSM_H_
#define GFSM_H_

#include <stdbool.h>
#include <stdint.h>

#include "FsmState.h"

/* Generic Finite State Machine lib. */

#define GFSM_OK 0
#define GFSM_FAIL -1
#define GFSM_ERR -2



typedef void (*FsmCallbackOnStateEnter_t)(FsmState_t prev_state, FsmState_t curr_state);
typedef void (*FsmCallbackOnStateExit_t)(FsmState_t curr_state, FsmState_t next_state);
typedef bool (*FsmCallbackOnStateValidate_t)(FsmState_t curr_state, FsmState_t next_state);
typedef bool (*FsmCallbackOnStateGuard_t)(FsmState_t curr_state, FsmState_t next_state);

typedef struct {
	const FsmState_t state;

	FsmState_t allowed_states[FSM_STATE_NUM];
	uint8_t n_allowed_states;

	FsmCallbackOnStateValidate_t on_validate;
	FsmCallbackOnStateGuard_t on_guard;
	FsmCallbackOnStateEnter_t on_enter;
	FsmCallbackOnStateExit_t on_exit;


}FsmStateSpec_t;


struct GFsm {
	FsmStateSpec_t *SystemStateSpec;
	uint8_t n_states;

	FsmState_t prev_state;
	FsmState_t current_state;
	FsmState_t next_state;
};

typedef struct GFsm GFsm_t;

/* OK if successful, ERR if fsm = NULL, state_spec = NULL or n_states = 0. */
int GFsmInit(GFsm_t *fsm, FsmStateSpec_t *state_spec, uint8_t n_states, FsmState_t start_state);

/* ERR if the state transition is illegal.
 * FAIL if the next state has already been set. Run must be called.
 * OK if successful. */
int GFsmTransition(GFsm_t *fsm, FsmState_t new_state);

/* Run the FSM, transitions it to the next state if possible.
 * FAIL if the guard for the next state was not evaluated as true. */
int GFsmRun(GFsm_t *fsm);

FsmState_t GFsmStateCurrentGet(GFsm_t *fsm);

FsmState_t GFsmStateNextGet(GFsm_t *fsm);

FsmState_t GFsmStatePrevGet(GFsm_t *fsm);


#endif /* GFSM_H_ */
