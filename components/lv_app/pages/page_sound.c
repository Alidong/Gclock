#include "lvgl.h"
#include "hal/lv_app_hal.h"
#define LINE_POINTS 64
#define HEIGHT 64
static lv_obj_t *lineObj = NULL;
static lv_obj_t* pageSound = NULL;
static lv_point_t linePoints[LINE_POINTS];
static void timer_update_points(struct _lv_timer_t *timer)
{
    uint8_t* fftPoints=lv_app_hal_fft();
    for (uint16_t i = 0; i < LINE_POINTS; i++)
    {
        linePoints[i].y = fftPoints[i];
    }
    lv_line_set_points(lineObj, linePoints, LINE_POINTS);     /*Set the points*/
}

lv_obj_t* line_create(lv_obj_t* parent)
{
    /*Create an array for the points of the line*/
    for (size_t i = 0; i < LINE_POINTS; i++)
    {
        linePoints[i].x = i;
        linePoints[i].y = 0;
    }
    /*Create style*/
    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 1);
    lv_style_set_line_color(&style_line, lv_palette_main(LV_PALETTE_LIGHT_GREEN));
    lv_style_set_line_rounded(&style_line, false);

    /*Create a line and apply the new style*/
    lv_obj_t * line;
    line = lv_line_create(parent);
    lv_line_set_y_invert(line,true);
    lv_line_set_points(line, linePoints, LINE_POINTS);     /*Set the points*/
    lv_obj_add_style(line, &style_line, 0);
    // lv_align(line);
    return line;
}
lv_obj_t* page_sound_init(lv_obj_t* parent)            // 页面创建
{
    lv_obj_t* preObj=lv_obj_get_child(parent,-1);
    pageSound=lv_obj_create(parent);
    lv_obj_set_size(pageSound,LV_PCT(100),LV_PCT(100));
    lv_obj_set_style_bg_color(pageSound, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align_to(pageSound,preObj ,LV_ALIGN_OUT_RIGHT_MID, 0, 0);

    //Create
    lineObj =line_create(pageSound);
    lv_obj_align(lineObj,LV_ALIGN_BOTTOM_MID,0,0);
    lv_timer_create(timer_update_points, 10, NULL);
    return pageSound;
}