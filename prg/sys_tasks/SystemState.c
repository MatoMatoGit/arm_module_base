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

GFsm_t fsm;

int SystemStateInit(void)
{
	GFsmInit(&fsm, SystemStateSpec, FSM_STATE_NUM, SYS_STATE_INITIALIZATION);
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
