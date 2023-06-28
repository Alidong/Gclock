#ifndef _DEV_PAL_H
#define _DEV_PAL_H
#include <esp_types.h>
#include <esp_err.h>
#define INTERNAL_DISK   "/spiffs"
#define EXTERNAL_DISK   "/sdcard"
typedef struct
{
    float humi;
    float temp;
} aht10_data_t;

enum
{
    LCD_SET_LIGHT,
    LCD_GET_SIZE,
    LCD_SET_CB,
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

typedef void(*lcd_ready_cb_t)(void* ctx);
typedef struct 
{
    lcd_ready_cb_t cb;
    void* ctx;
}dev_lcd_flush_ready_cb_t;

#define KNOB_PRESS_MASK     (0x01<<(0))
#define KNOB_LEFT_MASK      (0x01<<(1))
#define KNOB_RIGHT_MASK     (0x01<<(2))
#define KNOB_LONG_PRESS_MASK (0x01<<(3))
#endif