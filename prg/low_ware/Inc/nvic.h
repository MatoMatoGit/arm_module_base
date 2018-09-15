/*
 * nvic.h
 *
 *  Created on: 26 aug. 2018
 *      Author: Dorus
 */

#ifndef NVIC_H_
#define NVIC_H_

#include "stm32f1xx_hal.h"

void NvicInterruptPrioritySet(IRQn_Type irq, uint32_t prio, uint32_t sub_prio);

void NvicInterruptEnable(IRQn_Type irq, uint8_t en);

#endif /* NVIC_H_ */
