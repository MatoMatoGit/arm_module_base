/*
 * sys_config.h
 *
 *  Created on: 24 aug. 2018
 *      Author: Dorus
 */

#ifndef SYS_CONFIG_H_
#define SYS_CONFIG_H_

/* Configuration files. */
#include "pump_config.h"
#include "rgb_led_config.h"
#include "storage_config.h"
//#include "ui_config.h"
#include "composer_config.h"

/***** Hardware Configuration macros. *****/

#define DISPLAY_NUM_DIGITS					4
#define DISPLAY_MAX_VALUE					9999
#define DISPLAY_REFRESH_RATE				25

/***** Software Configuration macros. *****/

#define EVG_SYSTEM_FLAG_ALARM				0x00
#define EVG_SYSTEM_FLAG_PUMP_RUNNING		0x01
#define EVG_SYSTEM_FLAG_ERROR				0x02
#define EVG_SYSTEM_FLAG_ERROR_CRIT			0x03

#define FILE_SIZE_LOG						0x200
#define FILE_SIZE_SCHEDULE					0x200


#endif /* SYS_CONFIG_H_ */
