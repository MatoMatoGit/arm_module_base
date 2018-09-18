/*
 * UiController.c
 *
 *  Created on: 21 mei 2018
 *      Author: Dorus
 */

/* Self include. */
#include "UiController.h"
#include "ui_Config.h"

/* System task includes. */
#include "ScheduleManager.h"
#include "IrrigationController.h"

/* Driver includes. */
#include "RgbLed/rgb_led.h"
#include "SevenSegDisplay/seven_seg.h"

/* OS includes. */
#include "PriorRTOS.h"

/* Standard includes. */
#include <stdlib.h>
#include <string.h>

LOG_FILE_NAME("UiController");

static void UiControllerTask(void *p_arg, U32_t v_arg);

static SysResult_t IValueDecrement(U8_t index, S32_t val);
static SysResult_t IValueIncrement(U8_t index, S32_t val);
static U8_t IMapIndexToAddress(U8_t index);
static void IDisplayUpdate(void);

static void ITimerCallbackValueStable(Id_t timer_id, void *context);
static void ITimerCallbackUiInactive(Id_t timer_id, void *context);

static void IButtonCallbackIncrement(Button_t button, ButtonTrigger_t trigger);
static void IButtonCallbackDecrement(Button_t button, ButtonTrigger_t trigger);
static void IButtonCallbackSelect(Button_t button, ButtonTrigger_t trigger);
static void IButtonCallbackPumpOn(Button_t button, ButtonTrigger_t trigger);
static void IButtonCallbackPumpOff(Button_t button, ButtonTrigger_t trigger);
static void IButtonCallbackUiActivate(Button_t button, ButtonTrigger_t trigger);

static volatile ButtonCallback_t ButtonCallbackSelectRelease;

static volatile U8_t SelectedValue = 0;
static Id_t TmrValueStable = ID_INVALID;
static Id_t TmrUiInactive = ID_INVALID;
static Id_t MboxIrrigation = ID_INVALID;
static Id_t MboxSchedule = ID_INVALID;
static Id_t EvgSystem = ID_INVALID;


SysResult_t UiControllerInit(Id_t mbox_irrigation, Id_t mbox_schedule, Id_t evg_system)
{
	SysResult_t res = SYS_RESULT_OK;
	Id_t tsk_irrigation_controller = ID_INVALID;

	tsk_irrigation_controller = TaskCreate(UiControllerTask, TASK_CAT_MEDIUM, 1,
		(TASK_PARAMETER_START | TASK_PARAMETER_ESSENTIAL), 0, NULL, 0);
	TmrValueStable = TimerCreate(UI_THRESHOLD_VALUE_STABLE_MS, TIMER_PARAMETER_PERIODIC, ITimerCallbackValueStable, NULL);
	TmrUiInactive = TimerCreate(UI_THRESHOLD_INACTIVE_MS, TIMER_PARAMETER_PERIODIC, ITimerCallbackUiInactive, NULL);
	if(tsk_irrigation_controller == ID_INVALID || TmrValueStable == ID_INVALID || TmrUiInactive == ID_INVALID) {
		MboxIrrigation = mbox_irrigation;
		MboxSchedule = mbox_schedule;
		EvgSystem = evg_system;
		res = SYS_RESULT_ERROR;
	}

	if(res == SYS_RESULT_OK) {
		/* Configure the select button as wake-up trigger. */
		ButtonTriggerCallbackSet(BUTTON_UI_SEL, BUTTON_TRIGGER_RELEASE, IButtonCallbackUiActivate);
		ButtonCallbackSelectRelease = IButtonCallbackUiActivate;

		/* Pump Test functionality can also be used when the UI is disabled. */
		ButtonTriggerCallbackSet(BUTTON_UI_SEL, BUTTON_TRIGGER_HOLD, IButtonCallbackPumpOn);
		ButtonTriggerHoldThresholdSet(BUTTON_UI_SEL, UI_THRESHOLD_PUMP_ON_MS);

		SevenSegDisplayEnable(0); /* Turn display off. */
		IDisplayUpdate(); /* Make sure display will show a valid value when the UI is activated. */
	}

	return res;
}


static void UiControllerTask(void *p_arg, U32_t v_arg)
{
	OsResult_t res = OS_RES_ERROR;

	TASK_INIT_BEGIN() {
	} TASK_INIT_END();

}


static SysResult_t IValueIncrement(U8_t index, S32_t val)
{
	SysResult_t res = SYS_RESULT_OK;

	if(UiValues[index].current_val + val <= UiValues[index].max_val) {
		UiValues[index].current_val += val;
	} else {
		UiValues[index].current_val = UiValues[index].max_val;
	}

	IDisplayUpdate();

	return res;
}

