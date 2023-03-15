#ifndef _LCD_H
#define _LCD_H
#include <esp_types.h>
/* LCD CONFIG */
#define LCD_PIXEL_CLOCK_HZ   (20 * 1000 * 1000)
// The pixel number in horizontal and vertical
#define LCD_H_RES            170
#define LCD_V_RES            320
#define LCD_PIX_SIZE            (LCD_H_RES * LCD_V_RES)
// The pixel number in horizontal and vertical
void lcd_init(void);
void lcd_flush(int x_start, int y_start, int x_end, int y_end, const void *buf);
void lcd_set_light(uint8_t lighteness);
#endif