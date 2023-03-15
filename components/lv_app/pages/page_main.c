#include "page.h"
#include "lvgl.h"
#include "font/lv_custom_font.h"
#ifndef USE_LV_SIMULATOR
#include "drivers/driver.h"
#include "drivers/aht10/aht10.h"
#include "drivers/bat/battery.h"
#include <time.h>
#include <sys/time.h>
#endif // USE_LV_SIMULATOR
page_node_t page_main;
static lv_obj_t *label1 = NULL;

#ifndef USE_LV_SIMULATOR
static void scan_sensor(struct _lv_timer_t *timer)
{
    aht10_data_t data;
    read(DRV->aht10Handle, &data, sizeof(aht10_data_t));
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    lv_label_set_text_fmt(label1, "%02d\n%02d\n%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}
#endif
static void onCreate(struct _page_node *page)            // 页面创建
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
    label1 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
    lv_obj_set_style_text_color(label1, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label1, &modenine_100, 0);
    lv_label_set_text(label1, "00\n00\n00");
    lv_obj_set_width(label1, LV_PCT(100));  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, 0);
#ifndef USE_LV_SIMULATOR
    lv_timer_create(scan_sensor, 1000, NULL);
#endif
}
void page_main_init()
{
    page_main.isReleased = true;
    page_main.onCreate = onCreate;
}