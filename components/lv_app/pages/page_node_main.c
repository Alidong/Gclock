#include "../pageManager/page_manager.h"
#include"icon/icons.h"
#include "lvgl.h"
#include "esp_log.h"
#include "hal/lv_app_hal.h"

page_node_t page1;
page_node_t page2;
page_node_t page3;
static void timer(lv_timer_t *timer)
{
    pm_stack_pop_page(NULL,PM_ANIM_OVER_BOTTOM_TO_TOP);
    lv_timer_pause(timer);
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
    lv_obj_set_width(label1,LV_SIZE_CONTENT);
    lv_label_set_text(label1, page->name);
    lv_obj_center(label1);
    lv_obj_t* btn=lv_btn_create(obj);
    lv_obj_align(btn,LV_ALIGN_LEFT_MID,0,0);
    lv_obj_set_size(btn,LV_DPX(48),LV_DPX(48));

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
    }
    else if(count==3)
    {
        pm_stack_replace_page("page3", PM_ANIM_PUSH_TOP_TO_BOTTOM);
    }
    else if(count==4)
    {
        count=0;
        pm_stack_replace_page(NULL,PM_ANIM_OVER_BOTTOM_TO_TOP);
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

    page2.onCreate = onCreate;
    page2.onRelease = onRelease;
    page2.onAppearing=onAppearing;
    page2.onDisappearing=onDisappearing;
    lv_snprintf(page2.name,PAGE_NAME_LEN,"page2");
    pm_register_page(&page2);

    page3.onCreate = onCreate;
    page3.onRelease = onRelease;
    page3.onAppearing=onAppearing;
    page3.onDisappearing=onDisappearing;
    lv_snprintf(page3.name,PAGE_NAME_LEN,"page3");
    pm_register_page(&page3);
    lv_timer_create(test, 2 * 1000, NULL);
}