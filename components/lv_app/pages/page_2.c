#include "../pageManager/page_manager.h"
#include"icon/icons.h"
#include "lvgl.h"
//#include "esp_log.h"
#include "pal.h"

#include "hal/lv_app_hal.h"

static page_node_t page2;
static void onCreate(page_node_t *page)            // 页面创建
{
    //top bar
    lv_obj_t* obj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj,LV_PCT(100),LV_PCT(100));
    // lv_obj_set_style_bg_img_src(obj,&IMG_CLOCK,0);
    lv_obj_center(obj);

    lv_obj_t* img = lv_gif_create(obj);
    /* Assuming a File system is attached to letter 'A'
     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    lv_gif_set_src(img, "P:"EXTERNAL_DISK"/bulb.gif");
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
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
        // lv_obj_clear_flag(child,LV_OBJ_FLAG_HIDDEN);
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
    // uint16_t count=lv_obj_get_child_cnt(page->obj);
    // for (size_t i = 0; i < count; i++)
    // {
    //     lv_obj_t* child=lv_obj_get_child(page->obj,i);
    //     // lv_obj_clear_flag(child,LV_OBJ_FLAG_HIDDEN);
    //     lv_anim_t a;
    //     lv_anim_init(&a);
    //     lv_anim_set_var(&a, child);
    //     lv_anim_set_values(&a,lv_obj_get_height(child),1);
    //     lv_anim_set_time(&a, 300);
    //     lv_anim_set_exec_cb(&a, pageAnim);
    //     lv_anim_set_delay(&a,i*50);
    //     lv_anim_timeline_add(animLine,delay,&a);
    // }
}

static void onRelease(page_node_t *page)            // 页面销毁
{
    lv_obj_del(page->obj);
}
void page2_init()
{
    page2.onCreate = onCreate;
    page2.onRelease = onRelease;
    // page2.onAppearing=onAppearing;
    // page2.onDisappearing=onDisappearing;
    lv_snprintf(page2.name,PAGE_NAME_LEN,"page2");
    pm_register_page(&page2);
}