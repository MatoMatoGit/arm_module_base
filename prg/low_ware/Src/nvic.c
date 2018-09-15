/*
 * nvic.c
 *
 *  Created on: 26 aug. 2018
 *      Author: Dorus
 */
#include "nvic.h"

#include "stm32f1xx_hal.h"


//  HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0);
//  HAL_NVIC_EnableIRQ(ADC1_2_IRQn);

void NvicInterruptPrioritySet(IRQn_Type irq, uint32_t prio, uint32_t sub_prio)
{
	HAL_NVIC_SetPriority(irq, prio, sub_prio);
}

void NvicInterruptEnable(IRQn_Type irq, uint8_t en)
{
	if(en) {
		HAL_NVIC_EnableIRQ(irq);
	} else {
		HAL_NVIC_DisableIRQ(irq);
	}
}
