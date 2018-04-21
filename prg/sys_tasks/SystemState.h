/*
 * SystemState.h
 *
 *  Created on: 16 apr. 2018
 *      Author: Dorus
 */

#ifndef SYSTEM_STATE_H_
#define SYSTEM_STATE_H_

#include "GFsm.h"

#include <stdlib.h>

#define SYSTEM_STATUS_BLINK_FAST_INTERVAL_MS 100
#define SYSTEM_STATUS_BLINK_SLOW_INTERVAL_MS 800

#define SYSTEM_STATE_OK		GFSM_OK
#define SYSTEM_STATE_FAIL	GFSM_FAIL
#define SYSTEM_STATE_ERR	GFSM_OK

void SysStateInitOnEnter(FsmState_t prev_state, FsmState_t new_state);


#endif /* SYSTEM_STATE_H_ */
