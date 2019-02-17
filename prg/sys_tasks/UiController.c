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
#include "SystemManager.h"

/* Driver includes. */
#include "RgbLed/rgb_led.h"
#include "SevenSegDisplay/seven_seg.h"
#include "Button/button.h"

/* OS includes. */
#include "PriorRTOS.h"

/* Standard includes. */
#include <stdlib.h>
#include <string.h>
#include <SystemEvg.h>

LOG_FILE_NAME("UiController");

static void UiControllerTask(const void *p_arg, U32_t v_arg);

static SysResult_t IValueDecrement(U8_t index, S32_t val);
static SysResult_t IValueIncrement(U8_t index, S32_t val);
static void IDisplayUpdate(U32_t value);
static void IUiInit(void);
static void IRgbLedStateSet(SysState_t state);
static void ISysStateSet(SysState_t state);
static void ISysStateReturn(void);
const char *ISysStateToString(SysState_t state);
static U8_t IStateIndexFromState(SysState_t state);

static void ITimerCallbackValueStable(Id_t timer_id, void *context);
static void ITimerCallbackUiInactive(Id_t timer_id, void *context);

static void IButtonCallbackIncrement(Button_t button, ButtonTrigger_t trigger);
static void IButtonCallbackDecrement(Button_t button, ButtonTrigger_t trigger);
static void IButtonCallbackSelect(Button_t button, ButtonTrigger_t trigger);
static void IButtonCallbackPumpOn(Button_t button, ButtonTrigger_t trigger);
static void IButtonCallbackPumpOff(Button_t button, ButtonTrigger_t trigger);
static void IButtonCallbackUiActivate(Button_t button, ButtonTrigger_t trigger);
static void IButtonCallbackClearError(Button_t button, ButtonTrigger_t trigger);

static volatile ButtonCallback_t ButtonCallbackSelectRelease;

static volatile U8_t SelectedValue = 0;
static Id_t TmrValueStable = ID_INVALID;
static Id_t TmrUiInactive = ID_INVALID;
static Id_t MboxIrrigation = ID_INVALID;
static Id_t MboxSchedule = ID_INVALID;
static Id_t EvgSystem = ID_INVALID;

static volatile SysState_t CurrentState = SYS_STATE_INITIALIZED;
static volatile SysState_t PreviousState = SYS_STATE_INITIALIZED;
static volatile SysState_t SavedState = SYS_STATE_INITIALIZED;

RgbLedColor_t SysStateColors[] = {
	RGB_LED_COLOR_RED,

	RGB_LED_COLOR_BLUE,
	RGB_LED_COLOR_VIOLET,

	RGB_LED_COLOR_GREEN,
	RGB_LED_COLOR_BLUE,

	RGB_LED_COLOR_RED,
	RGB_LED_COLOR_RED
};

RgbLedMode_t SysStateModes[] = {
	RGB_LED_MODE_ON,

	RGB_LED_MODE_BLINK,
	RGB_LED_MODE_BLINK,

	RGB_LED_MODE_ON,
	RGB_LED_MODE_ON,

	RGB_LED_MODE_BLINK,
	RGB_LED_MODE_ON
};

U32_t SysStateBlinkIntervals[] = {
	0,

	250,
	250,

	0,
	0,

	1000,
	0
};


SysResult_t UiControllerInit(Id_t mbox_irrigation, Id_t mbox_schedule)
{
	SysResult_t res = SYS_RESULT_OK;
	Id_t tsk_irrigation_controller = ID_INVALID;

	tsk_irrigation_controller = TaskCreate(UiControllerTask, TASK_CAT_MEDIUM, 1,
		(TASK_PARAMETER_START | TASK_PARAMETER_ESSENTIAL), 0, NULL, 0);
	TmrValueStable = TimerCreate(UI_THRESHOLD_VALUE_STABLE_MS, TIMER_PARAMETER_PERIODIC, ITimerCallbackValueStable, NULL);
	TmrUiInactive = TimerCreate(UI_THRESHOLD_INACTIVE_MS, TIMER_PARAMETER_PERIODIC, ITimerCallbackUiInactive, NULL);
	if(tsk_irrigation_controller == ID_INVALID || TmrValueStable == ID_INVALID || TmrUiInactive == ID_INVALID) {
		res = SYS_RESULT_ERROR;
	}

	if(res == SYS_RESULT_OK) {
		LOG_DEBUG_NEWLINE("UiController task and timers created.");
		MboxIrrigation = mbox_irrigation;
		MboxSchedule = mbox_schedule;
		EvgSystem = SystemEvgGet();
	}

	return res;
}


