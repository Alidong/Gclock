#include "page.h"
#include "lvgl.h"
#ifndef USE_LV_SIMULATOR
#include "drivers/driver.h"
#include "drivers/wifi/wifi.h"
#include "drivers/bat/battery.h"
#endif // USE_LV_SIMULATOR
#define ANIM_TIME 500
page_node_t page_bar;
static lv_timer_t *timerHandle;
static lv_obj_t *topBar = NULL;
static lv_obj_t *buttomBar = NULL;
static lv_obj_t *leftBtn;
static lv_obj_t *rightBtn;

static void timer(struct _lv_timer_t *timer)
{
    page_pop(&page_bar);
    lv_timer_pause(timerHandle);
}
static void btn_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        LV_LOG_USER("Clicked");
        lv_timer_reset(timerHandle);
        if (lv_event_get_current_target(e) == leftBtn)
        {
            printf("leftBtn Clicked\n");
        }
        else
        {
            printf("rightBtn Clicked\n");
        }
    }
}
static void onCreate(struct _page_node *page)            // 页面创建
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
    //top bar
    topBar = lv_obj_create(lv_layer_top());
    lv_obj_set_size(topBar, LV_PCT(100), LV_PCT(10));
    lv_obj_set_align(topBar, LV_ALIGN_TOP_MID);
    lv_obj_set_style_bg_color(topBar, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(topBar, LV_OPA_30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(topBar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *label1 = lv_label_create(topBar);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_SCROLL_CIRCULAR);     /*Circular scroll*/
    lv_obj_set_width(label1, LV_PCT(30));
    lv_label_set_text(label1, "top bar..........");
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, 0);
    //buttom bar
    buttomBar = lv_obj_create(lv_layer_top());
    lv_obj_set_size(buttomBar, LV_PCT(100), LV_PCT(10));
    lv_obj_set_align(buttomBar, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_style_bg_color(buttomBar, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(buttomBar, LV_OPA_30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(buttomBar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_update_layout(lv_layer_top());
    //left btn
    leftBtn = lv_btn_create(buttomBar);
    lv_obj_set_style_bg_color(leftBtn, lv_palette_main(LV_PALETTE_ORANGE), 0);
    lv_obj_set_style_radius(leftBtn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_size(leftBtn, lv_obj_get_height(buttomBar) / 4, lv_obj_get_height(buttomBar) / 4);
    lv_obj_align(leftBtn, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_add_event_cb(leftBtn, btn_event_handler, LV_EVENT_ALL, NULL);
    //right btn
    rightBtn = lv_btn_create(buttomBar);
    lv_obj_set_style_bg_color(rightBtn, lv_palette_main(LV_PALETTE_ORANGE), 0);
    lv_obj_set_style_radius(rightBtn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_size(rightBtn, lv_obj_get_height(buttomBar) / 4, lv_obj_get_height(buttomBar) / 4);
    lv_obj_align(rightBtn, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_event_cb(rightBtn, btn_event_handler, LV_EVENT_ALL, NULL);

    // lv_obj_t * label2 = lv_label_create(buttomBar);
    // lv_label_set_long_mode(label2, LV_LABEL_LONG_SCROLL_CIRCULAR);     /*Circular scroll*/
    // lv_obj_set_width(label2, LV_PCT(30));
    // lv_label_set_text(label2, "buttom bar bar..........");
    // lv_obj_align(label2, LV_ALIGN_CENTER, 0, 0);

    //
    timerHandle = lv_timer_create(timer, 3 * 1000, NULL);

}
static void anim_y_cb(void *var, int32_t v)
{
    lv_obj_set_y(var, v);
}
static void onAppearing(struct _page_node *page)
{
    // lv_obj_clear_flag(topBar,LV_OBJ_FLAG_HIDDEN);
    // lv_obj_clear_flag(buttomBar,LV_OBJ_FLAG_HIDDEN);
    lv_timer_resume(timerHandle);
    lv_timer_reset(timerHandle);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, topBar);
    lv_anim_set_values(&a, -lv_obj_get_height(topBar), 0);
    lv_anim_set_time(&a, ANIM_TIME);
    lv_anim_set_exec_cb(&a, anim_y_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);

    lv_anim_t b;
    lv_anim_init(&b);
    lv_anim_set_var(&b, buttomBar);
    lv_anim_set_values(&b, lv_obj_get_height(buttomBar), 0);
    lv_anim_set_time(&b, ANIM_TIME);
    lv_anim_set_exec_cb(&b, anim_y_cb);
    lv_anim_set_path_cb(&b, lv_anim_path_ease_out);
    lv_anim_start(&b);
    lv_obj_fade_in(topBar, ANIM_TIME, 0);
    lv_obj_fade_in(buttomBar, ANIM_TIME, 0);
}
static void onDisappearing(struct _page_node *page)
{
    // lv_obj_add_flag(topBar,LV_OBJ_FLAG_HIDDEN);
    // lv_obj_add_flag(buttomBar,LV_OBJ_FLAG_HIDDEN);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, topBar);
    lv_anim_set_values(&a, 0, -lv_obj_get_height(topBar));
    lv_anim_set_time(&a, ANIM_TIME);
    lv_anim_set_exec_cb(&a, anim_y_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);

    lv_anim_t b;
    lv_anim_init(&b);
    lv_anim_set_var(&b, buttomBar);
    lv_anim_set_values(&b, 0, lv_obj_get_height(buttomBar));
    lv_anim_set_time(&b, ANIM_TIME);
    lv_anim_set_exec_cb(&b, anim_y_cb);
    lv_anim_set_path_cb(&b, lv_anim_path_ease_out);
    lv_anim_start(&b);
    lv_obj_fade_out(topBar, ANIM_TIME, 0);
    lv_obj_fade_out(buttomBar, ANIM_TIME, 0);
}
void page_bar_init()
{
    page_bar.isReleased = true;
    page_bar.onCreate = onCreate;
    page_bar.onAppearing = onAppearing;
    page_bar.onDisappearing = onDisappearing;
}