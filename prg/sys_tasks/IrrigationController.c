/*
 * IrrigationController.c
 *
 *  Created on: 21 mei 2018
 *      Author: Dorus
 */

#include "IrrigationController.h"

#include "Pump/pump.h"
#include "PriorRTOS.h"

#include <stdlib.h>

LOG_FILE_NAME("IrrigationController.c");

#define PUMP_RUN_DELAY_MS	5000
#define PUMP_DURATION_HOUR	3600
#define LITER_TO_MILLILITER(ltr) (ltr * 1000)

U32_t PumpAmountMl = 0;

static void ICallbackPumpStopped(void);
static void ICallbackDelayedPumpRun(Id_t timer_id, void *context);

static void TaskIrrigationController(void *p_arg, U32_t v_arg);

SysResult_t IrrigationControllerInit(Id_t mbox_irrigation)
{
	SysResult_t res = SYS_RESULT_OK;
	Id_t tsk_irrigation_controller = ID_INVALID;

	tsk_irrigation_controller = TaskCreate(TaskIrrigationController, TASK_CAT_HIGH, 5,
		(TASK_PARAMETER_START | TASK_PARAMETER_ESSENTIAL), 0, NULL, (U32_t)mbox_irrigation);
	if(tsk_irrigation_controller == ID_INVALID) {
		res = SYS_RESULT_ERROR;
	}

	if(res == SYS_RESULT_OK) {
		res = PumpInit(ICallbackPumpStopped);
	}

	if(res == SYS_RESULT_OK) {
		PumpEnable(1);
	}
	return res;
}


static void TaskIrrigationController(void *p_arg, U32_t v_arg)
{
	static Id_t mbox_irrigation;
	OsResult_t res = OS_RES_ERROR;
	SysResult_t pump_res = SYS_RESULT_OK;
	U16_t amount = 0;
	U16_t trigger = 0;

	TASK_INIT_BEGIN() {
		mbox_irrigation = (Id_t)v_arg;
	} TASK_INIT_END();

	res = MailboxPend(mbox_irrigation, IRRIGATION_MBOX_ADDR_TRIGGER, &trigger, OS_TIMEOUT_INFINITE);

	if(res == OS_RES_OK) {
		res = MailboxPend(mbox_irrigation, IRRIGATION_MBOX_ADDR_AMOUNT, &amount, OS_TIMEOUT_NONE);
	}

	if(res == OS_RES_OK) {
		switch(trigger) {

			/* If the trigger is manual on the pump is ran indefinitely until
			 * a manual off trigger is received. */
			case IRRIGATION_TRIGGER_MANUAL_ON: {
				LOG_DEBUG_NEWLINE("Received trigger: manual, on.");
				pump_res = PumpRun();
				if(pump_res == SYS_RESULT_OK) {
					LOG_DEBUG_NEWLINE("Pump turned on.");
				} else {
					LOG_ERROR_NEWLINE("Pump could not be turned on.");
				}
				break;
			}

			/* If the trigger is manual off the pump is stopped. */
			case IRRIGATION_TRIGGER_MANUAL_OFF: {
				LOG_DEBUG_NEWLINE("Received trigger: manual, off.");
				pump_res = PumpStop();
				if(pump_res == SYS_RESULT_OK) {
					LOG_DEBUG_NEWLINE("Pump turned off.");
				} else {
					LOG_ERROR_NEWLINE("Pump could not be turned off.");
				}
				break;
			}

			/* If the trigger is the schedule the pump is ran for the amount received
			 * in the mailbox. */
			case IRRIGATION_TRIGGER_SCHEDULE: {
				LOG_DEBUG_NEWLINE("Received trigger: schedule.");
				LOG_DEBUG_NEWLINE("Received amount: %u L.", amount);
				PumpAmountMl = (U32_t)LITER_TO_MILLILITER(amount); /* Convert liters to milliliters and store it. */

				/*If the pump is already running due to a manual trigger, the
				 * scheduled run is delayed. This is because the scheduled
				 * run must still occur. */
				if(PumpIsRunning()) {
					LOG_DEBUG_NEWLINE("Pump is already running. Delaying scheduled run.");
					if(TimerCreate(PUMP_RUN_DELAY_MS, (TIMER_PARAMETER_PERIODIC | TIMER_PARAMETER_ON),
						ICallbackDelayedPumpRun, NULL) == ID_INVALID) {
						LOG_ERROR_NEWLINE("Delay timer was not created.");
					}
				} else {
					/* If the pump is not running, run it for the specified amount. */
					pump_res = PumpRunForAmount(PumpAmountMl);
					if(pump_res == SYS_RESULT_OK) {
						LOG_DEBUG_NEWLINE("Pump turned on.");
					} else {
						LOG_ERROR_NEWLINE("Pump could not be turned on.");
					}
				}
				break;
			}

			/* Invalid trigger value. */
			default: {
				LOG_ERROR_NEWLINE("Received trigger: invalid.");
				break;
			}
		}
	}
}

static void ICallbackPumpStopped(void)
{
	LOG_DEBUG_NEWLINE("Pump stopped.");
}

static void ICallbackDelayedPumpRun(Id_t timer_id, void *context)
{
	OS_ARG_UNUSED(context);

	/* If the pump is still running after the delay, the delay timer is reset.
	 * If the pump is not running anymore, the pump is ran to pump the scheduled
	 * amount. */
	if(PumpIsRunning()) {
		LOG_DEBUG_NEWLINE("Delay expired. Pump is still running. Delay again.");
		TimerReset(timer_id);
	} else {
		if(PumpRunForAmount(PumpAmountMl) == SYS_RESULT_OK) {
			LOG_DEBUG_NEWLINE("Delay expired. Pump turned on.");
		} else {
			LOG_ERROR_NEWLINE("Pump could not be turned on.");
		}
		PumpAmountMl = 0;
		TimerDelete(&timer_id);
	}
}