static void UiControllerTask(const void *p_arg, U32_t v_arg)
{
	OsResult_t res = OS_RES_ERROR;
	static U8_t ui_init_done = 0;
	static U8_t sys_flags = 0;
	static U8_t prev_sys_flags = 0;

	TASK_INIT_BEGIN() {
	} TASK_INIT_END();

	//LOG_DEBUG_NEWLINE("UiControllerTask running.");

	if(ui_init_done == 0) {
		res = EventgroupFlagsRequireSet(EvgSystem, SYSTEM_FLAG_SCHED_LOADED, OS_TIMEOUT_INFINITE);
		if(res == OS_RES_OK || res == OS_RES_EVENT) {
			LOG_DEBUG_NEWLINE("Current schedule values received from the ScheduleManager.");
			IUiInit();
			EventgroupFlagsClear(EvgSystem, SYSTEM_FLAG_SCHED_LOADED);
			ui_init_done = 1;
		}
	} else {

		prev_sys_flags = sys_flags;
		sys_flags = EventgroupFlagsGet(EvgSystem, 0xFF);

		if(sys_flags & SYSTEM_FLAG_ERROR_CRIT) {
			ISysStateSet(SYS_STATE_CRIT_ERROR);
		} else {
			if(sys_flags & SYSTEM_FLAG_PUMP_RUNNING) {
				ISysStateSet(SYS_STATE_PUMPING);
			} else if(sys_flags & SYSTEM_FLAG_ERROR) {
				ISysStateSet(SYS_STATE_ERROR);
			} else if(prev_sys_flags) {
				ISysStateReturn();
			}
		}

	}

	TaskSleep(500);
}


static SysResult_t IValueIncrement(U8_t index, S32_t val)
{
	SysResult_t res = SYS_RESULT_OK;

	if(UiValues[index].current_val + val <= UiValues[index].max_val) {
		UiValues[index].current_val += val;
	} else {
		UiValues[index].current_val = UiValues[index].max_val;
	}

	IDisplayUpdate(UiValues[SelectedValue].current_val);

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

	IDisplayUpdate(UiValues[SelectedValue].current_val);

	return res;
}

static void IDisplayUpdate(U32_t value)
{
	if(value > SEVEN_SEG_CONFIG_MAX_VALUE) {
		value = SEVEN_SEG_CONFIG_MAX_VALUE;
	}
	LOG_DEBUG_NEWLINE("Updating display value: %u.", value);
	SevenSegDisplaySet(value);
}

static void IUiInit(void)
{
	LOG_DEBUG_NEWLINE("Initializing UI.");
	/* Configure the select button as wake-up trigger. */
	ButtonTriggerCallbackSet(BUTTON_UI_SEL, BUTTON_TRIGGER_RELEASE, IButtonCallbackUiActivate);
	ButtonCallbackSelectRelease = IButtonCallbackUiActivate;

	/* Pump Test functionality can also be used when the UI is disabled. */
	ButtonTriggerCallbackSet(BUTTON_UI_SEL, BUTTON_TRIGGER_HOLD, IButtonCallbackPumpOn);
	ButtonTriggerHoldThresholdSet(BUTTON_UI_SEL, UI_THRESHOLD_PUMP_ON_MS);

	/* Syncing current schedule values. */
	IrrigationData_t irg_data;
	ScheduleManagerIrrigationDataGet(&irg_data);
	if(irg_data.irg_amount_l > UiValues[UI_VALUE_INDEX_AMOUNT].max_val) {
		UiValues[UI_VALUE_INDEX_AMOUNT].current_val = UiValues[UI_VALUE_INDEX_AMOUNT].max_val;
	} else if(irg_data.irg_amount_l < UiValues[UI_VALUE_INDEX_AMOUNT].min_val) {
		UiValues[UI_VALUE_INDEX_AMOUNT].current_val = UiValues[UI_VALUE_INDEX_AMOUNT].min_val;
	} else {
		UiValues[UI_VALUE_INDEX_AMOUNT].current_val = irg_data.irg_amount_l;
	}
	if(irg_data.irg_freq > UiValues[UI_VALUE_INDEX_FREQ].max_val) {
		UiValues[UI_VALUE_INDEX_FREQ].current_val = UiValues[UI_VALUE_INDEX_FREQ].max_val;
	} else if(irg_data.irg_freq < UiValues[UI_VALUE_INDEX_FREQ].min_val) {
		UiValues[UI_VALUE_INDEX_FREQ].current_val = UiValues[UI_VALUE_INDEX_FREQ].min_val;
	} else {
		UiValues[UI_VALUE_INDEX_FREQ].current_val = irg_data.irg_freq;
	}

	ButtonInterruptEnable(1);
	SevenSegDisplayEnable(1); /* Turn display on. */
	IDisplayUpdate(UiValues[SelectedValue].current_val); /* Make sure display will show a valid value when the UI is activated. */
	SevenSegDisplayEnable(0); /* Turn display off. */

	ISysStateSet(SYS_STATE_IDLE);
}

