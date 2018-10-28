/*
 * SystemManager.c
 *
 *  Created on: 17 okt. 2018
 *      Author: Dorus
 */

#include "SystemManager.h"

#include "Eventgroup.h"
#include "SystemEvg.h"

static Id_t SystemEvg = ID_INVALID;

static SystemError_t CurrentError = SYSTEM_ERROR_NONE;
static SystemComponent_t CurrentErrorComponent = SYSTEM_COMP_APP_COMPOSER;

SysResult_t SystemManagerInit(void)
{
	SystemEvg = SystemEvgGet();
}

SysResult_t SystemRaiseError(SystemComponent_t comp, SystemError_t err)
{
	CurrentErrorComponent = comp;
	CurrentError = err;

	switch(err) {
	case SYSTEM_ERROR: {
		EventgroupFlagsSet(SystemEvg, SYSTEM_FLAG_ERROR);
		break;
	}

	case SYSTEM_ERROR_CRIT: {
		EventgroupFlagsSet(SystemEvg, SYSTEM_FLAG_ERROR_CRIT);
		break;
	}
	}

}

SysResult_t SystemClearError(void)
{
	CurrentError = SYSTEM_ERROR_NONE;
	EventgroupFlagsClear(SystemEvg, (SYSTEM_FLAG_ERROR | SYSTEM_FLAG_ERROR_CRIT));
}

SystemError_t SystemErrorGet(void)
{
	return CurrentError;
}
