#include "lvgl.h"
#include "hal/lv_app_hal.h"
#include "font/lv_custom_font.h"
static lv_obj_t * meterTemp = NULL;
static lv_obj_t* pageEnv = NULL;

static void timer(struct _lv_timer_t *timer)
{
    float temp,humi;
    lv_app_hal_aht10_get(&temp,&humi);
    //lv_label_set_text_fmt(label1, "%.1f\n%.1f", temp, humi);
}

lv_obj_t* page_env_init(lv_obj_t* parent)            // 页面创建
{
    // lv_obj_t* preObj=lv_obj_get_child(parent,-1);
    // pageEnv=lv_obj_create(parent);
    // lv_obj_set_size(pageEnv,LV_PCT(100),LV_PCT(100));
    // lv_obj_set_style_bg_color(pageEnv, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_align_to(pageEnv,preObj ,LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    // meterTemp = lv_meter_create(pageEnv);
    // lv_obj_center(meterTemp);
    // lv_obj_set_size(meterTemp, LV_DPX(180), LV_DPX(180));

    // /*Add a scale first*/
    // lv_meter_set_scale_ticks(meterTemp, 40, 2, 8, lv_palette_main(LV_PALETTE_GREY));
    // lv_meter_set_scale_major_ticks(meterTemp, 5, 4, 12, lv_color_white(), 10);

    // lv_meter_indicator_t* indic;

    // /*Add a blue arc to the start*/
    // indic = lv_meter_add_arc(meterTemp, 3, lv_palette_main(LV_PALETTE_BLUE), 0);
    // lv_meter_set_indicator_start_value(meterTemp, indic, -10);
    // lv_meter_set_indicator_end_value(meterTemp, indic, 10);

    // /*Make the tick lines blue at the start of the scale*/
    // indic = lv_meter_add_scale_lines(meterTemp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_BLUE),
    //     false, 0);
    // lv_meter_set_indicator_start_value(meterTemp, indic, -10);
    // lv_meter_set_indicator_end_value(meterTemp, indic, 10);

    // /*Add a red arc to the end*/
    // indic = lv_meter_add_arc(meterTemp, 3, lv_palette_main(LV_PALETTE_RED), 0);
    // lv_meter_set_indicator_start_value(meterTemp, indic, 30);
    // lv_meter_set_indicator_end_value(meterTemp, indic, 40);

    // /*Make the tick lines red at the end of the scale*/
    // indic = lv_meter_add_scale_lines(meterTemp, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false,
    //     0);
    // lv_meter_set_indicator_start_value(meterTemp, indic, 30);
    // lv_meter_set_indicator_end_value(meterTemp, indic, 40);
    // lv_meter_set_scale_range(meterTemp,-10,40,270,135);
    // /*Add a needle line indicator*/
    // indic = lv_meter_add_needle_line(meterTemp, 4, lv_palette_main(LV_PALETTE_GREY), -10);
    return pageEnv;
}