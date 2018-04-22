/*
 * SystemState.c
 *
 *  Created on: 16 apr. 2018
 *      Author: Dorus
 */

#include "SystemState.h"


#include "GFsm.h"
#include "rgb_led.h"

FsmStateSpec_t SystemStateSpec[FSM_STATE_NUM] = {
	{
		.state = SYS_STATE_INITIALIZATION,
		.allowed_states = {
							SYS_STATE_IDLE
						},
		.n_allowed_states = 1,
		.on_validate = StateInitOnValidate,
		.on_guard = StateInitOnGuard,
		.on_enter = StateInitOnEnter,
		.on_exit = StateInitOnExit,
	},
	{
		.state = SYS_STATE_IDLE,
		.allowed_states = {
							SYS_STATE_IDLE,
							SYS_STATE_RUNNING,
							SYS_STATE_CONNECTED,
							SYS_STATE_PUMPING,
							SYS_STATE_ERROR,
							SYS_STATE_CRIT_ERROR
						},
		.n_allowed_states = 6,
		.on_validate = StateIdleOnValidate,
		.on_guard = StateIdleOnGuard,
		.on_enter = StateIdleOnEnter,
		.on_exit = StateIdleOnExit,
	},
	{
		.state = SYS_STATE_RUNNING,
		.allowed_states = {
							SYS_STATE_RUNNING,
							SYS_STATE_IDLE,
							SYS_STATE_PUMPING,
							SYS_STATE_ERROR,
							SYS_STATE_CRIT_ERROR
						},
		.n_allowed_states = 5,
		.on_validate = StateRunningOnValidate,
		.on_guard = StateRunningOnGuard,
		.on_enter = StateRunningOnEnter,
		.on_exit = StateRunningOnExit,
	},
	{
		.state = SYS_STATE_CONNECTED,
		.allowed_states = {
							SYS_STATE_IDLE,
							SYS_STATE_TRANSFER,
							SYS_STATE_ERROR,
							SYS_STATE_CRIT_ERROR
						},
		.n_allowed_states = 4,
		.on_validate = StateConnectedOnValidate,
		.on_guard = StateConnectedOnGuard,
		.on_enter = StateConnectedOnEnter,
		.on_exit = StateConnectedOnExit,
	},
	{
		.state = SYS_STATE_TRANSFER,
		.allowed_states = {
							SYS_STATE_CONNECTED,
							SYS_STATE_ERROR,
							SYS_STATE_CRIT_ERROR
						},
		.n_allowed_states = 3,
		.on_validate = StateTransferOnValidate,
		.on_guard = StateTransferOnGuard,
		.on_enter = StateTransferOnEnter,
		.on_exit = StateTransferOnExit,
	},
	{
		.state = SYS_STATE_PUMPING,
		.allowed_states = {
							SYS_STATE_RUNNING,
							SYS_STATE_ERROR,
							SYS_STATE_CRIT_ERROR
						},
		.n_allowed_states = 3,
		.on_validate = StatePumpingOnValidate,
		.on_guard = StatePumpingOnGuard,
		.on_enter = StatePumpingOnEnter,
		.on_exit = StatePumpingOnExit,
	},
	{
		.state = SYS_STATE_ERROR,
		.allowed_states = {
							SYS_STATE_IDLE,
							SYS_STATE_ERROR,
							SYS_STATE_CRIT_ERROR
						},
		.n_allowed_states = 2,
		.on_validate = StateErrorOnValidate,
		.on_guard = StateErrorOnGuard,
		.on_enter = StateErrorOnEnter,
		.on_exit = StateErrorOnExit,
	},
	{
		.state = SYS_STATE_CRIT_ERROR,
		.allowed_states = {
							SYS_STATE_CRIT_ERROR
						},
		.n_allowed_states = 1,
		.on_validate = StateCritErrorOnValidate,
		.on_guard = StateCritErrorOnGuard,
		.on_enter = StateCritErrorOnEnter,
		.on_exit = StateCritErrorOnExit,
	},
};

typedef enum {
	STATUS_LED_MODE_OFF,
	STATUS_LED_MODE_CONSTANT_ON,
	STATUS_LED_MODE_BLINK_SLOW,
	STATUS_LED_MODE_BLINK_FAST
}StatusLedMode_t;

typedef struct {
	FsmState_t state;
	RgbLedColor_t led_color;
	StatusLedMode_t led_mode;
}StatusLedMap_t;


