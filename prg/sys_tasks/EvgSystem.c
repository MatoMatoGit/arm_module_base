/*
 * EvgSystem.c
 *
 *  Created on: 6 okt. 2018
 *      Author: Dorus
 */


#include "EvgSystem.h"

#include "include/Eventgroup.h"

static Id_t EvgSystem = ID_INVALID;

OsResult_t EvgSystemInit(void)
{
	OsResult_t res = OS_RES_OK;

	EvgSystem = EventgroupCreate();
	if(EvgSystem == ID_INVALID) {
		res = OS_RES_ERROR;
	}

	return res;
}

Id_t EvgSystemGet(void)
{
	Id_t evg_sys = EvgSystem;

	return evg_sys;
}
