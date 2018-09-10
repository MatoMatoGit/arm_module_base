/*
 * button.c
 *
 *  Created on: 10 sep. 2018
 *      Author: Dorus
 */

#include "button.h"

#include "gpio.h"
#include "stm32f1xx_hal.h"


typedef uint32_t Id_t; /* TODO: REMOVE THIS. */

typedef struct {
	Button_t btn;
	ButtonCallback_t cbs[BUTTON_TRIGGER_NUM];
	uint32_t hold_th_ms; /* Hold threshold in ms. */
	Id_t hold_tmr;
}ButtonDesc_t;

ButtonDesc_t ButtonDescs[BUTTON_NUM];

void TimerCallbackHold(Id_t timer_id, void *context);
void ButtonCallbackInterrupt(uint8_t pin, uint8_t state);

static void ITriggerCallback(Button_t button, ButtonTrigger_t trigger);

int ButtonInit(void)
{
	int res = BUTTON_RESULT_ERR;

	if(BUTTON_TRIGGER_NUM < UINT8_MAX) {
		res = BUTTON_RESULT_OK;



		/* Initialize the button descriptors. */
		for(uint32_t i = 0; i < BUTTON_NUM; i++) {
			ButtonDescs[i].btn = i;
			ButtonDescs[i].hold_th_ms = BUTTON_CONFIG_HOLD_THRESHOLD_MS_DEFAULT;
//			ButtonDescs[i].hold_tmr = TimerCreate(ButtonDescs[i].hold_th_ms, (TIMER_PARAMETER_AR | TIMER_PARAMETER_PERIODIC), TimerCallbackHold, &ButtonDescs[i]);
//			if(ButtonDescs[i].hold_tmr == ID_INVALID) {
//				res = BUTTON_RESULT_ERR;
//			}

			//GpioUiButtonIntCallbackSet(i, ButtonCallbackInterrupt);

			/* Set all callbacks to NULL. */
			for(uint8_t j = 0; j < BUTTON_TRIGGER_NUM; j++) {
				ButtonDescs[i].cbs[j] = NULL;
			}
		}
	}

	return res;
}

int ButtonTriggerCallbackSet(Button_t button, ButtonTrigger_t trigger, ButtonCallback_t callback)
{
	int res = BUTTON_RESULT_OK;

	ButtonDescs[button].cbs[trigger] = callback;

	return res;
}
int ButtonTriggerHoldThresholdSet(Button_t button, uint32_t threshold_ms)
{
	ButtonDescs[button].hold_th_ms = threshold_ms;

	return BUTTON_RESULT_OK;
}

/***** Internal functions *****/

void TimerCallbackHold(Id_t timer_id, void *context)
{
	ButtonDesc_t *button_desc = (ButtonDesc_t *)context;

	ITriggerCallback(button_desc->btn, BUTTON_TRIGGER_HOLD);
}

void ButtonCallbackInterrupt(uint8_t pin, uint8_t state)
{
	/* Wait for the button state to stabilize. */
	HAL_Delay(BUTTON_CONFIG_STABLE_THRESHOLD_MS);

	/* Previous state must be equal to the current state to
	 * be considered stable. */
	if(state == GpioUiButtonStateGet(pin)) {
#if BUTTON_CONFIG_PRESSED_LOW_ACTIVE==1
		if(state) {
			ITriggerCallback(pin, BUTTON_TRIGGER_RELEASE);
			//TimerStop(hold_tmr);
			//TimerReset(hold_tmr);
		} else {
			ITriggerCallback(pin, BUTTON_TRIGGER_PRESS);
			//TimerReset(hold_tmr);
			//TimerStart(hold_tmr);
		}
#else
		if(state) {
			ITriggerCallback(pin, BUTTON_TRIGGER_PRESS);
			//TimerReset(hold_tmr);
			//TimerStart(hold_tmr);
		} else {
			ITriggerCallback(pin, BUTTON_TRIGGER_RELEASE);
			//TimerStop(hold_tmr);
			//TimerReset(hold_tmr);
		}
#endif
	}
}

static void ITriggerCallback(Button_t button, ButtonTrigger_t trigger)
{
	if(ButtonDescs[button].cbs[trigger] != NULL) {
		ButtonDescs[button].cbs[trigger](button, trigger);
	}
}
