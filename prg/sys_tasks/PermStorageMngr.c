/*
 * PermStorageMngr.c
 *
 *  Created on: 16 apr. 2018
 *      Author: Dorus
 */

#include "PermStorageMngr.h"

#include <PriorRTOS.h>

#include "stm32f1xx_hal_flash.h"
#include <stdint.h>

__attribute__((__section__(".user_data"))) const uint8_t LogData[DEBUG_LOG_SIZE_BYTES];


void SysTaskDebug(const void *p_args, U32_t v_arg)
{

	TASK_INIT_BEGIN() {
		TaskPollAdd(LoggerRingbuf, RINGBUF_EVENT_DATA_IN, OS_TIMEOUT_INFINITE);
		TaskPollAdd(LoggerRingbuf, RINGBUF_EVENT_FULL, OS_TIMEOUT_INFINITE);
	} TASK_INIT_END();

	/* Handle Data In and Full events of the Logger Ringbuffer. */
	if(TaskPoll(LoggerRingbuf, RINGBUF_EVENT_DATA_IN, OS_TIMEOUT_INFINITE, false) == OS_RES_EVENT
			|| TaskPoll(LoggerRingbuf, RINGBUF_EVENT_FULL, OS_TIMEOUT_INFINITE, false) == OS_RES_EVENT) {


		/* Read data from the ringbuffer. Make sure it is aligned. */


		/* Write data to flash. */
	}

}