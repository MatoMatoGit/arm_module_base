/*
 * IrrigationController.c
 *
 *  Created on: 21 mei 2018
 *      Author: Dorus
 */

/* Self include. */
#include "IrrigationController.h"

/* System tasks includes. */
#include "Composer.h"

/* Driver includes. */
#include "Pump/pump.h"

/* OS includes. */
#include "PriorRTOS.h"

/* Standard includes. */
#include <stdlib.h>
#include <string.h>

LOG_FILE_NAME("IrrigationController");

#define PUMP_RUN_DELAY_MS	5000
#define PUMP_DURATION_HOUR	3600
#define LITER_TO_MILLILITER(ltr) (ltr * 1000)

#define MBOX_IRRIGATION_NUM_ADDR 2

U32_t PumpAmountMl = 0;

static volatile Id_t TmrIrrigationDelay = ID_INVALID;

static void ICallbackPumpStopped(void);
static void ICallbackDelayedPumpRun(Id_t timer_id, void *context);

static void IrrigationControllerTask(const void *p_arg, U32_t v_arg);

SysResult_t IrrigationControllerInit(Id_t *mbox_irrigation)
{
	SysResult_t res = SYS_RESULT_OK;
	Id_t tsk_irrigation_controller = ID_INVALID;

	tsk_irrigation_controller = TaskCreate(IrrigationControllerTask, TASK_CAT_HIGH, 5,
		(TASK_PARAMETER_ESSENTIAL), 0, NULL, 0);
	*mbox_irrigation = MailboxCreate(MBOX_IRRIGATION_NUM_ADDR, &tsk_irrigation_controller, 1);
	if(tsk_irrigation_controller == ID_INVALID || *mbox_irrigation == ID_INVALID) {
		LOG_ERROR_NEWLINE("Failed to create IrrigationController task or Irrigation mailbox");
		res = SYS_RESULT_ERROR;
	}

	if(res == SYS_RESULT_OK) {
		LOG_DEBUG_NEWLINE("IrrigationController initialized.");
		ComposerCallbackSetOnPumpStopped(ICallbackPumpStopped);
		PumpEnable(1);
		TaskResumeWithVarg(tsk_irrigation_controller, (U32_t)*mbox_irrigation);
	}
	
	return res;
}


static void IrrigationControllerTask(const void *p_arg, U32_t v_arg)
{
	static Id_t mbox_irrigation;
	OsResult_t res = OS_RES_ERROR;
	SysResult_t pump_res = SYS_RESULT_OK;
	U16_t amount = 0;
	U16_t trigger = 0;

	TASK_INIT_BEGIN() {
		mbox_irrigation = (Id_t)v_arg;
	} TASK_INIT_END();

	LOG_DEBUG_NEWLINE("IrrigationControllerTask running.");

	res = MailboxPend(mbox_irrigation, IRRIGATION_MBOX_ADDR_TRIGGER, &trigger, OS_TIMEOUT_INFINITE);

	if(res == OS_RES_OK || res == OS_RES_EVENT) {
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
				PumpStop();
				LOG_DEBUG_NEWLINE("Pump turned off.");
				break;
			}

			/* If the trigger is the schedule the pump is ran for the amount received
			 * in the mailbox. */
			case IRRIGATION_TRIGGER_SCHEDULE: {
				res = MailboxPend(mbox_irrigation, IRRIGATION_MBOX_ADDR_AMOUNT, &amount, OS_TIMEOUT_NONE);
				if(res == OS_RES_OK || res == OS_RES_EVENT) {
					LOG_DEBUG_NEWLINE("Received trigger: schedule.");
					LOG_DEBUG_NEWLINE("Received amount: %u L.", amount);
					PumpAmountMl = (U32_t)LITER_TO_MILLILITER(amount); /* Convert liters to milliliters and store it. */

					/*If the pump is already running due to a manual trigger, the
					 * scheduled run is delayed. This is because the scheduled
					 * run must still occur. */
					if(PumpIsRunning()) {
						LOG_DEBUG_NEWLINE("Pump is already running. Delaying scheduled run.");
						if(TmrIrrigationDelay == ID_INVALID) {
							TmrIrrigationDelay = TimerCreate(PUMP_RUN_DELAY_MS, (TIMER_PARAMETER_PERIODIC | TIMER_PARAMETER_ON),
									ICallbackDelayedPumpRun, NULL);
							if(TmrIrrigationDelay == ID_INVALID) {
								LOG_ERROR_NEWLINE("Delay timer was not created.");
							}
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
				} else {
					LOG_ERROR_NEWLINE("Amount unspecified.");
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

	//TaskSleep(5000);
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
		TimerReset(TmrIrrigationDelay);
	} else {
		if(PumpRunForAmount(PumpAmountMl) == SYS_RESULT_OK) {
			LOG_DEBUG_NEWLINE("Delay expired. Pump turned on.");
		} else {
			LOG_ERROR_NEWLINE("Pump could not be turned on.");
		}
		PumpAmountMl = 0;
		TimerDelete(&TmrIrrigationDelay);
	}
}

