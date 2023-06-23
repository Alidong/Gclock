#include "pm_anim.h"
#include "lvgl.h"
#define ANIM_TIME 300
#define ANIM_POS_PATH lv_anim_path_ease_in
#define ANIM_SIZE_PATH lv_anim_path_ease_in
typedef struct _pm_anim
{
    page_node_t* origin;
    page_node_t* new;
    anim_done_cb_t cb;
    void* ctx;
}pm_anim_ctrl_t;
static pm_anim_ctrl_t st_anim_ctrl;
static void anim_reset(void)
{
    st_anim_ctrl.cb=NULL;
    st_anim_ctrl.ctx=NULL;
    st_anim_ctrl.new=NULL;
    st_anim_ctrl.origin=NULL;
}
static void anim_notify(lv_anim_t* anim)
{
    if (st_anim_ctrl.cb)
    {
        st_anim_ctrl.cb(st_anim_ctrl.ctx);
    }
    anim_reset();
}
static void anim_exec_pos_y_cb(void *ctx, int32_t y)
{
    lv_obj_t* obj=(lv_obj_t*)ctx;
    lv_obj_set_y(obj,y);
}
void pm_anim_over_pos_y(page_node_t* page,anim_done_cb_t cb,void* ctx,int32_t start,int32_t end)
{
    st_anim_ctrl.cb=cb;
    st_anim_ctrl.ctx=ctx;
    lv_obj_t* obj=page->obj;
    lv_obj_update_layout(obj);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a,start,end);
    lv_anim_set_time(&a, ANIM_TIME);
    lv_anim_set_exec_cb(&a, anim_exec_pos_y_cb);
    lv_anim_set_path_cb(&a, ANIM_POS_PATH);
    lv_anim_set_ready_cb(&a,anim_notify);
    lv_anim_start(&a);
}
void pm_anim_over_top_to_buttom(page_node_t* page,anim_done_cb_t cb,void* ctx)
{
    st_anim_ctrl.cb=cb;
    st_anim_ctrl.ctx=ctx;
    lv_obj_t* obj=page->obj;
    lv_obj_update_layout(obj);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a,-lv_obj_get_height(lv_scr_act()),0);
    lv_anim_set_time(&a, ANIM_TIME);
    lv_anim_set_exec_cb(&a, anim_exec_pos_y_cb);
    lv_anim_set_path_cb(&a, ANIM_POS_PATH);
    lv_anim_set_ready_cb(&a,anim_notify);
    lv_anim_start(&a);
}
void pm_anim_over_buttom_to_top(page_node_t* page,anim_done_cb_t cb,void* ctx)
{
    st_anim_ctrl.cb=cb;
    st_anim_ctrl.ctx=ctx;
    lv_obj_t* obj=page->obj;
    lv_obj_update_layout(obj);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a,lv_obj_get_height(lv_scr_act()),0);
    lv_anim_set_time(&a, ANIM_TIME);
    lv_anim_set_exec_cb(&a, anim_exec_pos_y_cb);
    lv_anim_set_path_cb(&a, ANIM_POS_PATH);
    lv_anim_set_ready_cb(&a,anim_notify);
    lv_anim_start(&a);
}
static void anim_exec_pos_x_cb(void *ctx, int32_t x)
{
    page_node_t* obj=(page_node_t*)ctx;
    lv_obj_set_x(obj,x);
}
void pm_anim_over_pos_x(page_node_t* page,anim_done_cb_t cb,void* ctx,int32_t start,int32_t end)
{
    st_anim_ctrl.cb=cb;
    st_anim_ctrl.ctx=ctx;
    lv_obj_t* obj=page->obj;
    lv_obj_update_layout(obj);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a,start,end);
    lv_anim_set_time(&a, ANIM_TIME);
    lv_anim_set_exec_cb(&a, anim_exec_pos_x_cb);
    lv_anim_set_path_cb(&a, ANIM_POS_PATH);
    lv_anim_set_ready_cb(&a,anim_notify);
    lv_anim_start(&a);
}
void pm_anim_over_left_to_right(page_node_t* page,anim_done_cb_t cb,void* ctx)
{
    st_anim_ctrl.cb=cb;
    st_anim_ctrl.ctx=ctx;
    lv_obj_t* obj=page->obj;
    lv_obj_update_layout(obj);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a,-lv_obj_get_width(lv_scr_act()),0);
    lv_anim_set_time(&a, ANIM_TIME);
    lv_anim_set_exec_cb(&a, anim_exec_pos_x_cb);
    lv_anim_set_path_cb(&a, ANIM_POS_PATH);
    lv_anim_set_ready_cb(&a,anim_notify);
    lv_anim_start(&a);
}
void pm_anim_over_right_to_left(page_node_t* page,anim_done_cb_t cb,void* ctx)
{
    st_anim_ctrl.cb=cb;
    st_anim_ctrl.ctx=ctx;
    lv_obj_t* obj=page->obj;
    lv_obj_update_layout(obj);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a,lv_obj_get_width(lv_scr_act()),0);
    lv_anim_set_time(&a, ANIM_TIME);
    lv_anim_set_exec_cb(&a, anim_exec_pos_x_cb);
    lv_anim_set_path_cb(&a, ANIM_POS_PATH);
    lv_anim_set_ready_cb(&a,anim_notify);
    lv_anim_start(&a);
}
static void anim_exec_push_pos_y_cb(void *ctx, int32_t y)
{
    lv_obj_set_y(st_anim_ctrl.new->obj,y);
    if (st_anim_ctrl.origin)
    {
        int16_t start=(int16_t)ctx;
        lv_obj_set_y(st_anim_ctrl.origin->obj,y-start);
    }
}
void pm_anim_push_pos_y(page_node_t* origin,page_node_t* new,void* cb,void* ctx,int32_t start,int32_t end)
{
    st_anim_ctrl.cb=cb;
    st_anim_ctrl.ctx=ctx;
    st_anim_ctrl.origin=origin;
    st_anim_ctrl.new=new;
    lv_obj_t* obj=new->obj;
    lv_obj_update_layout(obj);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, (void*)((int16_t)start));
    lv_anim_set_values(&a,start,end);
    lv_anim_set_time(&a, ANIM_TIME);
    lv_anim_set_exec_cb(&a, anim_exec_push_pos_y_cb);
    lv_anim_set_path_cb(&a, ANIM_POS_PATH);
    lv_anim_set_ready_cb(&a,anim_notify);
    lv_anim_start(&a);
}
void pm_anim_push_top_to_buttom(page_node_t* origin,page_node_t* new,void* cb,void* ctx)
{

    st_anim_ctrl.cb=cb;
    st_anim_ctrl.ctx=ctx;
    st_anim_ctrl.origin=origin;
    st_anim_ctrl.new=new;
    lv_obj_t* obj=new->obj;
    lv_obj_update_layout(obj);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, (void*)((int16_t)1));
    lv_anim_set_values(&a,-lv_obj_get_height(lv_scr_act()),0);
    lv_anim_set_time(&a, ANIM_TIME);
    lv_anim_set_exec_cb(&a, anim_exec_push_pos_y_cb);
    lv_anim_set_path_cb(&a, ANIM_POS_PATH);
    lv_anim_set_ready_cb(&a,anim_notify);
    lv_anim_start(&a);
}
void pm_anim_push_buttom_to_top(page_node_t* origin,page_node_t* new,void* cb,void* ctx)
{

    st_anim_ctrl.cb=cb;
    st_anim_ctrl.ctx=ctx;
    st_anim_ctrl.origin=origin;
    st_anim_ctrl.new=new;
    lv_obj_t* obj=new->obj;
    lv_obj_update_layout(obj);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, (void*)((int16_t)-1));
    lv_anim_set_values(&a,lv_obj_get_height(lv_scr_act()),0);
    lv_anim_set_time(&a, ANIM_TIME);
    lv_anim_set_exec_cb(&a, anim_exec_push_pos_y_cb);
    lv_anim_set_path_cb(&a, ANIM_POS_PATH);
    lv_anim_set_ready_cb(&a,anim_notify);
    lv_anim_start(&a);
}
static void anim_exec_push_pos_x_cb(void *ctx, int32_t x)
{
    lv_obj_set_x(st_anim_ctrl.new->obj,x);
    if (st_anim_ctrl.origin)
    {
        int16_t start=(int16_t)ctx;
        lv_obj_set_x(st_anim_ctrl.origin->obj,x-start);
    }
}
void pm_anim_push_pos_x(page_node_t* origin,page_node_t* new,void* cb,void* ctx,int32_t start,int32_t end)
{

    st_anim_ctrl.cb=cb;
    st_anim_ctrl.ctx=ctx;
    st_anim_ctrl.origin=origin;
    st_anim_ctrl.new=new;
    lv_obj_t* obj=new->obj;
    lv_obj_update_layout(obj);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, (void*)((int16_t)start));
    lv_anim_set_values(&a,start,end);
    lv_anim_set_time(&a, ANIM_TIME);
    lv_anim_set_exec_cb(&a, anim_exec_push_pos_x_cb);
    lv_anim_set_path_cb(&a, ANIM_POS_PATH);
    lv_anim_set_ready_cb(&a,anim_notify);
    lv_anim_start(&a);
}
void pm_anim_push_left_to_right(page_node_t* origin,page_node_t* new,void* cb,void* ctx)
{

    st_anim_ctrl.cb=cb;
    st_anim_ctrl.ctx=ctx;
    st_anim_ctrl.origin=origin;
    st_anim_ctrl.new=new;
    lv_obj_t* obj=new->obj;
    lv_obj_update_layout(obj);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, (void*)((int16_t)1));
    lv_anim_set_values(&a,-lv_obj_get_width(lv_scr_act()),0);
    lv_anim_set_time(&a, ANIM_TIME);
    lv_anim_set_exec_cb(&a, anim_exec_push_pos_x_cb);
    lv_anim_set_path_cb(&a, ANIM_POS_PATH);
    lv_anim_set_ready_cb(&a,anim_notify);
    lv_anim_start(&a);
}
void pm_anim_push_right_to_left(page_node_t* origin,page_node_t* new,void* cb,void* ctx)
{

    st_anim_ctrl.cb=cb;
    st_anim_ctrl.ctx=ctx;
    st_anim_ctrl.origin=origin;
    st_anim_ctrl.new=new;
    lv_obj_t* obj=new->obj;
    lv_obj_update_layout(obj);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, (void*)((int16_t)-1));
    lv_anim_set_values(&a,lv_obj_get_width(lv_scr_act()),0);
    lv_anim_set_time(&a, ANIM_TIME);
    lv_anim_set_exec_cb(&a, anim_exec_push_pos_x_cb);
    lv_anim_set_path_cb(&a, ANIM_POS_PATH);
    lv_anim_set_ready_cb(&a,anim_notify);
    lv_anim_start(&a);
}