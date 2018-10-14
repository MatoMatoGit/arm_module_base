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
	SYS_STATE_IDLE = 0,
	SYS_STATE_PUMPING,
	SYS_STATE_SET_AMOUNT,
	SYS_STATE_SET_FREQ,
	SYS_STATE_ERROR,
	SYS_STATE_CRIT_ERROR
}SysState_t;

SysResult_t UiControllerInit(Id_t mbox_irrigation, Id_t mbox_schedule);

#endif /* UI_CONTROLLER_H_ */
