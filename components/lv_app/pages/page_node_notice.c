#include "page_node/page_node.h"
#include "lvgl.h"
#include "hal/lv_app_hal.h"
#define ANIM_TIME 300
page_node_t page_node_notice;
static lv_obj_t *noticePage = NULL;
static lv_obj_t *label1 =NULL;
static lv_timer_t* delayTimer;

static void delay(struct _lv_timer_t *timer)
{
    page_node_pop(&page_node_notice);
    lv_timer_pause(delayTimer);
}
static void heart_beat(struct _lv_timer_t *timer)
{
    do
    {
        #ifndef USE_LV_SIMULATOR
        static bool noticeClear = false;
        #else
        static bool noticeClear = true;
        #endif // !USE_LV_SIMULATOR
        if(!lv_app_hal_wifi_is_ok())
        {
            lv_label_set_text(label1, "wifi connecting...");
            page_node_push(&page_node_notice);
            noticeClear =true;
            break;
        }
        else if(!lv_app_hal_ntp_is_ok())
        {
            lv_label_set_text(label1, "ntp sync...");
            page_node_push(&page_node_notice);
            noticeClear =true;
            break;
        }
        if (noticeClear)
        {
            noticeClear =false;
            lv_timer_reset(delayTimer);
            lv_timer_resume(delayTimer);
        }
    } while (false);
}

static void onCreate(struct _page_node *page)            // 页面创建
{
    lv_obj_set_style_bg_color(lv_layer_top(), lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
    //top bar
    noticePage = lv_obj_create(lv_layer_top());
    lv_obj_set_size(noticePage, LV_PCT(72), LV_PCT(30));
    lv_obj_align(noticePage, LV_ALIGN_CENTER,0,0);
    lv_obj_set_style_bg_color(noticePage, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_opa(noticePage, LV_OPA_30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(noticePage, LV_OBJ_FLAG_SCROLLABLE);
    label1 = lv_label_create(noticePage);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_SCROLL_CIRCULAR);     /*Circular scroll*/
    lv_obj_set_width(label1, LV_PCT(100));
    lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
    lv_obj_set_style_text_color(label1, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(label1, "notice page...");
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, 0);

    lv_timer_create(heart_beat, ANIM_TIME, NULL);
    delayTimer=lv_timer_create(delay, 1000, NULL);
    lv_timer_pause(delayTimer);

}
static void anim_cb(void *var, int32_t v)
{
    lv_obj_set_style_transform_height(noticePage,v,0);
}
static void onAppearing(struct _page_node *page)
{
    lv_obj_set_style_bg_opa(lv_layer_top(),LV_OPA_90,LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_fade_in(noticePage,ANIM_TIME,0);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, noticePage);
    lv_anim_set_values(&a, -100, 0);
    lv_anim_set_time(&a, ANIM_TIME);
    lv_anim_set_exec_cb(&a, anim_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);
}
static void onDisappearing(struct _page_node *page)
{
    lv_obj_set_style_bg_opa(lv_layer_top(),LV_OPA_0,LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_fade_out(noticePage,ANIM_TIME,0);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, noticePage);
    lv_anim_set_values(&a, 0, -100);
    lv_anim_set_time(&a, ANIM_TIME);
    lv_anim_set_exec_cb(&a, anim_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
    lv_anim_start(&a);
}
void page_node_notice_init()
{
    page_node_notice.isReleased = true;
    page_node_notice.onCreate = onCreate;
    page_node_notice.onAppearing = onAppearing;
    page_node_notice.onDisappearing = onDisappearing;
}