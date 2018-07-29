/*
 * SystemResult.h
 *
 *  Created on: 11 mei 2018
 *      Author: Dorus
 */

#ifndef SYSTEM_RESULT_H_
#define SYSTEM_RESULT_H_

typedef enum {
	SYS_RESULT_ERROR	= -3,
	SYS_RESULT_INV_ARG	= -2,
	SYS_RESULT_FAIL		= -1,
	SYS_RESULT_OK		=  0,
	SYS_RESULT_BUSY		=  1,
	SYS_RESULT_TIMEOUT	=  2
}SysResult_t;

#endif /* SYSTEM_RESULT_H_ */