static void IRgbLedStateSet(SysState_t state)
{
	U8_t idx = IStateIndexFromState(state);
	RgbLedModeSet(RGB_LED_MODE_OFF);
	RgbLedColorSet(SysStateColors[idx]);
	if(SysStateModes[idx] == RGB_LED_MODE_BLINK) {
		RgbLedBlinkIntervalSet(SysStateBlinkIntervals[idx]);
	}
	RgbLedModeSet(SysStateModes[idx]);
}

static void ISysStateSet(SysState_t state)
{
	if(state == CurrentState) {
		return;
	}

	LOG_DEBUG_NEWLINE("[CURRENT] Saved state: %s | Current state: %s | New state: %s",
			ISysStateToString(SavedState), ISysStateToString(CurrentState), ISysStateToString(state));
	if(state == SYS_STATE_CLEAR_ERROR) {
		LOG_DEBUG_NEWLINE("Clearing error.");
		CurrentState = SavedState = SYS_STATE_IDLE;
		state = SYS_STATE_IDLE;
	}
	if(CurrentState < SYS_STATE_INDICATOR_PRESERVE) {
		if(CurrentState > SYS_STATE_INDICATOR_SAVE && CurrentState >= SavedState) {
			SavedState = CurrentState;
			LOG_DEBUG_NEWLINE("New saved state: %s", ISysStateToString(SavedState));
		}
		PreviousState = CurrentState;
		CurrentState = state;
		IRgbLedStateSet(CurrentState);
	} else {
		if(state > CurrentState) {
			PreviousState = CurrentState;
			CurrentState = state;
			IRgbLedStateSet(CurrentState);
		}
	}
	LOG_DEBUG_NEWLINE("[UPDATED] Saved state: %s | Current state: %s",
				ISysStateToString(SavedState), ISysStateToString(CurrentState));

}

static void ISysStateReturn(void)
{
	ISysStateSet(SavedState);
}

const char *ISysStateToString(SysState_t state)
{
	switch(state)
	{
	case SYS_STATE_SET_AMOUNT:
		return "SetAmount";
	case SYS_STATE_SET_FREQ:
		return "SetFrequency";
	case SYS_STATE_IDLE:
		return "Idle";
	case SYS_STATE_PUMPING:
		return "Pumping";
	case SYS_STATE_ERROR:
		return "Error";
	case SYS_STATE_CRIT_ERROR:
		return "CriticalError";
	case SYS_STATE_CLEAR_ERROR:
		return "ClearError";
	case SYS_STATE_INITIALIZED:
		return "Initialized";
	default:
		return "Invalid";
	}
}

static U8_t IStateIndexFromState(SysState_t state)
{
	U8_t idx = 0;

	switch(state) {
	default:
	case SYS_STATE_INITIALIZED: {
		idx = 0;
		break;
	}
	case SYS_STATE_SET_AMOUNT: {
		idx = 1;
		break;
	}
	case SYS_STATE_SET_FREQ: {
		idx = 2;
		break;
	}
	case SYS_STATE_IDLE: {
		idx = 3;
		break;
	}
	case SYS_STATE_PUMPING: {
		idx = 4;
		break;
	}
	case SYS_STATE_ERROR: {
		idx = 5;
		break;
	}
	case SYS_STATE_CRIT_ERROR: {
		idx = 6;
		break;
	}
	}

	return idx;
}

static void ITimerCallbackValueStable(Id_t timer_id, void *context)
{
	LOG_DEBUG_NEWLINE("Posting values in Schedule mailbox: Amount: %u | Freq: %u", UiValues[UI_VALUE_INDEX_AMOUNT].current_val, UiValues[UI_VALUE_INDEX_FREQ].current_val);
	MailboxPost(MboxSchedule, SCHEDULE_MBOX_ADDR_AMOUNT, (U16_t)UiValues[UI_VALUE_INDEX_AMOUNT].current_val, OS_TIMEOUT_NONE);
	MailboxPost(MboxSchedule, SCHEDULE_MBOX_ADDR_FREQ, (U16_t)UiValues[UI_VALUE_INDEX_FREQ].current_val, OS_TIMEOUT_NONE);
}

static void ITimerCallbackUiInactive(Id_t timer_id, void *context)
{
	LOG_DEBUG_NEWLINE("Deactivating UI");

	TimerStop(TmrUiInactive);
	SevenSegDisplayEnable(0); /* Turn display off. */

	/* Configure the select button as wake-up trigger. */
	ButtonTriggerCallbackSet(BUTTON_UI_INC, BUTTON_TRIGGER_PRESS, NULL);
	ButtonTriggerCallbackSet(BUTTON_UI_DEC, BUTTON_TRIGGER_PRESS, NULL);
	ButtonTriggerCallbackSet(BUTTON_UI_SEL, BUTTON_TRIGGER_RELEASE, IButtonCallbackUiActivate);
	ButtonCallbackSelectRelease = IButtonCallbackUiActivate;

	TimerReset(TmrValueStable);
	TimerStart(TmrValueStable);
	SelectedValue = 0;
	ISysStateReturn();
}

