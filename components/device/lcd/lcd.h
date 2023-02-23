#ifndef _LCD_H
#define _LCD_H
#include <esp_types.h>
#include "../device.h"

// The pixel number in horizontal and vertical
void lcd_init(void);
void lcd_flush(int x_start,int y_start,int x_end,int y_end,const void* buf);
void lcd_set_light(uint8_t lighteness);
#endif