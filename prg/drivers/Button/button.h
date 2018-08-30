#ifndef BUTTON_H_
#define BUTTON_H_

typedef enum {
	BUTTON_TRIGGER_PRESS,
	BUTTON_TRIGGER_HOLD,
	BUTTON_TRIGGER_RELEASE
}ButtonTrigger_t;

typedef void (*ButtonCallback_t)(uint8_t button, ButtonTrigger_t trigger);

int ButtonTriggerCallbackSet(uint8_t button, ButtonTrigger_t trigger, ButtonCallback_t callback);
int ButtonTriggerThresholdSet(uint8_t button, uint32_t threshold_ms);



#endif