StatusLedMap_t StatusLedMaps[FSM_STATE_NUM] = {
		{
			.state = SYS_STATE_INITIALIZATION,
			.led_color = RGB_LED_COLOR_WHITE,
			.led_mode = STATUS_LED_MODE_OFF,
		},
		{
			.state = SYS_STATE_IDLE,
			.led_color = RGB_LED_COLOR_GREEN,
			.led_mode = STATUS_LED_MODE_BLINK_SLOW,
		},
		{
			.state = SYS_STATE_CONNECTED,
			.led_color = RGB_LED_COLOR_BLUE,
			.led_mode = STATUS_LED_MODE_CONSTANT_ON,
		},
		{
			.state = SYS_STATE_TRANSFER,
			.led_color = RGB_LED_COLOR_BLUE,
			.led_mode = STATUS_LED_MODE_BLINK_FAST,
		},
		{
			.state = SYS_STATE_RUNNING,
			.led_color = RGB_LED_COLOR_GREEN,
			.led_mode = STATUS_LED_MODE_CONSTANT_ON,
		},
		{
			.state = SYS_STATE_PUMPING,
			.led_color = RGB_LED_COLOR_GREEN,
			.led_mode = STATUS_LED_MODE_BLINK_FAST,
		},
		{
			.state = SYS_STATE_ERROR,
			.led_color = RGB_LED_COLOR_RED,
			.led_mode = STATUS_LED_MODE_BLINK_SLOW,
		},
		{
			.state = SYS_STATE_CRIT_ERROR,
			.led_color = RGB_LED_COLOR_RED,
			.led_mode = STATUS_LED_MODE_BLINK_FAST,
		}
};

static GFsm_t Fsm;
static Id_t TskFsm;
static Id_t EvgFsm;

#define EVG_FLAG_TRANSITION 0x01

void SysTaskFsm(const void *p_args, U32_t v_arg);
static void IStatusLedSet(FsmState_t state);

int SystemStateInit(void)
{
	int res = GFsmInit(&Fsm, SystemStateSpec, FSM_STATE_NUM, SYS_STATE_INITIALIZATION);

	if(res == SYSTEM_STATE_OK) {
		res = SYSTEM_STATE_ERR;

		TskFsm = TaskCreate(SysTaskFsm, TASK_CAT_HIGH, 5, 0, 0, NULL);
		EvgFsm = EventgroupCreate();
		if(TskFsm != OS_ID_INVALID && EvgFsm != OS_ID_INVALID) {
			res = SYSTEM_STATE_OK;
		}
	}

	return res;
}

void SysTaskFsm(const void *p_args, U32_t v_arg)
{
	TASK_INIT_BEGIN() {
		RgbLedInit();
	}TASK_INIT_END();

	int fsm_res = GFSM_ERR;

	fsm_res = GFsmRun(&Fsm);

	if(fsm_res == GFSM_ERR) {
		LOG_ERROR_NEWLINE("FSM next state not set.");
		while(1);
	}

	IStatusLedSet(GFsmStateCurrentGet(&fsm));

	EventgroupRequireSet(EvgFsm, EVG_FLAG_TRANSITION, OS_TIMEOUT_INFINITE);
}

int SystemStateTransition(FsmState_t new_state)
{
	int res = SYSTEM_STATE_ERR;

	res = GFsmTransition(&Fsm, new_state);
	if(res == SYSTEM_STATE_OK) {
		EventgroupFlagSet(EvgFsm, EVG_FLAG_TRANSITION);
	} else if(res == SYSTEM_STATE_ERR) {
		LOG_ERROR_NEWLINE("Illegal state.");
		while(1);
	}

	return res;
}

/***** Internal functions. *****/

static void IStatusLedSet(FsmState_t state)
{
	StatusLedMap_t map = StatusLedMaps[(unsigned int)state];

	RgbLedColorSet(map.led_color);

	switch(map.led_mode) {
		default:
		case STATUS_LED_MODE_OFF: {
			RgbLedModeSet(RGB_LED_MODE_OFF);
			break;
		}
		case STATUS_LED_MODE_CONSTANT_ON: {
			RgbLedModeSet(RGB_LED_MODE_ON);
			break;
		}
		case STATUS_LED_MODE_BLINK_SLOW: {
			RgbLedBlinkIntervalSet(SYSTEM_STATUS_BLINK_SLOW_INTERVAL_MS);
			RgbLedModeSet(RGB_LED_MODE_BLINK);
			break;
		}
		case STATUS_LED_MODE_BLINK_FAST: {
			RgbLedBlinkIntervalSet(SYSTEM_STATUS_BLINK_FAST_INTERVAL_MS);
			RgbLedModeSet(RGB_LED_MODE_BLINK);
			break;
		}
	}
}

