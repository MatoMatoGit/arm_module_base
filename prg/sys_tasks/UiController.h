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

#define UI_THRESHOLD_VALUE_STABLE_MS	100
#define UI_THRESHOLD_PUMP_ON_MS			4000

SysResult_t UiControllerInit(Id_t mbox_irrigation, Id_t mbox_schedule, Id_t evg_system);

#endif /* UI_CONTROLLER_H_ */
