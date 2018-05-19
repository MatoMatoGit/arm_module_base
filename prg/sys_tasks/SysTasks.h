/*
 * SysTasks.h
 *
 *  Created on: 16 apr. 2018
 *      Author: Dorus
 */

#ifndef SYS_TASKS_H_
#define SYS_TASKS_H_

#include "PermStorageMngr.h"
#include "SystemState.h"

SysResult_t SysTasksInit(void)
{
	SysResult_t res = SYS_RESULT_ERROR;

	res = SysTaskPermStorageMngrInit();

	if(res == SYS_RESULT_OK) {
		res = SysTaskSystemStateInit();
	}

	return res;
}

#endif /* SYS_TASKS_SYSTASKS_H_ */
