#include "../pageManager/page_manager.h"
#include"icon/icons.h"
#include "lvgl.h"
#include "esp_log.h"
#include "hal/lv_app_hal.h"

static page_node_t page3;
static lv_obj_t * meter;

static void set_value(void * indic, int32_t v)
{
    lv_meter_set_indicator_end_value(meter, indic, v);
}
static void onCreate(page_node_t *page)            // 页面创建
{
    //top bar
    lv_obj_t* obj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj,LV_PCT(100),LV_PCT(100));
    // lv_obj_set_style_bg_img_src(obj,&IMG_CLOCK,0);
    lv_obj_center(obj);

    meter = lv_meter_create(obj);
    lv_obj_center(meter);
    lv_obj_set_size(meter, 170, 170);

    /*Remove the circle from the middle*/
    lv_obj_remove_style(meter, NULL, LV_PART_INDICATOR);

    /*Add a scale first*/
    lv_meter_scale_t * scale = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale, 11, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter, scale, 1, 2, 30, lv_color_hex3(0xeee), 15);
    lv_meter_set_scale_range(meter, scale, 0, 100, 270, 90);

    /*Add a three arc indicator*/
    lv_meter_indicator_t * indic1 = lv_meter_add_arc(meter, scale, 10, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_indicator_t * indic2 = lv_meter_add_arc(meter, scale, 10, lv_palette_main(LV_PALETTE_GREEN), -10);
    lv_meter_indicator_t * indic3 = lv_meter_add_arc(meter, scale, 10, lv_palette_main(LV_PALETTE_BLUE), -20);

    /*Create an animation to set the value*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, set_value);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_repeat_delay(&a, 100);
    lv_anim_set_playback_delay(&a, 100);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);

    lv_anim_set_time(&a, 2000);
    lv_anim_set_playback_time(&a, 500);
    lv_anim_set_var(&a, indic1);
    lv_anim_start(&a);

    lv_anim_set_time(&a, 1000);
    lv_anim_set_playback_time(&a, 1000);
    lv_anim_set_var(&a, indic2);
    lv_anim_start(&a);

    lv_anim_set_time(&a, 1000);
    lv_anim_set_playback_time(&a, 2000);
    lv_anim_set_var(&a, indic3);
    lv_anim_start(&a);
    page->obj=obj;
}
static void pageAnim(void* ctx, int32_t h)
{
    lv_obj_set_height(ctx,h);
}
static void onAppearing(page_node_t *page)
{
    uint16_t count=lv_obj_get_child_cnt(page->obj);
    for (size_t i = 0; i < count; i++)
    {
        lv_obj_t* child=lv_obj_get_child(page->obj,i);
        // lv_obj_clear_flag(child,LV_OBJ_FLAG_HIDDEN);
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, child);
        lv_anim_set_values(&a,0,lv_obj_get_height(child));
        lv_anim_set_time(&a, 300);
        lv_anim_set_exec_cb(&a, pageAnim);
        lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
        lv_anim_start(&a);
    }
}
static void onDisappearing(page_node_t *page)
{
    uint16_t count=lv_obj_get_child_cnt(page->obj);
    for (size_t i = 0; i < count; i++)
    {
        lv_obj_t* child=lv_obj_get_child(page->obj,i);
        // lv_obj_clear_flag(child,LV_OBJ_FLAG_HIDDEN);
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, child);
        lv_anim_set_values(&a,0,300);
        lv_anim_set_time(&a, 300);
        lv_anim_set_exec_cb(&a, pageAnim);
        lv_anim_set_delay(&a,i*50);
        lv_anim_start(&a);
    }
}

static void onRelease(page_node_t *page)            // 页面销毁
{
    lv_obj_del(page->obj);
    lv_anim_del(NULL,set_value);
}
void page3_init()
{
    page3.onCreate = onCreate;
    page3.onRelease = onRelease;
    page3.onAppearing=onAppearing;
    // page3.onDisappearing=onDisappearing;
    lv_snprintf(page3.name,PAGE_NAME_LEN,"page3");
    pm_register_page(&page3);
}