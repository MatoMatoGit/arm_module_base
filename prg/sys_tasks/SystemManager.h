/*
 * SystemLog.h
 *
 *  Created on: 17 okt. 2018
 *      Author: Dorus
 */

#ifndef SYSTEM_MANAGER_H_
#define SYSTEM_MANAGER_H_

#include "SystemResult.h"
#include "include/StdTypes.h"

typedef enum {
	SYSTEM_ERROR_CRIT,

	SYSTEM_ERROR,

	SYSTEM_ERROR_NONE,
}SystemError_t;

typedef enum {
	SYSTEM_COMP_DRIVER_PUMP = 1,
	SYSTEM_COMP_DRIVER_RGB_LED = 2,
	SYSTEM_COMP_DRIVER_STORAGE = 3,
	SYSTEM_COMP_DRIVER_BUTTON = 4,
	SYSTEM_COMP_DRIVER_TIME = 5,
	SYSTEM_COMP_DRIVER_SEVEN_SEG = 6,

	SYSTEM_COMP_RTOS = 7,

	SYSTEM_COMP_APP_IRRIGATION_CONTROLLER = 8,
	SYSTEM_COMP_APP_SCHEDULE_MANAGER = 9,
	SYSTEM_COMP_APP_COMPOSER = 10,
	SYSTEM_COMP_APP_UI_CONTROLLER = 11
}SystemComponent_t;

SysResult_t SystemManagerInit(void);

SysResult_t SystemRaiseError(SystemComponent_t comp, SystemError_t err, U8_t num);

SysResult_t SystemClearError(void);

SystemError_t SystemErrorGet(void);

U16_t SystemErrorCodeGet(void);




#endif /* SYSTEM_MANAGER_H_ */
