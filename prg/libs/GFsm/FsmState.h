/*
 * FsmState.h
 *
 *  Created on: 21 apr. 2018
 *      Author: Dorus
 */

#ifndef FSM_STATE_H_
#define FSM_STATE_H_

typedef enum {
	SYS_STATE_INITIALIZATION = 0, /* Must be 0. */
	SYS_STATE_IDLE,
	SYS_STATE_RUNNING,
	SYS_STATE_CONNECTED,
	SYS_STATE_TRANSFER,
	SYS_STATE_PUMPING,
	SYS_STATE_ERROR,
	SYS_STATE_CRIT_ERROR,
	FSM_STATE_NUM /* Must be the last in the enum. */
}FsmState_t;


#endif /* FSM_STATE_H_ */
