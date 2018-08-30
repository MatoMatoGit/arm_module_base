/*
 * ui_config.h
 *
 *  Created on: 1 aug. 2018
 *      Author: Dorus
 */

#ifndef UI_CONFIG_H_
#define UI_CONFIG_H_

#define UI_NUM_VALUES 2 /*Max. 255 */

#define UI_VALUE_INDEX_AMOUNT			0
#define UI_VALUE_AMOUNT_LITERS_MAX		1000
#define UI_VALUE_AMOUNT_LITERS_MIN		1
#define UI_VALUE_AMOUNT_LITERS_DEFAULT	10

#define UI_VALUE_INDEX_FREQ		1
#define UI_VALUE_FREQ_MAX		1000
#define UI_VALUE_FREQ_MIN		1
#define UI_VALUE_FREQ_DEFAULT	UI_VALUE_FREQ_MIN

typedef struct {
	S32_t default_val;
	S32_t current_val;
	S32_t max_val;
	S32_t min_val;
}UiValue_t;

UiValue_t UiValues[UI_NUM_VALUES] = {
	{
		.default_val = UI_VALUE_AMOUNT_LITERS_DEFAULT,
		.current_val = 0,
		.max_val = UI_VALUE_AMOUNT_LITERS_MAX,
		.min_val = UI_VALUE_AMOUNT_LITERS_MIN,
	},
	{
		.default_val = UI_VALUE_FREQ_DEFAULT,
		.current_val = 0,
		.max_val = UI_VALUE_FREQ_MAX,
		.min_val = UI_VALUE_FREQ_MIN,
	},
};

#endif /* UI_CONFIG_H_ */
