/*
 * UiController.c
 *
 *  Created on: 21 mei 2018
 *      Author: Dorus
 */

/* Self include. */
#include "UiController.h"

/* System tasks includes. */

/* Driver includes. */
#include "RgbLed/rgb_led.h"
#include "SevenSegDisplay/seven_seg.h"

/* OS includes. */
#include "PriorRTOS.h"

/* Standard includes. */
#include <stdlib.h>
#include <string.h>

LOG_FILE_NAME("UiController");

static void TaskUiController(void *p_arg, U32_t v_arg);

SysResult_t UiControllerInit(void)
{
	SysResult_t res = SYS_RESULT_OK;
	Id_t tsk_irrigation_controller = ID_INVALID;

	tsk_irrigation_controller = TaskCreate(TaskUiController, TASK_CAT_REALTIME, 1,
		(TASK_PARAMETER_START | TASK_PARAMETER_ESSENTIAL), 0, NULL, 0);
	if(tsk_irrigation_controller == ID_INVALID) {
		res = SYS_RESULT_ERROR;
	}

	return res;
}


static void TaskUiController(void *p_arg, U32_t v_arg)
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

	if(res == OS_RES_OK || res == OS_RES_EVENT) {
		res = MailboxPend(mbox_irrigation, IRRIGATION_MBOX_ADDR_AMOUNT, &amount, OS_TIMEOUT_NONE);
	}


}
