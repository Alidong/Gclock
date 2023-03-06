#include "page.h"
#include "lvgl.h"
#include "lvgl/examples/lv_examples.h"
#include "device/device.h"
#include "device/aht10/aht10.h"
#include "device/power/power.h"
page_node_t page_main;
static lv_obj_t * label1 = NULL;
static void scan_sensor(struct _lv_timer_t * timer)
{
    aht10_data_t data;
    read(DEV->aht10Handle,&data,sizeof(aht10_data_t));
    lv_label_set_text_fmt(label1, "#0000ff Tempreture:%d.%d# #ff00ff Humidity:%d.%d%%# #ff0000 of a# label, align the lines to the center "
                      "BAT Volt=%d%%.",data.temp/100,data.temp%100,data.humi/10,data.humi&10,power_get_bat_percent());
}
static void onCreate(struct _page_node *page)            // 页面创建
{
    label1 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
   
    lv_label_set_text(label1, "#0000ff Re-color# #ff00ff words# #ff0000 of a# label, align the lines to the center "
                      "and wrap long text automatically.");
    lv_obj_set_width(label1, 150);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, -40);

    lv_obj_t * label2 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label2, LV_LABEL_LONG_SCROLL_CIRCULAR);     /*Circular scroll*/
    lv_obj_set_width(label2, 150);
    lv_label_set_text(label2, "It is a circularly scrolling text. ");
    lv_obj_align(label2, LV_ALIGN_CENTER, 0, 40);
    lv_timer_create(scan_sensor,1000,NULL);
}
void page_main_init()
{
    page_main.onCreate=onCreate;
}