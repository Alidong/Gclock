/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

// This demo UI is adapted from LVGL official example: https://docs.lvgl.io/master/examples.html#scatter-chart

#include "lv_app.h"
#include "lvgl.h"
#include "esp_err.h"
#include "esp_log.h"
#include "device/lcd/lcd.h"
#include "device/board.h"
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/errno.h>
#include <sys/unistd.h>
#include <sys/select.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_vendor.h"
static const char* TAG="lv_port:";
static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
    // pass the draw buffer to the driver
    //esp_lcd_panel_draw_bitmap(lcd_panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
    lcd_flush(offsetx1, offsety1, offsetx2, offsety2, color_map);
    lv_disp_flush_ready(drv);
}
void lv_disp_init(void)
{
    static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
    static lv_disp_drv_t disp_drv;      // contains callback functions
    void *buf1 = NULL;
    ESP_LOGI(TAG, "Allocate separate LVGL draw buffers from PSRAM");
    buf1 = malloc(LCD_H_RES * LCD_V_RES * sizeof(lv_color_t));
    assert(buf1);
    void *buf2 = NULL;
    buf2 = malloc(LCD_H_RES * LCD_V_RES * sizeof(lv_color_t));
    assert(buf2);
    // initialize LVGL draw buffers
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, LCD_H_RES * LCD_V_RES);
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LCD_H_RES;
    disp_drv.ver_res = LCD_V_RES;
    disp_drv.flush_cb = lvgl_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    //disp_drv.user_data = lcd_panel_handle;
    lv_disp_drv_register(&disp_drv);
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
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &lvgl_tick,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, 1 * 1000));  
}
/*Will be called by the library to read the button*/
static void button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{

    static uint8_t last_btn = 0;

    /*Get the pressed button's ID*/
    uint8_t btn_act=0xFF;
    read(DEV->buttonHandle,&btn_act,sizeof(btn_act));
    if(btn_act != 0) {
        data->state = LV_INDEV_STATE_PR;
        last_btn = btn_act&0x01? 0:1;
        printf("lv:btn_act=%d\r\n",last_btn);
    }
    else {
        data->state = LV_INDEV_STATE_REL;
    }

    /*Save the last pressed button's ID*/
    data->btn_id = last_btn;
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
    indev_drv.type = LV_INDEV_TYPE_BUTTON;
    indev_drv.read_cb = button_read;
    lv_indev_t * indev_button = lv_indev_drv_register(&indev_drv);

    /*Assign buttons to points on the screen*/
    static const lv_point_t btn_points[2] = {
        {0, 0},   /*Button 0 -> x:10; y:10*/
        {0, 100},  /*Button 1 -> x:40; y:100*/
    };
    lv_indev_set_button_points(indev_button, btn_points);
}
void lv_port_init(void)
{
    lv_tick_init();
    lv_disp_init();
    lv_indev_init();
}