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

#define IRRIGATION_CONTROLLER_CONFIG_MOISTURE_CHECK_INTERVAL_MS 10e3	/* 10 seconds */
#define IRRIGATION_CONTROLLER_CONFIG_MOISTURE_BACKOFF_INTERVAL_MS 6e5	/* 10 minutes */

#define IRRIGATION_MBOX_ADDR_AMOUNT		0x00
#define IRRIGATION_MBOX_ADDR_TRIGGER	0x01
#define IRRIGATION_MBOX_ADDR_MOISTURE_THRESHOLD	0x02

#define IRRIGATION_TRIGGER_SCHEDULE		0x00
#define IRRIGATION_TRIGGER_MANUAL_ON	0x01
#define IRRIGATION_TRIGGER_MANUAL_OFF	0x02

typedef enum {
	ERROR_PUMP_ACTIVATION = 1,
	ERROR_WATER_LEVEL	  = 2,
}IrrigationControllerError;

SysResult_t IrrigationControllerInit(Id_t *mbox_irrigation);

#endif /* IRRIGATION_CONTROLLER_H_ */
