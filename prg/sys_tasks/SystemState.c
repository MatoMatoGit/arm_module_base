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
		.on_enter = IStateInitOnEnter,
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

	}TASK_INIT_END();

	GFsmRun(&Fsm);

	EventgroupRequireSet(EvgFsm, EVG_FLAG_TRANSITION, OS_TIMEOUT_INFINITE);
}

int SystemStateTransition(FsmState_t new_state)
{
	int res = SYSTEM_STATE_ERR;

	res = GFsmTransition(&Fsm, new_state);
	if(res == SYSTEM_STATE_ERR) {
		EventgroupFlagSet(EvgFsm, EVG_FLAG_TRANSITION);
	}

	return res;
}

static void IStateInitOnEnter(FsmState_t prev_state, FsmState_t new_state)
{

}

void StateInitOnExit(FsmState_t prev_state, FsmState_t new_state)
{

}

bool StateInitOnGuard(FsmState_t prev_state, FsmState_t new_state)
{
	return true;
}

bool StateInitOnValidate(FsmState_t prev_state, FsmState_t new_state)
{
	return true;
}

void StateIdleOnEnter(FsmState_t prev_state, FsmState_t new_state)
{

}

void StateIdleOnExit(FsmState_t prev_state, FsmState_t new_state)
{

}

bool Idle(FsmState_t prev_state, FsmState_t new_state)
{
	return true;
}

bool StateIdleOnValidate(FsmState_t prev_state, FsmState_t new_state)
{
	return true;
}
