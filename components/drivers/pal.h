#ifndef PAL_H
#define PAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <esp_types.h>
#include <esp_err.h>
typedef struct
{
    float humi;
    float temp;
} aht10_data_t;

enum
{
    LCD_SET_LIGHT,
    LCD_GET_SIZE,
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

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*PAL_H*/