/* Button callbacks. */

static void IButtonCallbackUiActivate(Button_t button, ButtonTrigger_t trigger)
{
	LOG_DEBUG_NEWLINE("Activating UI");
	TimerStop(TmrValueStable);

	if(CurrentState != SYS_STATE_ERROR && CurrentState != SYS_STATE_PUMPING) {
		ButtonTriggerCallbackSet(BUTTON_UI_INC, BUTTON_TRIGGER_PRESS, IButtonCallbackIncrement);
		ButtonTriggerCallbackSet(BUTTON_UI_DEC, BUTTON_TRIGGER_PRESS, IButtonCallbackDecrement);
		ButtonTriggerCallbackSet(BUTTON_UI_SEL, BUTTON_TRIGGER_RELEASE, IButtonCallbackSelect);
		ButtonCallbackSelectRelease = IButtonCallbackSelect;
		IDisplayUpdate(UiValues[SelectedValue].current_val);
		SevenSegDisplayEnable(1); /* Turn display on. */

		ISysStateSet(SYS_STATE_SET_AMOUNT);
	} else if(CurrentState == SYS_STATE_ERROR) {
		ButtonTriggerCallbackSet(BUTTON_UI_SEL, BUTTON_TRIGGER_RELEASE, IButtonCallbackClearError);
		ButtonCallbackSelectRelease = IButtonCallbackClearError;
		IDisplayUpdate((U32_t)SystemErrorCodeGet());
		SevenSegDisplayEnable(1); /* Turn display on. */
	}

	TimerReset(TmrUiInactive);
	TimerStart(TmrUiInactive);
}



static void IButtonCallbackIncrement(Button_t button, ButtonTrigger_t trigger)
{
	LOG_DEBUG_NEWLINE("Increment value");

	TimerReset(TmrUiInactive);
	IValueIncrement(SelectedValue, 1);
}

static void IButtonCallbackDecrement(Button_t button, ButtonTrigger_t trigger)
{
	LOG_DEBUG_NEWLINE("Decrement value");

	TimerReset(TmrUiInactive);
	IValueDecrement(SelectedValue, 1);
}

static void IButtonCallbackClearError(Button_t button, ButtonTrigger_t trigger)
{
	LOG_DEBUG_NEWLINE("Clearing error code: %04d", SystemErrorCodeGet());

	SystemClearError();
	ISysStateSet(SYS_STATE_CLEAR_ERROR);

	ITimerCallbackUiInactive(TmrUiInactive, NULL);
}

static void IButtonCallbackSelect(Button_t button, ButtonTrigger_t trigger)
{
	TimerReset(TmrUiInactive);

	if(SelectedValue < UI_NUM_VALUES - 1) {
		SelectedValue++;
	} else {
		SelectedValue = 0;
	}

	if(SelectedValue == UI_VALUE_INDEX_AMOUNT) {
		ISysStateSet(SYS_STATE_SET_AMOUNT);
	} else if(SelectedValue == UI_VALUE_INDEX_FREQ) {
		ISysStateSet(SYS_STATE_SET_FREQ);
	}

	LOG_DEBUG_NEWLINE("Select: %d", SelectedValue);

	IDisplayUpdate(UiValues[SelectedValue].current_val);
}

static void IButtonCallbackPumpOn(Button_t button, ButtonTrigger_t trigger)
{
	LOG_DEBUG_NEWLINE("Pump on");

	ButtonTriggerCallbackSet(BUTTON_UI_SEL, BUTTON_TRIGGER_RELEASE, IButtonCallbackPumpOff);
	MailboxPost(MboxIrrigation, IRRIGATION_MBOX_ADDR_TRIGGER, IRRIGATION_TRIGGER_MANUAL_ON, OS_TIMEOUT_NONE);
}

static void IButtonCallbackPumpOff(Button_t button, ButtonTrigger_t trigger)
{
	LOG_DEBUG_NEWLINE("Pump off");

	MailboxPost(MboxIrrigation, IRRIGATION_MBOX_ADDR_TRIGGER, IRRIGATION_TRIGGER_MANUAL_OFF, OS_TIMEOUT_NONE);
	ButtonTriggerCallbackSet(BUTTON_UI_SEL, BUTTON_TRIGGER_RELEASE, ButtonCallbackSelectRelease); /* Restore callback. */
}



