/*
 * EvgSystem.c
 *
 *  Created on: 6 okt. 2018
 *      Author: Dorus
 */


#include "SystemEvg.h"
#include "include/Eventgroup.h"

static Id_t EvgSystem = ID_INVALID;

OsResult_t SystemEvgInit(void)
{
	OsResult_t res = OS_RES_OK;

	EvgSystem = EventgroupCreate();
	if(EvgSystem == ID_INVALID) {
		res = OS_RES_ERROR;
	}

	return res;
}

Id_t SystemEvgGet(void)
{
	Id_t evg_sys = EvgSystem;

	return evg_sys;
}
