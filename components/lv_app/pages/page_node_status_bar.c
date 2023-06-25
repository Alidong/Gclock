#include "../pageManager/page_manager.h"
#include "lvgl.h"

#include "hal/lv_app_hal.h"

static const char* BAT_SYMBOL[]={
    "#ff0000 "LV_SYMBOL_BATTERY_EMPTY"#",
    "#ff6600 "LV_SYMBOL_BATTERY_1"#",
    LV_SYMBOL_BATTERY_2,
    LV_SYMBOL_BATTERY_3,
    LV_SYMBOL_BATTERY_FULL,
   "#0aeb2f "LV_SYMBOL_CHARGE"#",
};

static const char* WDAY_STR[7]={
    "Sun",
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat",
};

page_node_t top_status_bar;

static const char* bat_to_symbol();
static void timer(lv_timer_t *timer)
{
    pm_stack_pop_page(NULL,PM_ANIM_OVER_BOTTOM_TO_TOP);
    lv_timer_pause(timer);
}

static void onCreate(page_node_t *page)            // 页面创建
{
    //top bar
    lv_obj_t* topBar = lv_obj_create(lv_layer_top());
    lv_obj_set_size(topBar, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_align(topBar, LV_ALIGN_TOP_MID);
    lv_obj_set_style_bg_opa(topBar, LV_OPA_0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_state(topBar,LV_STATE_FOCUSED|LV_STATE_SCROLLED);
    lv_obj_clear_flag(topBar,LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t* label1 = lv_label_create(topBar);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_SCROLL_CIRCULAR);     /*Circular scroll*/
    lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
    lv_obj_set_style_text_color(label1, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_width(label1,LV_SIZE_CONTENT);
    lv_label_set_text(label1, "02-05 Sun");
    lv_obj_align(label1, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t* label2= lv_label_create(topBar);
    lv_label_set_long_mode(label2, LV_LABEL_LONG_SCROLL_CIRCULAR);     /*Circular scroll*/
    lv_label_set_recolor(label2, true);                      /*Enable re-coloring by commands in the text*/
    lv_obj_set_style_text_color(label2, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(label2,BAT_SYMBOL[5]);
    lv_obj_align(label2, LV_ALIGN_RIGHT_MID, 0, 0);
    page->timerHandler=lv_timer_create(timer, 3 * 1000, NULL);
    page->obj=topBar;
}
static void onRelease(page_node_t *page)            // 页面销毁
{
    lv_timer_del(page->timerHandler);
    lv_obj_del(page->obj);
}
static const char* bat_to_symbol()
{
    if (lv_app_hal_battery_is_charging())
    {
        return BAT_SYMBOL[5];
    }
    else
    {
        return BAT_SYMBOL[(lv_app_hal_battery_get_percent()+24)/25];
    }
}
static void test(lv_timer_t* timer)
{
    pm_stack_push_page("StatusBar", PM_ANIM_OVER_TOP_TO_BOTTOM);
}
void page_top_bar_init()
{
    top_status_bar.onCreate = onCreate;
    top_status_bar.onRelease = onRelease;
    lv_snprintf(top_status_bar.name,PAGE_NAME_LEN,"StatusBar");
    pm_register_page(&top_status_bar);
    pm_stack_push_page("StatusBar",PM_ANIM_OVER_TOP_TO_BOTTOM);
    lv_timer_create(test, 5 * 1000, NULL);
}