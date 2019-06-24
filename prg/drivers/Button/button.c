/*
 * button.c
 *
 *  Created on: 10 sep. 2018
 *      Author: Dorus
 */

#include "button.h"

#include "gpio.h"
#include "stm32f1xx_hal.h"

#include "Timer.h"

typedef struct {
	Button_t btn;
	uint8_t gpio_btn;
	ButtonCallback_t cbs[BUTTON_TRIGGER_NUM];
	uint32_t hold_th_ms; /* Hold threshold in ms. */
	Id_t hold_tmr;
}ButtonDesc_t;

ButtonDesc_t ButtonDescs[BUTTON_NUM] = {
	{ .gpio_btn = UI_BUTTON_INC },
	{ .gpio_btn = UI_BUTTON_DEC },
	{ .gpio_btn = UI_BUTTON_SEL },
};

void TimerCallbackHold(Id_t timer_id, void *context);
void ButtonCallbackInterrupt(uint8_t pin, uint8_t state);

static void ITriggerCallback(ButtonDesc_t *desc, ButtonTrigger_t trigger);
static ButtonDesc_t *IButtonDescFromGpioButton(uint8_t gpio_btn);

int ButtonInit(void)
{
	int res = BUTTON_RESULT_ERR;

	if(BUTTON_TRIGGER_NUM < UINT8_MAX) {
		res = BUTTON_RESULT_OK;

		/* Initialize the button descriptors. */
		for(uint32_t i = 0; i < BUTTON_NUM; i++) {
			ButtonDescs[i].btn = i;
			ButtonDescs[i].hold_th_ms = BUTTON_CONFIG_HOLD_THRESHOLD_MS_DEFAULT;
			ButtonDescs[i].hold_tmr = TimerCreate(ButtonDescs[i].hold_th_ms,
					(TIMER_PARAMETER_AR | TIMER_PARAMETER_PERIODIC), TimerCallbackHold, &ButtonDescs[i]);
			if(ButtonDescs[i].hold_tmr == ID_INVALID) {
				res = BUTTON_RESULT_ERR;
				break;
			}


			GpioUiButtonIntCallbackSet(ButtonDescs[i].gpio_btn, ButtonCallbackInterrupt);

			/* Set all callbacks to NULL. */
			for(uint8_t j = 0; j < BUTTON_TRIGGER_NUM; j++) {
				ButtonDescs[i].cbs[j] = NULL;
			}
		}

		if(res == BUTTON_RESULT_OK) {
			GpioUiButtonInit();
		}
	}

	return res;
}

void ButtonInterruptEnable(uint8_t en)
{
	GpioIntUiButtonEnable(en);
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
	OS_ARG_UNUSED(timer_id);

	ButtonDesc_t *button_desc = (ButtonDesc_t *)context;

	ITriggerCallback(button_desc, BUTTON_TRIGGER_HOLD);
}

void ButtonCallbackInterrupt(uint8_t pin, uint8_t state)
{
	ButtonDesc_t *desc = IButtonDescFromGpioButton(pin);

	/* Wait for the button state to stabilize. */
	//HAL_Delay(BUTTON_CONFIG_STABLE_THRESHOLD_MS);

	/* Previous state must be equal to the current state to
	 * be considered stable. */
	//if(state == GpioUiButtonStateGet(GpioPinGetButton(pin))) {
#if BUTTON_CONFIG_PRESSED_LOW_ACTIVE==1
		if(state) {
			TimerStop(desc->hold_tmr);
			ITriggerCallback(desc, BUTTON_TRIGGER_RELEASE);
			TimerReset(desc->hold_tmr);

		} else {
			ITriggerCallback(desc, BUTTON_TRIGGER_PRESS);
			TimerReset(desc->hold_tmr);
			TimerStart(desc->hold_tmr);
		}
#else
		if(state) {
			ITriggerCallback(desc, BUTTON_TRIGGER_PRESS);
			TimerReset(desc->hold_tmr);
			TimerStart(desc->hold_tmr);
		} else {
			TimerStop(desc->hold_tmr);
			ITriggerCallback(desc, BUTTON_TRIGGER_RELEASE);
			TimerReset(desc->hold_tmr);
		}
#endif
	//}
}

static void ITriggerCallback(ButtonDesc_t *desc, ButtonTrigger_t trigger)
{
	if(desc->cbs[trigger] != NULL) {
		desc->cbs[trigger](desc->btn, trigger);
	}
}

static ButtonDesc_t *IButtonDescFromGpioButton(uint8_t gpio_btn)
{
	ButtonDesc_t *desc = NULL;

	for(uint32_t i = 0; i < BUTTON_NUM; i++) {
		if(ButtonDescs[i].gpio_btn == gpio_btn) {
			desc = &ButtonDescs[i];
			break;
		}
	}

	return desc;
}
