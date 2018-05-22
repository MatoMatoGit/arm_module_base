/*
 * IrrigationController.h
 *
 *  Created on: 21 mei 2018
 *      Author: Dorus
 */

#ifndef IRRIGATION_CONTROLLER_H_
#define IRRIGATION_CONTROLLER_H_

#include "SystemResult.h"
#include <OsTypes.h>
#include <Eventgroup.h>

#define IRRIGATION_CONTROLLER_TRIGGER_BUTTON	EVENTGROUP_FLAG_MASK_0
#define IRRIGATION_CONTROLLER_TRIGGER_SCHEDULE	EVENTGROUP_FLAG_MASK_1

SysResult_t IrrigationControllerInit(Id_t evg_trigger);

SysResult_t IrrigationControllerAmountSet(U16_t amount_ml);


#endif /* IRRIGATION_CONTROLLER_H_ */
