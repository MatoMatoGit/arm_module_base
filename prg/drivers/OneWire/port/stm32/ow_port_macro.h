/*
 * ow_port_macro.h
 *
 *  Created on: Jun 15, 2019
 *      Author: dorus
 */

#ifndef OW_PORT_MACRO_H_
#define OW_PORT_MACRO_H_

#define OW_PORT_WAIT_US(us) do {\
	asm volatile (	"MOV R0,%[loops]\n\t"\
			"1: \n\t"\
			"SUB R0, #1\n\t"\
			"CMP R0, #0\n\t"\
			"BNE 1b \n\t" : : [loops] "r" (16*us) : "memory"\
		      );\
} while(0)

#endif /* OW_PORT_MACRO_H_ */
