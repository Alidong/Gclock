/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

// This demo UI is adapted from LVGL official example: https://docs.lvgl.io/master/examples.html#scatter-chart

#include "lv_app.h"
#include "lvgl.h"
#include "pal.h"
#include "esp_err.h"
#include "esp_log.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
static const char *TAG = "lv_port:";
uint32_t dispHZ=50;
static lv_disp_t * disp;
static void lv_flush_done_cb(void* ctx)
{
    lv_disp_drv_t* drv=(lv_disp_drv_t*)ctx;
    lv_disp_flush_ready(drv);
}
static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    // pass the draw buffer to the driver
    //esp_lcd_panel_draw_bitmap(lcd_panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
    dev_lcd_flush_t lcd =
    {
        .xStart = area->x1,
        .xEnd = area->x2,
        .yStart = area->y1,
        .yEnd = area->y2,
        .buf = color_map
    };
    write(DRV->lcdHandle, &lcd, sizeof(dev_lcd_flush_t));
}
void lv_disp_init(void)
{
    static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
    static lv_disp_drv_t disp_drv;      // contains callback functions
    dev_lcd_pix_t lcd;
    fcntl(DRV->lcdHandle, LCD_GET_SIZE,(uint32_t)&lcd);
    dev_lcd_flush_ready_cb_t flushCB=
    {
        .cb=lv_flush_done_cb,
        .ctx=&disp_drv,
    };
    fcntl(DRV->lcdHandle, LCD_SET_CB,(uint32_t)&flushCB);
    ESP_LOGI(TAG, "Allocate separate LVGL draw buffers from PSRAM");
    void *buf1 = NULL;
    buf1 = malloc(lcd.width * lcd.height/2 * sizeof(lv_color_t));
    assert(buf1);
    void *buf2 = NULL;
    buf2 = malloc(lcd.width *  lcd.height/2 * sizeof(lv_color_t));
    assert(buf2);
    // initialize LVGL draw buffers
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, lcd.width * lcd.height/2);
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = lcd.width;
    disp_drv.ver_res = lcd.height;
    disp_drv.flush_cb = lvgl_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    //disp_drv.user_data = lcd_panel_handle;
    disp=lv_disp_drv_register(&disp_drv);
}
static void lvgl_tick(void *arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(1);
}
void lv_tick_init(void)
{
    ESP_LOGI(TAG, "Install LVGL tick timer");
    // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
    const esp_timer_create_args_t lvgl_tick_timer_args =
    {
        .callback = &lvgl_tick,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, 1 * 1000));
}

/*Will be called by the library to read the encoder*/
static void knob_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{

    static int32_t encoder=0;
    uint32_t knob;
    read(DRV->knobHandle,&knob,sizeof(knob));
    if (knob&KNOB_LONG_PRESS_MASK)
    {
        data->state = LV_INDEV_STATE_PRESSED;
    }
    else if(knob&KNOB_LEFT_MASK)
    {
        encoder--;
        if (dispHZ>1)
        {
            dispHZ--;
            disp->refr_timer->period=(1000/dispHZ);
        }
        
    }
    else if(knob&KNOB_RIGHT_MASK)
    {
       encoder++;
        if (dispHZ<50)
        {
            dispHZ++;
            disp->refr_timer->period=(1000/dispHZ);
        }
    }
    else
    {
        data->state=LV_INDEV_STATE_RELEASED;
    }
    data->enc_diff=encoder;
}

static void lv_indev_init(void)
{
    /**
     * Here you will find example implementation of input devices supported by LittelvGL:
     *  - Touchpad
     *  - Mouse (with cursor support)
     *  - Keypad (supports GUI usage only with key)
     *  - Encoder (supports GUI usage only with: left, right, push)
     *  - Button (external buttons to press points on the screen)
     *
     *  The `..._read()` function are only examples.
     *  You should shape them according to your hardware
     */
    /*Register a button input device*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv.read_cb = knob_read;
    lv_indev_t *indev = lv_indev_drv_register(&indev_drv);
    /*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     *add objects to the group with `lv_group_add_obj(group, obj)`
     *and assign this input device to group to navigate in it:
     *`lv_indev_set_group(indev_encoder, group);`*/
    // lv_indev_set_group(indev, lv_group_get_default());
}
void lv_port_init(void)
{
    lv_tick_init();
    lv_disp_init();
    lv_indev_init();
}