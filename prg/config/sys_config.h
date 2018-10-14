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
#define DISPLAY_MAX_VALUE					9//999
#define DISPLAY_MIN_VALUE					1
#define DISPLAY_REFRESH_RATE				25

/***** Software Configuration macros. *****/

#define FILE_SIZE_LOG						0x400
#define FILE_SIZE_SCHEDULE					0x200
#define FILE_SIZE_TIME						0x200


#endif /* SYS_CONFIG_H_ */
