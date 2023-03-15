#ifndef _DEV_LCD_H
#define _DEV_LCD_H
#include <esp_types.h>
#include <esp_err.h>
enum
{
    LCD_SET_LIGHT,
    LCD_LIGHT_POWER,
};
typedef struct
{
    uint16_t xStart;
    uint16_t xEnd;
    uint16_t yStart;
    uint16_t yEnd;
    void *buf;
} dev_lcd_flush_t;
typedef struct
{
    uint16_t width;
    uint16_t height;
} dev_lcd_pix_t;

esp_err_t dev_lcd_init(void);
#endif