/*
 * Composer.h
 *
 *  Created on: 2 aug. 2018
 *      Author: Dorus
 */

#ifndef COMPOSER_H_
#define COMPOSER_H_

#include "SystemResult.h"

#include "pump.h"

SysResult_t ComposerInit(void);

void ComposerCallbackSetOnPumpStopped(PumpCallback_t cb);

void IComposerCallbackOnPumpStopped(void);

#endif /* COMPOSER_H_ */
