#include "pm_anim.h"
#include "lvgl.h"
#define ANIM_TIME 300
static void anim_exec_pos_y_cb(void *ctx, int32_t y)
{
    lv_obj_t* obj=(lv_obj_t*)ctx;
    lv_obj_set_y(obj,y);
}
void pm_anim_over_top_to_buttom(lv_obj_t* obj,void* cb)
{
    lv_obj_update_layout(obj);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a,-lv_obj_get_height(obj),0);
    lv_anim_set_time(&a, ANIM_TIME);
    lv_anim_set_exec_cb(&a, anim_exec_pos_y_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
    lv_anim_set_ready_cb(&a,(lv_anim_ready_cb_t)cb);
    lv_anim_start(&a);
}
void pm_anim_over_buttom_to_top(lv_obj_t* obj,void* cb)
{
    lv_obj_update_layout(obj);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a,0,-lv_obj_get_width(obj));
    lv_anim_set_time(&a, ANIM_TIME);
    lv_anim_set_exec_cb(&a, anim_exec_pos_y_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
    lv_anim_set_ready_cb(&a,(lv_anim_ready_cb_t)cb);
    lv_anim_start(&a);
}