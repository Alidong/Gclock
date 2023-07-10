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
    buf1 = malloc(lcd.width * lcd.height * sizeof(lv_color_t));
    assert(buf1);
    void *buf2 = NULL;
    buf2 = malloc(lcd.width *  lcd.height * sizeof(lv_color_t));
    assert(buf2);
    // initialize LVGL draw buffers
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, lcd.width * lcd.height);
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = lcd.width;
    disp_drv.ver_res = lcd.height;
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
        fcntl(DRV->ledHandle,LED_SCROLL_LEFT,NULL);
    }
    else if(knob&KNOB_RIGHT_MASK)
    {
       encoder++;
       fcntl(DRV->ledHandle,LED_SCROLL_RIGHT,NULL);
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



/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Open a file
 * @param drv       pointer to a driver where this function belongs
 * @param path      path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode      read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @return          a file descriptor or NULL on error
 */
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    lv_fs_res_t res = LV_FS_RES_FS_ERR;

    void * f = NULL;

    if(mode == LV_FS_MODE_WR) {
        /*Open a file for write*/
        f=fopen(path,"w");
    }
    else if(mode == LV_FS_MODE_RD) {
        /*Open a file for read*/
        f=fopen(path,"r");
    }
    else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) {
        /*Open a file for read and write*/
        f=fopen(path,"w+");
    }
    return f;
}

/**
 * Close an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable. (opened with fs_open)
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p)
{
    lv_fs_res_t res = LV_FS_RES_OK;

    fclose(file_p);
    return res;
}

/**
 * Read data from an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable.
 * @param buf       pointer to a memory block where to store the read data
 * @param btr       number of Bytes To Read
 * @param br        the real number of read bytes (Byte Read)
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    *br=fread(buf,1,btr,file_p);
    return LV_FS_RES_OK;
}

/**
 * Write into a file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable
 * @param buf       pointer to a buffer with the bytes to write
 * @param btw       Bytes To Write
 * @param bw        the number of real written bytes (Bytes Written). NULL if unused.
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    *bw=fwrite(buf,1,btw,file_p);
    return LV_FS_RES_OK;
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable. (opened with fs_open )
 * @param pos       the new position of read write pointer
 * @param whence    tells from where to interpret the `pos`. See @lv_fs_whence_t
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    uint32_t fsWhence=0;
    switch (whence)
    {
    case LV_FS_SEEK_SET:
        fsWhence=SEEK_SET;
        break;
    case LV_FS_SEEK_CUR:
        fsWhence=SEEK_CUR;
        break;
    case LV_FS_SEEK_END:
        fsWhence=SEEK_END;
        break;    
    default:
        return LV_FS_RES_UNKNOWN;
        break;
    }
    fseek(file_p,pos,fsWhence);
    return LV_FS_RES_OK;
}
/**
 * Give the position of the read write pointer
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable.
 * @param pos_p     pointer to to store the result
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    *pos_p=ftell(file_p);
    return LV_FS_RES_OK;
}

/**
 * Initialize a 'lv_fs_dir_t' variable for directory reading
 * @param drv       pointer to a driver where this function belongs
 * @param path      path to a directory
 * @return          pointer to the directory read descriptor or NULL on error
 */
static void * fs_dir_open(lv_fs_drv_t * drv, const char * path)
{
    void * dir = NULL;
    /*Add your code here*/
    dir=opendir(path);
    return dir;
}

/**
 * Read the next filename form a directory.
 * The name of the directories will begin with '/'
 * @param drv       pointer to a driver where this function belongs
 * @param rddir_p   pointer to an initialized 'lv_fs_dir_t' variable
 * @param fn        pointer to a buffer to store the filename
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * rddir_p, char * fn)
{
    lv_fs_res_t res=LV_FS_RES_UNKNOWN;
    struct dirent *entry;
    entry=readdir(rddir_p);
    if (entry)
    {
        sprintf(fn, entry->d_name);
        res=LV_FS_RES_OK;
    }
    else
    {
        res=LV_FS_RES_FS_ERR;
    }
    return res;
}

/**
 * Close the directory reading
 * @param drv       pointer to a driver where this function belongs
 * @param rddir_p   pointer to an initialized 'lv_fs_dir_t' variable
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * rddir_p)
{ 
    closedir(rddir_p);
    return LV_FS_RES_OK;
}
static void lv_fs_init(void)
{

    /*---------------------------------------------------
     * Register the file system interface in LVGL
     *--------------------------------------------------*/

    /*Add a simple drive to open images*/
    static lv_fs_drv_t fs_drv;
    lv_fs_drv_init(&fs_drv);

    /*Set up fields...*/
    fs_drv.letter = 'P';
    fs_drv.cache_size=1024*32;
    fs_drv.open_cb = fs_open;
    fs_drv.close_cb = fs_close;
    fs_drv.read_cb = fs_read;
    fs_drv.write_cb = fs_write;
    fs_drv.seek_cb = fs_seek;
    fs_drv.tell_cb = fs_tell;

    fs_drv.dir_close_cb = fs_dir_close;
    fs_drv.dir_open_cb = fs_dir_open;
    fs_drv.dir_read_cb = fs_dir_read;

    lv_fs_drv_register(&fs_drv);
}
void lv_port_init(void)
{
    lv_tick_init();
    lv_disp_init();
    lv_indev_init();
    lv_fs_init();
}