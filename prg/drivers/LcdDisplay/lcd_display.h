/*
 * lcd_display.h
 *
 *  Created on: Mar 2, 2019
 *      Author: dorus
 */

#ifndef LCD_DISPLAY_H_
#define LCD_DISPLAY_H_

#include "SystemResult.h"

#include "hd44780.h"

SysResult_t LcdDisplayInit(void);

HD44780 *LcdDisplayHandleGet(void);

#endif /* LCD_DISPLAY_H_ */
