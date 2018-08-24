/*
 * Composer.h
 *
 *  Created on: 2 aug. 2018
 *      Author: Dorus
 */

#ifndef COMPOSER_H_
#define COMPOSER_H_

#include "SystemResult.h"
#include <stdbool.h>

SysResult_t ComposerInit(void);

void ComposerCallbackSetOnPumpStopped(CallbackPumpStopped_t cb);

#endif /* COMPOSER_H_ */
