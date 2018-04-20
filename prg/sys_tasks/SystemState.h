/*
 * SystemState.h
 *
 *  Created on: 16 apr. 2018
 *      Author: Dorus
 */

#ifndef SYSTEM_STATE_H_
#define SYSTEM_STATE_H_

#include <stdlib.h>

#define SYSTEM_STATUS_BLINK_FAST_INTERVAL_MS 100
#define SYSTEM_STATUS_BLINK_SLOW_INTERVAL_MS 800

void SysStateInitOnEnter(FsmState_t prev_state, FsmState_t new_state);


#endif /* SYSTEM_STATE_H_ */
