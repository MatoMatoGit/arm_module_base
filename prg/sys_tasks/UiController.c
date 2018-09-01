/*
 * UiController.c
 *
 *  Created on: 21 mei 2018
 *      Author: Dorus
 */

/* Self include. */
#include "UiController.h"
#include "UiConfig.h"

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

static U8_t SelectedValue = 0;
static Id_t TmrValueStable = ID_INVALID;
static Id_t MboxIrrigation = ID_INVALID;
static Id_t MboxSchedule = ID_INVALID;
static Id_t EvgSystem = ID_INVALID;

static void UiControllerTask(void *p_arg, U32_t v_arg);

static SysResult_t IValueDecrement(U8_t index, S32_t val);
static SysResult_t IValueIncrement(U8_t index, S32_t val);
static U8_t IMapIndexToAddress(U8_t index);
static void IDisplayUpdate(void);

static void ITimerCallbackValueStable(Id_t timer_id, void *context);
static void IButtonCallbackIncrement(void);
static void IButtonCallbackDecrement(void);
static void IButtonCallbackSelect(void);
static void IButtonCallbackPumpOn(void);
static void IButtonCallbackPumpOff(void);

SysResult_t UiControllerInit(Id_t mbox_irrigation, Id_t mbox_schedule, Id_t evg_system)
{
	SysResult_t res = SYS_RESULT_OK;
	Id_t tsk_irrigation_controller = ID_INVALID;

	tsk_irrigation_controller = TaskCreate(UiControllerTask, TASK_CAT_REALTIME, 1,
		(TASK_PARAMETER_START | TASK_PARAMETER_ESSENTIAL), 0, NULL, 0);
	TmrValueStable = TimerCreate(UI_THRESHOLD_VALUE_STABLE_MS, TIMER_PARAMETER_PERIODIC, ITimerCallbackValueStable, NULL);
	if(tsk_irrigation_controller == ID_INVALID || TmrValueStable == ID_INVALID) {
		MboxIrrigation = mbox_irrigation;
		MboxSchedule = mbox_schedule;
		EvgSystem = evg_system;
		res = SYS_RESULT_ERROR;
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
	SysResult_t res = SYS_RESULT_ERROR;

	if(UiValues[index].current_val + val <= UiValues[index].max_val) {
		UiValues[index].current_val += val;
		IDisplayUpdate();
		res = SYS_RESULT_OK;
	}

	return res;
}

static SysResult_t IValueDecrement(U8_t index, S32_t val)
{
	SysResult_t res = SYS_RESULT_ERROR;

	if(UiValues[index].current_val - val >= UiValues[index].min_val) {
		UiValues[index].current_val -= val;
		IDisplayUpdate();
		res = SYS_RESULT_OK;
	}

	return res;
}

static void ITimerCallbackValueStable(Id_t timer_id, void *context)
{
	MailboxPost(MboxSchedule, IMapIndexToAddress(SelectedValue), (U16_t)UiValues[SelectedValue].current_val, OS_TIMEOUT_NONE);
}

static void IButtonCallbackIncrement(void)
{
	IValueIncrement(SelectedValue, 1);
	TimerReset(TmrValueStable);
	TimerStart(TmrValueStable);
}

static void IButtonCallbackDecrement(void)
{
	IValueDecrement(SelectedValue, 1);
	TimerReset(TmrValueStable);
	TimerStart(TmrValueStable);
}

static void IButtonCallbackSelect(void)
{
	TimerStop(TmrValueStable);
	ITimerCallbackValueStable(TmrValueStable, NULL);

	if(SelectedValue < UI_NUM_VALUES - 1) {
		SelectedValue++;
	} else {
		SelectedValue = 0;
	}
	
	IDisplayUpdate();
}

static void IButtonCallbackPumpOn(void)
{
	MailboxPost(MboxIrrigation, IRRIGATION_MBOX_ADDR_TRIGGER, IRRIGATION_TRIGGER_MANUAL_ON, OS_TIMEOUT_NONE);
}

static void IButtonCallbackPumpOff(void)
{
	MailboxPost(MboxIrrigation, IRRIGATION_MBOX_ADDR_TRIGGER, IRRIGATION_TRIGGER_MANUAL_OFF, OS_TIMEOUT_NONE);
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