static SysResult_t IValueDecrement(U8_t index, S32_t val)
{
	SysResult_t res = SYS_RESULT_OK;

	if(UiValues[index].current_val - val >= UiValues[index].min_val) {
		UiValues[index].current_val -= val;
	} else {
		UiValues[index].current_val = UiValues[index].min_val;
	}

	IDisplayUpdate();

	return res;
}

static U8_t IMapIndexToAddress(U8_t index)
{
	U8_t addr = 0;

	switch(index) {
	case UI_VALUE_INDEX_AMOUNT: {
		addr = SCHEDULE_MBOX_ADDR_AMOUNT;
		break;
	}
	case UI_VALUE_INDEX_FREQ: {
		addr = SCHEDULE_MBOX_ADDR_FREQ;
		break;
	}
	}

	return addr;
}

static void IDisplayUpdate(void)
{
	SevenSegDisplaySet(UiValues[SelectedValue]);
}


static void ITimerCallbackValueStable(Id_t timer_id, void *context)
{
	MailboxPost(MboxSchedule, IMapIndexToAddress(SelectedValue), (U16_t)UiValues[SelectedValue].current_val, OS_TIMEOUT_NONE);
}

static void ITimerCallbackUiInactive(Id_t timer_id, void *context)
{
	TimerStop(TmrValueStable);
	TimerStop(TmrUiInactive);
	SevenSegDisplayEnable(0); /* Turn display off. */

	/* Configure the select button as wake-up trigger. */
	ButtonTriggerCallbackSet(BUTTON_UI_INC, BUTTON_TRIGGER_PRESS, NULL);
	ButtonTriggerCallbackSet(BUTTON_UI_DEC, BUTTON_TRIGGER_PRESS, NULL);
	ButtonTriggerCallbackSet(BUTTON_UI_SEL, BUTTON_TRIGGER_RELEASE, IButtonCallbackUiActivate);
	ButtonCallbackSelectRelease = IButtonCallbackUiActivate;

	ITimerCallbackValueStable(TmrValueStable, NULL);
	SelectedValue = 0;
}

/* Button callbacks. */

static void IButtonCallbackUiActivate(Button_t button, ButtonTrigger_t trigger)
{
	ButtonTriggerCallbackSet(BUTTON_UI_INC, BUTTON_TRIGGER_PRESS, IButtonCallbackIncrement);
	ButtonTriggerCallbackSet(BUTTON_UI_DEC, BUTTON_TRIGGER_PRESS, IButtonCallbackDecrement);
	ButtonTriggerCallbackSet(BUTTON_UI_SEL, BUTTON_TRIGGER_RELEASE, IButtonCallbackSelect);
	ButtonCallbackSelectRelease = IButtonCallbackSelect;
	SevenSegDisplayEnable(1); /* Turn display on. */
	TimerReset(TmrUiInactive);
	TimerStart(TmrUiInactive);
}



static void IButtonCallbackIncrement(Button_t button, ButtonTrigger_t trigger)
{
	TimerReset(TmrUiInactive);
	IValueIncrement(SelectedValue, 1);
	TimerReset(TmrValueStable);
	TimerStart(TmrValueStable);
}

static void IButtonCallbackDecrement(Button_t button, ButtonTrigger_t trigger)
{
	TimerReset(TmrUiInactive);
	IValueDecrement(SelectedValue, 1);
	TimerReset(TmrValueStable);
	TimerStart(TmrValueStable);
}

static void IButtonCallbackSelect(Button_t button, ButtonTrigger_t trigger)
{
	TimerReset(TmrUiInactive);
	TimerStop(TmrValueStable);
	ITimerCallbackValueStable(TmrValueStable, NULL);

	if(SelectedValue < UI_NUM_VALUES - 1) {
		SelectedValue++;
	} else {
		SelectedValue = 0;
	}
	
	IDisplayUpdate();
}

static void IButtonCallbackPumpOn(Button_t button, ButtonTrigger_t trigger)
{
	TimerStop(TmrUiInactive);

	ButtonTriggerCallbackSet(BUTTON_UI_SEL, BUTTON_TRIGGER_RELEASE, IButtonCallbackPumpOff);
	MailboxPost(MboxIrrigation, IRRIGATION_MBOX_ADDR_TRIGGER, IRRIGATION_TRIGGER_MANUAL_ON, OS_TIMEOUT_NONE);
}

static void IButtonCallbackPumpOff(Button_t button, ButtonTrigger_t trigger)
{
	MailboxPost(MboxIrrigation, IRRIGATION_MBOX_ADDR_TRIGGER, IRRIGATION_TRIGGER_MANUAL_OFF, OS_TIMEOUT_NONE);
	ButtonTriggerCallbackSet(BUTTON_UI_SEL, BUTTON_TRIGGER_RELEASE, ButtonCallbackSelectRelease); /* Restore callback. */

	TimerReset(TmrUiInactive);
	TimerStart(TmrUiInactive);
}

