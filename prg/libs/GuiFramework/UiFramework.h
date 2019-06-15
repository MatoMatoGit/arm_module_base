/*
 * UiFramework.h
 *
 *  Created on: May 30, 2019
 *      Author: dorspi
 */

#ifndef UI_FRAMEWORK_H_
#define UI_FRAMEWORK_H_

#define UI_CONFIG_MAX_LINKED_OBJECTS		3	/* Max. 255. */
#define UI_CONFIG_MAX_NUM_OBJECTS_WIDTH		2	/* Max. 255. */
#define UI_CONFIG_MAX_NUM_OBJECTS_HEIGHT	2	/* Max. 255. */
#define UI_CONFIG_MAX_WINDOWS				10	/* Max. 255. */

#if UI_CONFIG_MAX_LINKED_OBJECTS > UINT8_MAX
#error "Ui: Error! UI_CONFIG_MAX_LINKED_OBJECTS exceeds maximum value."
#endif

#if UI_CONFIG_MAX_OBJECTS_PER_WINDOW > UINT8_MAX
#error "Ui: Error! UI_CONFIG_MAX_OBJECTS_PER_WINDOW exceeds maximum value."
#endif

#if UI_CONFIG_MAX_WINDOWS > UINT8_MAX
#error "Ui: Error! UI_CONFIG_MAX_WINDOWS exceeds maximum value."
#endif

typedef enum {
	UI_RESULT_OK = 0,
	UI_RESULT_ERROR,
} UiResult_e;

typedef enum {
	UI_OBJECT_TYPE_VALUE = 0,
	UI_OBJECT_TYPE_TEXT,
	UI_OBJECT_TYPE_SYMBOL,
	UI_OBJECT_TYPE_LINK,
	UI_OBJECT_TYPE_LINE,
	UI_OBJECT_TYPE_SHAPE,
	UI_OBJECT_TYPE_BITMAP,
} UiObjectType_e;

typedef struct {
	uint16_t x;
	uint16_t y;
} UiCoordinates_t;

typedef struct {
	uint16_t w;
	uint16_t h;
} UiDimensions_t;

typedef struct {
	bool selectable;
	bool mutable;
	bool visible;
} UiProperties_t;

typedef struct {
	uint32_t value;
	bool negative;
} ObjectDataValue_t;

typedef struct {
	char *text;
} ObjectDataText_t;

typedef struct {
	void *symbol;
} ObjectDataSymbol_t;

typedef struct {
	UiWindow_t *link;
} ObjectDataLink_t;

typedef struct {
	union {
		ObjectDataValue_t	data_value;
		ObjectDataText_t	data_text;
		ObjectDataSymbol_t	data_symbol;
		ObjectDataLink_t	data_link;
	};
	uint16_t size;
} UiObjectData_t;

typedef UiResult_e (*UiLcdFnRender_t)(void *obj, UiObjectType_e type);
typedef UiResult_e (*UiLcdFnEnable_t)(uint8_t en);

typedef struct {
	UiLcdFnRender_t render_object;
	UiLcdFnEnable_t lcd_enable;
} UiLcdFunctions_t;

typedef struct {
	/*
	 * type; string, value, symbol, line, shape, bitmap
	 * location x, y
	 * size w, h
	 * properties: selectable, changeable
	 * linked objects
	 * num linked objects
	 *
	 * object left
	 * object right
	 * object up
	 * object down
	 *
	 * context
	 * cb select
	 * cb change
	 * cb load
	 */

	UiObjectType_e type;
	UiCoordinates_t coordinates;
	UiDimensions_t dimensions;
	UiProperties_t properties;

	UiObjectData_t obj_data;

	UiObjectCommon_t *linked_objs[UI_CONFIG_MAX_LINKED_OBJECTS];
	uint8_t num_linked_objs;
} UiObjectCommon_t;

typedef struct {
	UiObjectCommon_t common;
} UiObjectText_t;

typedef struct {
	UiObjectCommon_t common;
} UiObjectValue_t;

typedef struct {
	UiObjectCommon_t common;
} UiObjectSymbol_t;

typedef struct {
	UiObjectCommon_t common;
} UiObjectLink_t;

typedef struct {
	UiDimensions_t dimensions;
	UiObjectCommon_t *objs[UI_CONFIG_MAX_NUM_OBJECTS_HEIGHT][UI_CONFIG_MAX_NUM_OBJECTS_WIDTH];
	uint8_t num_objs;
} UiWindow_t;


typedef struct {
	UiDimensions_t dimensions;
	UiLcdFunctions_t *lcd_fn;
	UiWindow_t *windows[UI_CONFIG_MAX_WINDOWS];
	uint8_t num_windows;

	UiWindow_t *curr_window;
	UiWindow_t *next_window;
	UiWindow_t *prev_window;
} Ui_t;

/*
 * Increment field
 * Decrement field
 * Change field
 * Invert field
 * Select field
 * Set prev window
 * Get prev window
 * Goto prev window
 * Goto window
 * Draw window
 * Draw UI
 * UI enable
 * Add field
 * Remove field
 *
 * Move cursor up
 * Move cursor down
 * Move cursor left
 * Move cursor right
 *
 */

UiObjectCommon_t ObjSelectRow0 = {
	.type = UI_OBJECT_TYPE_SYMBOL,
	.coordinates = {.x = 0, y = 0},
	.dimensions = {.w = 1, .h = 1},
	.properties = {.selectable = false, .mutable = false, .visible = true},
	.obj_data = {.size = 10, .data_symbol = NULL},
	.linked_objs = {NULL},
	.num_linked_objs = 0,
};

UiObjectCommon_t ObjSelectRow1 = {
		.type = UI_OBJECT_TYPE_SYMBOL,
		.coordinates = {.x = 0, y = 1},
		.dimensions = {.w = 1, .h = 1},
		.properties = {.selectable = false, .mutable = false, .visible = true},
		.obj_data = {.size = 10, .data_symbol = NULL},
		.linked_objs = {NULL},
		.num_linked_objs = 0,
};

const char *TextMoisture = "vochtigheid";

UiObjectCommon_t ObjMoistureValue = {
	.type = UI_OBJECT_TYPE_TEXT,
	.coordinates = {.x = 13, y = 0},
	.dimensions = {.w = 3, .h = 1},
	.properties = {.selectable = false, .mutable = false, .visible = true},
	.obj_data = {.size = sizeof(uint16_t), .data_value = 0},
	.linked_objs = {NULL},
	.num_linked_objs = 0,
};

UiObjectCommon_t ObjMoistureText = {
	.type = UI_OBJECT_TYPE_TEXT,
	.coordinates = {.x = 1, y = 0},
	.dimensions = {.w = 12, .h = 1},
	.properties = {.selectable = false, .mutable = false, .visible = true},
	.obj_data = {.size = sizeof(TextMoisture) .data_text = TextMoisture},
	.linked_objs = {&ObjMoistureValue},
	.num_linked_objs = 1,
};




#endif /* UI_FRAMEWORK_H_ */
