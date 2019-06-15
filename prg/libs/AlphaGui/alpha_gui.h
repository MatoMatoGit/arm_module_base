/*
 * AlphaGui.h
 *
 *  Created on: Jun 10, 2019
 *      Author: dorus
 */

#ifndef ALPHA_GUI_H_
#define ALPHA_GUI_H_

#include <stdint.h>
#include <stdbool.h>

#define AGUI_CONFIG_MAX_FIELDS 4
#define AGUI_CONFIG_DISPLAY_NUM_ROWS 2
#define AGUI_CONFIG_DISPLAY_NUM_COLUMNS 20
#define AGUI_CONFIG_NUM_STRINGS 2
#define AGUI_CONFIG_NUM_VALUES	  2

typedef struct AguiField AguiField_t;
typedef struct AguiWindow AguiWindow_t;

typedef void (*AguiCbFieldSelect_t)(AguiField_t *field);
typedef void (*AguiCbFieldValueModify_t)(AguiField_t *field, int32_t *value);
typedef void (*AguiCbFieldValueLoad_t)(AguiField_t *field, int32_t **value);
typedef void (*AguiFnTextDraw_t)(char *text, uint8_t row, uint8_t col);
typedef void (*AguiFnCursorDraw_t)(uint8_t row, uint8_t col, bool selected);
typedef void (*AguiFnClear_t)(void);

typedef enum {
	CURSOR_DIRECTION_UP = 0,
	CURSOR_DIRECTION_DOWN,
} CursorDirection_e;

typedef enum {
	FIELD_MOD_INC = 0,
	FIELD_MOD_DEC,
} FieldModification_e;

struct AguiField {
	AguiCbFieldSelect_t cb_select;
	AguiCbFieldValueModify_t cb_mod;
	AguiCbFieldValueLoad_t cb_load;

	char *strings[AGUI_CONFIG_NUM_STRINGS];
	uint8_t col_strings[AGUI_CONFIG_NUM_STRINGS];
	int32_t *values[AGUI_CONFIG_NUM_VALUES];
	uint8_t col_values[AGUI_CONFIG_NUM_VALUES];

	AguiWindow_t *link_window;
};

struct AguiWindow {
	AguiField_t *fields[AGUI_CONFIG_MAX_FIELDS];
	uint8_t num_fields;
};

typedef struct {
	AguiFnTextDraw_t draw_text;
	AguiFnCursorDraw_t draw_cursor;
	AguiFnClear_t clear;
	AguiWindow_t *window;
} AguiInit_t;

void AguiInit(AguiInit_t *init);
void AguiRefresh(void);
AguiWindow_t *AguiWindowPrevGet(void);
void AguiWindowNextSet(AguiWindow_t *window);
void AguiWindowPrev(void);
AguiWindow_t *AguiWindowGet(void);
void AguiSelect(void);
void AguiModify(FieldModification_e mod);

void AguiCursorMove(CursorDirection_e dir);
void AguiFieldDraw(AguiField_t *field, uint8_t row);

#endif /* ALPHA_GUI_H_ */
