/*
 * alpha_gui.c
 *
 *  Created on: Jun 10, 2019
 *      Author: dorus
 */


#include "alpha_gui.h"

#include <stdlib.h>
#include <stdio.h>

typedef struct {
	uint8_t col;
	uint8_t row;
	AguiFnCursorDraw_t draw;
	AguiField_t *field;
	bool select;
} AguiCursor_t;

typedef struct {
	AguiFnTextDraw_t draw_text;
	AguiFnClear_t clear;
	AguiCursor_t cursor;
	AguiWindow_t *curr_window;
	AguiWindow_t*prev_window;
	uint8_t display_fields[AGUI_CONFIG_DISPLAY_NUM_ROWS];
} Agui_t;

static void IAguiWindowSet(AguiWindow_t *window);

Agui_t Agui;

void AguiInit(AguiInit_t *init)
{
	Agui.draw_text = init->draw_text;
	Agui.cursor.draw = init->draw_cursor;
	Agui.clear = init->clear;

	AguiWindowNextSet(init->window);
}

void AguiRefresh(void)
{
	Agui.clear();
	for(uint8_t i = 0; i < AGUI_CONFIG_DISPLAY_NUM_ROWS; i++) {
		AguiFieldDraw(Agui.curr_window->fields[Agui.display_fields[i]], i);
	}
	Agui.cursor.draw(Agui.cursor.row, Agui.cursor.col, Agui.cursor.select);
}

void AguiWindowNextSet(AguiWindow_t *window)
{

	IAguiWindowSet(window);
}

AguiWindow_t *AguiWindowPrevGet(void)
{
	return Agui.prev_window;
}

void AguiWindowPrev(void)
{
	IAguiWindowSet(Agui.prev_window);
}

AguiWindow_t *AguiWindowGet(void)
{
	return Agui.curr_window;
}

void AguiFieldDraw(AguiField_t *field, uint8_t row)
{
	char val_string_buf[10];

	for(uint8_t i = 0; i < AGUI_CONFIG_NUM_STRINGS; i++) {
		if(field->strings[i] != NULL) {
			Agui.draw_text(field->strings[i], row, field->col_strings[i]);
		}
	}
	for(uint8_t i = 0; i < AGUI_CONFIG_NUM_VALUES; i++) {
		if(field->values[i] != NULL) {
			snprintf(val_string_buf, sizeof(val_string_buf), "%02d", (int)*field->values[i]);
			Agui.draw_text(val_string_buf, row, field->col_values[i]);
		}
	}
}

void AguiSelect(void)
{
	if(Agui.cursor.select == false) {
		Agui.cursor.select = true;
	} else {
		Agui.cursor.select = false;
	}

	if(Agui.cursor.field->cb_select != NULL) {
		Agui.cursor.field->cb_select(Agui.cursor.field);
	}

	if(Agui.cursor.field->link_window != NULL) {
		IAguiWindowSet(Agui.cursor.field->link_window);
	}
}

void AguiModify(FieldModification_e mod)
{

}

void AguiCursorVisible(bool visible);

void AguiCursorMove(CursorDirection_e dir)
{
	Agui.cursor.select = false;

	switch(dir) {
	case CURSOR_DIRECTION_UP:
		if(Agui.cursor.row > 0) {
			Agui.cursor.row--;
		} else if(Agui.display_fields[0] > 0) {
			Agui.cursor.row = AGUI_CONFIG_DISPLAY_NUM_ROWS - 1;
			Agui.display_fields[1] = Agui.display_fields[0];
			Agui.display_fields[0]--;
		}

		break;

	case CURSOR_DIRECTION_DOWN:
		if(Agui.cursor.row < (AGUI_CONFIG_DISPLAY_NUM_ROWS - 1)) {
			Agui.cursor.row++;
		} else if (Agui.display_fields[1] < (Agui.curr_window->num_fields - 1)) {
			Agui.cursor.row = 0;
			Agui.display_fields[0] = Agui.display_fields[1];
			Agui.display_fields[1]++;
		}
		break;
	}

	Agui.cursor.field = Agui.curr_window->fields[Agui.display_fields[Agui.cursor.row]];
}

AguiField_t *AguiCursorFieldGet(void)
{
	return Agui.cursor.field;
}

static void IAguiWindowSet(AguiWindow_t *window)
{
	Agui.prev_window = Agui.curr_window;
	Agui.curr_window = window;
	Agui.display_fields[0] = 0;
	Agui.display_fields[1] = Agui.display_fields[0] + 1;
	Agui.cursor.col = 0;
	Agui.cursor.row = 0;
	Agui.cursor.select = false;
	Agui.cursor.field = Agui.curr_window->fields[Agui.display_fields[Agui.cursor.row]];
}
