/*
 * IrrigationController.h
 *
 *  Created on: 21 mei 2018
 *      Author: Dorus
 */

#ifndef IRRIGATION_CONTROLLER_H_
#define IRRIGATION_CONTROLLER_H_

#include "SystemResult.h"
#include "OsTypes.h"

#define IRRIGATION_MBOX_ADDR_AMOUNT		0x00
#define IRRIGATION_MBOX_ADDR_TRIGGER	0x01

#define IRRIGATION_TRIGGER_SCHEDULE		0x00
#define IRRIGATION_TRIGGER_MANUAL_ON	0x01
#define IRRIGATION_TRIGGER_MANUAL_OFF	0x02

SysResult_t IrrigationControllerInit(Id_t mbox_irrigation);

#endif /* IRRIGATION_CONTROLLER_H_ */
