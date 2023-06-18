#include "lvgl.h"
#include "font/lv_custom_font.h"
#include "hal/lv_app_hal.h"
static lv_obj_t *label1 = NULL;
static lv_obj_t* pageTime = NULL;
static void timer(struct _lv_timer_t *timer)
{
    lv_app_hal_local_time_t timeinfo;
    lv_app_hal_get_local_time(&timeinfo);
    lv_label_set_text_fmt(label1, "%02d\n%02d\n%02d", timeinfo.hour, timeinfo.min, timeinfo.sec);
}
lv_obj_t* page_time_init(lv_obj_t* parent)            // 页面创建
{
    pageTime=lv_obj_create(parent);
    lv_obj_set_size(pageTime,LV_PCT(100),LV_PCT(100));
    lv_obj_set_style_bg_color(pageTime, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(pageTime, LV_ALIGN_CENTER,0, 0);


    // pageTest=lv_obj_create(lv_scr_act());
    // lv_obj_set_size(pageTest,LV_PCT(100),LV_PCT(100));
    // lv_obj_set_style_bg_color(pageTest, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_align(pageTest, LV_ALIGN_CENTER, 0, 0);

    label1 = lv_label_create(pageTime);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
    lv_obj_set_style_text_color(label1, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label1, &modenine_100, 0);
    lv_label_set_text(label1, "00\n00\n00");
    lv_obj_set_width(label1, LV_PCT(100));  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, -LV_DPX(5), LV_DPX(12));
    lv_timer_create(timer, 1000, NULL);
    return pageTime;
}