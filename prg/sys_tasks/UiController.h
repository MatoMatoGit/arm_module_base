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

#define UI_THRESHOLD_VALUE_STABLE_MS	2000
#define UI_THRESHOLD_PUMP_ON_MS			2500
#define UI_THRESHOLD_INACTIVE_MS		5000

SysResult_t UiControllerInit(Id_t mbox_irrigation, Id_t mbox_schedule, Id_t evg_system);

#endif /* UI_CONTROLLER_H_ */
