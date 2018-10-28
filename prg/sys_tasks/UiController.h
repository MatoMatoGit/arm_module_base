/*
 * UiController.h
 *
 *  Created on: 21 mei 2018
 *      Author: Dorus
 */

#ifndef UI_CONTROLLER_H_
#define UI_CONTROLLER_H_

#include "SystemResult.h"
#include "OsTypes.h"

#define UI_THRESHOLD_VALUE_STABLE_MS	5000
#define UI_THRESHOLD_PUMP_ON_MS			2500
#define UI_THRESHOLD_INACTIVE_MS		5000

typedef enum {
	SYS_STATE_INITIALIZED = 0,

	SYS_STATE_SET_AMOUNT = 1,
	SYS_STATE_SET_FREQ = 2,
	SYS_STATE_PUMPING = 3,

	SYS_STATE_INDICATOR_SAVE = 9,
	SYS_STATE_IDLE = 10,
	SYS_STATE_ERROR = 11,
	SYS_STATE_CLEAR_ERROR = 12,

	SYS_STATE_INDICATOR_PRESERVE = 19,
	SYS_STATE_CRIT_ERROR = 21
}SysState_t;

SysResult_t UiControllerInit(Id_t mbox_irrigation, Id_t mbox_schedule);

#endif /* UI_CONTROLLER_H_ */
