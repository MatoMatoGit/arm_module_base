/*
 * ow_port_macro.h
 *
 *  Created on: Jun 15, 2019
 *      Author: dorus
 */

#ifndef OW_PORT_MACRO_H_
#define OW_PORT_MACRO_H_

#include "dwt_stm32_delay.h"

#define OW_PORT_WAIT_US(us) DWT_Delay_us(us)

#endif /* OW_PORT_MACRO_H_ */
