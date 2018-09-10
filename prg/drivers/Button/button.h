#ifndef BUTTON_H_
#define BUTTON_H_

#include <stdint.h>

#define BUTTON_CONFIG_STABLE_THRESHOLD_MS		5
#define BUTTON_CONFIG_HOLD_THRESHOLD_MS_DEFAULT	3000
#define BUTTON_CONFIG_PRESSED_LOW_ACTIVE		1

#define BUTTON_RESULT_OK 0
#define BUTTON_RESULT_ERR -1

typedef enum {
	BUTTON_UI_INC = 0,
	BUTTON_UI_DEC,
	BUTTON_UI_SEL,

	BUTTON_NUM, /* Must be the last field in this enum. */
}Button_t;

typedef enum {
	BUTTON_TRIGGER_PRESS,
	BUTTON_TRIGGER_HOLD,
	BUTTON_TRIGGER_RELEASE,

	BUTTON_TRIGGER_NUM, /* Must be the last field in this enum. */
}ButtonTrigger_t;

typedef void (*ButtonCallback_t)(Button_t button, ButtonTrigger_t trigger);

int ButtonInit(void);
int ButtonTriggerCallbackSet(Button_t button, ButtonTrigger_t trigger, ButtonCallback_t callback);
int ButtonTriggerHoldThresholdSet(Button_t button, uint32_t threshold_ms);



#endif