/***** Default state callbacks. *****/


/* Init state callbacks. */

void __attribute__((weak)) StateInitOnEnter(FsmState_t prev_state, FsmState_t curr_state)
{

}

void __attribute__((weak)) StateInitOnExit(FsmState_t curr_state, FsmState_t next_state)
{

}

bool __attribute__((weak)) StateInitOnGuard(FsmState_t curr_state, FsmState_t next_state)
{
	return true;
}

bool __attribute__((weak)) StateInitOnValidate(FsmState_t curr_state, FsmState_t next_state)
{
	return true;
}


/* Idle state callbacks. */

void __attribute__((weak)) StateIdleOnEnter(FsmState_t prev_state, FsmState_t curr_state)
{

}

void __attribute__((weak)) StateIdleOnExit(FsmState_t curr_state, FsmState_t next_state)
{

}

bool __attribute__((weak)) StateIdleOnGuard(FsmState_t curr_state, FsmState_t next_state)
{
	return true;
}

bool __attribute__((weak)) StateIdleOnValidate(FsmState_t curr_state, FsmState_t next_state)
{
	return true;
}


/* Running state callbacks. */

void __attribute__((weak)) StateRunningOnEnter(FsmState_t prev_state, FsmState_t curr_state)
{

}

void __attribute__((weak)) StateRunningOnExit(FsmState_t curr_state, FsmState_t next_state)
{

}

bool __attribute__((weak)) StateRunningOnGuard(FsmState_t curr_state, FsmState_t next_state)
{
	return true;
}

bool __attribute__((weak)) StateRunningOnValidate(FsmState_t curr_state, FsmState_t next_state)
{
	return true;
}


/* Connected state callbacks. */

void __attribute__((weak)) StateConnectedOnEnter(FsmState_t prev_state, FsmState_t curr_state)
{

}

void __attribute__((weak)) StateConnectedOnExit(FsmState_t curr_state, FsmState_t next_state)
{

}

bool __attribute__((weak)) StateConnectedOnGuard(FsmState_t curr_state, FsmState_t next_state)
{
	return true;
}

bool __attribute__((weak)) StateConnectedOnValidate(FsmState_t curr_state, FsmState_t next_state)
{
	return true;
}

/* Transfer state callbacks. */

void __attribute__((weak)) StateTransferOnEnter(FsmState_t prev_state, FsmState_t curr_state)
{

}

void __attribute__((weak)) StateTransferOnExit(FsmState_t curr_state, FsmState_t next_state)
{

}

bool __attribute__((weak)) StateTransferOnGuard(FsmState_t curr_state, FsmState_t next_state)
{
	return true;
}

bool __attribute__((weak)) StateTransferOnValidate(FsmState_t curr_state, FsmState_t next_state)
{
	return true;
}

/* Pumping state callbacks. */

void __attribute__((weak)) StatePumpingOnEnter(FsmState_t prev_state, FsmState_t curr_state)
{

}

void __attribute__((weak)) StatePumpingOnExit(FsmState_t curr_state, FsmState_t next_state)
{

}

bool __attribute__((weak)) StatePumpingOnGuard(FsmState_t curr_state, FsmState_t next_state)
{
	return true;
}

bool __attribute__((weak)) StatePumpingOnValidate(FsmState_t curr_state, FsmState_t next_state)
{
	return true;
}

/* Error state callbacks. */

void __attribute__((weak)) StateErrorOnEnter(FsmState_t prev_state, FsmState_t curr_state)
{

}

void __attribute__((weak)) StateErrorOnExit(FsmState_t curr_state, FsmState_t next_state)
{

}

bool __attribute__((weak)) StateErrorOnGuard(FsmState_t curr_state, FsmState_t next_state)
{
	return true;
}

bool __attribute__((weak)) StateErrorOnValidate(FsmState_t curr_state, FsmState_t next_state)
{
	return true;
}

/* Critical Error state callbacks. */

void __attribute__((weak)) StateCritErrorOnEnter(FsmState_t prev_state, FsmState_t curr_state)
{

}

void __attribute__((weak)) StateCritErrorOnExit(FsmState_t curr_state, FsmState_t next_state)
{

}

bool __attribute__((weak)) StateCritErrorOnGuard(FsmState_t curr_state, FsmState_t next_state)
{
	return true;
}

bool __attribute__((weak)) StateCritErrorOnValidate(FsmState_t curr_state, FsmState_t next_state)
{
	return true;
}
