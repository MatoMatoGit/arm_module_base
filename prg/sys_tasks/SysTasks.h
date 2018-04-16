/*
 * SysTasks.h
 *
 *  Created on: 16 apr. 2018
 *      Author: Dorus
 */

#ifndef SYS_TASKS_H_
#define SYS_TASKS_H_

#include "SysTaskCommon.h"
#include "PermStorageMngr.h"
#include "SystemState.h"

SysTaskResult_t SysTasksInit(void)
{
	SysTaskResult_t res = SYS_TASK_ERR;

	res = SysTaskPermStorageMngrInit();

	if(res == SYS_TASK_OK) {
		res = SysTaskSystemStateInit();
	}

	return res;
}

#endif /* SYS_TASKS_SYSTASKS_H_ */
