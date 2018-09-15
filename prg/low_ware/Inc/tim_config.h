/*
 * tim_config.h
 *
 *  Created on: 15 sep. 2018
 *      Author: Dorus
 */

#ifndef TIM_CONFIG_H_
#define TIM_CONFIG_H_

#include "tim.h"

#include "stm32f1xx_hal.h"

typedef struct {
	Timer_t timer;
	TIM_TypeDef *instance;

	IRQn_Type irqn_elapsed;
	TimerCallback_t cb_elapsed; /* NULL is allowed. */
	uint32_t irq_prio_elapsed;
	uint32_t irq_subprio_elapsed;
}TimerConfig_t;


void TimerHwConfigSet(TimerConfig_t *config);

TIM_HandleTypeDef *TimerHwHandleFromIrqn(IRQn_Type irqn);

#endif /* TIM_CONFIG_H_ */
