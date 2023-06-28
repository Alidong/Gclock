#include "../pageManager/page_manager.h"
#include"icon/icons.h"
#include "lvgl.h"
//#include "esp_log.h"
#include "hal/lv_app_hal.h"

static page_node_t page1;
static lv_style_t style_indic;
static void set_temp(void * bar, int32_t temp)
{
    lv_bar_set_value(bar, temp, LV_ANIM_ON);
}
static void onCreate(page_node_t *page)            // 页面创建
{
    //top bar
    lv_obj_t* obj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj,LV_PCT(100),LV_PCT(100));
    // lv_obj_set_style_bg_img_src(obj,&IMG_CLOCK,0);
    lv_obj_center(obj);
    lv_obj_t* label1 = lv_label_create(obj);
    // lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
    lv_obj_set_width(label1,LV_PCT(70));
    lv_label_set_text(label1, "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    lv_label_set_long_mode(label1,LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_align(label1,LV_ALIGN_TOP_MID,0,0);
    


    lv_style_init(&style_indic);
    lv_style_set_bg_opa(&style_indic, LV_OPA_COVER);
    lv_style_set_bg_color(&style_indic, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_bg_grad_color(&style_indic, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_bg_grad_dir(&style_indic, LV_GRAD_DIR_VER);

    lv_obj_t * bar = lv_bar_create(obj);
    lv_obj_add_style(bar, &style_indic, LV_PART_INDICATOR);
    lv_obj_set_size(bar, 20, 120);
    lv_obj_align(bar,LV_ALIGN_BOTTOM_MID,0,0);
    lv_bar_set_range(bar, -20, 120);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, set_temp);
    lv_anim_set_time(&a, 3000);
    lv_anim_set_playback_time(&a, 3000);
    lv_anim_set_var(&a, bar);
    lv_anim_set_values(&a, -20, 120);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
    page->obj=obj;
}
static void pageAnim(void* ctx, int32_t h)
{
    lv_obj_set_height(ctx,h);
}
static void onAppearing(page_node_t *page,uint32_t delay,void* animLine)
{
    uint16_t count=lv_obj_get_child_cnt(page->obj);
    for (size_t i = 0; i < count; i++)
    {
        lv_obj_t* child=lv_obj_get_child(page->obj,i);
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, child);
        lv_anim_set_values(&a,0,lv_obj_get_height(child));
        lv_anim_set_time(&a, 300);
        lv_anim_set_exec_cb(&a, pageAnim);
        lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
        lv_anim_timeline_add(animLine,delay,&a);
    }
}
static void onDisappearing(page_node_t *page,uint32_t delay,void* animLine)
{
    uint16_t count=lv_obj_get_child_cnt(page->obj);
    for (size_t i = 0; i < count; i++)
    {
        lv_obj_t* child=lv_obj_get_child(page->obj,i);
        // lv_obj_clear_flag(child,LV_OBJ_FLAG_HIDDEN);
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, child);
        lv_anim_set_values(&a,lv_obj_get_height(child),1);
        lv_anim_set_time(&a, 300);
        lv_anim_set_exec_cb(&a, pageAnim);
        lv_anim_set_delay(&a,i*50);
        lv_anim_timeline_add(animLine,delay,&a);
    }
}

static void onRelease(page_node_t *page)            // 页面销毁
{
    lv_anim_del(NULL,set_temp);
    lv_anim_del(NULL,pageAnim);
    lv_obj_del(page->obj);
}
static void test(lv_timer_t* timer)
{
    static uint8_t count=0;
    count++;
    if (count==1)
    {
        pm_stack_replace_page("page1", PM_ANIM_PUSH_TOP_TO_BOTTOM);
    }
    else if(count==2)
    {
        pm_stack_replace_page("page2", PM_ANIM_PUSH_TOP_TO_BOTTOM);
        count=0;
    }
}
void page_main_init()
{
    page1.onCreate = onCreate;
    page1.onRelease = onRelease;
    page1.onAppearing=onAppearing;
    page1.onDisappearing=onDisappearing;
    lv_snprintf(page1.name,PAGE_NAME_LEN,"page1");
    pm_register_page(&page1);
    lv_timer_create(test, 5 * 1000, NULL);
}