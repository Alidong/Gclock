#include "pm_anim.h"
#include "lvgl.h"
#define ANIM_TIME 300
#define ANIM_POS_PATH lv_anim_path_ease_out
#define ANIM_SIZE_PATH lv_anim_path_ease_in
typedef struct _pm_anim
{
    lv_obj_t* driving;
    lv_obj_t* passive;
    lv_anim_timeline_t* animline;
}pm_anim_ctrl_t;
static pm_anim_ctrl_t st_anim_ctrl;

static void anim_reset(void)
{
    st_anim_ctrl.animline=NULL;
    st_anim_ctrl.driving=NULL;
    st_anim_ctrl.passive=NULL;
}
void pm_anim_set_timeline(void* timeline)
{
    if (timeline)
    {
        st_anim_ctrl.animline=(lv_anim_timeline_t*)timeline;
    }
}
void pm_anim_reset(void)
{
    anim_reset();
}
static void anim_exec_pos_y_cb(void *ctx, int32_t y)
{
    lv_obj_t* obj=(lv_obj_t*)ctx;
    lv_obj_set_y(obj,y);
}
void pm_anim_over_pos_y(void* page,int16_t start_y,int16_t end_y)
{
    lv_obj_t* obj=(lv_obj_t*)page;
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a,start_y,end_y);
    lv_anim_set_time(&a, ANIM_TIME);
    lv_anim_set_exec_cb(&a, anim_exec_pos_y_cb);
    lv_anim_set_path_cb(&a, ANIM_POS_PATH);
    uint32_t playtime=lv_anim_timeline_get_playtime(st_anim_ctrl.animline);
    lv_anim_timeline_add(st_anim_ctrl.animline,playtime,&a);
}
static void anim_exec_pos_x_cb(void *ctx, int32_t x)
{
    page_node_t* obj=(page_node_t*)ctx;
    lv_obj_set_x(obj,x);
}
void pm_anim_over_pos_x(void* page,int16_t start_x,int16_t end_x)
{
    lv_obj_t* obj=(lv_obj_t*)page;
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a,start_x,end_x);
    lv_anim_set_time(&a, ANIM_TIME);
    lv_anim_set_exec_cb(&a, anim_exec_pos_x_cb);
    lv_anim_set_path_cb(&a, ANIM_POS_PATH);
    uint32_t playtime=lv_anim_timeline_get_playtime(st_anim_ctrl.animline);
    lv_anim_timeline_add(st_anim_ctrl.animline,playtime,&a);
}
void pm_anim_over_pos_xy(void* page,int16_t start_x,int16_t end_x,int16_t start_y,int16_t end_y)
{
    lv_obj_t* obj=(lv_obj_t*)page;

    lv_anim_t ax;
    lv_anim_init(&ax);
    lv_anim_set_var(&ax, obj);
    lv_anim_set_values(&ax,start_x,end_x);
    lv_anim_set_time(&ax, ANIM_TIME);
    lv_anim_set_exec_cb(&ax, anim_exec_pos_x_cb);
    lv_anim_set_path_cb(&ax, ANIM_POS_PATH);

    lv_anim_t ay;
    lv_anim_init(&ay);
    lv_anim_set_var(&ay, obj);
    lv_anim_set_values(&ay,start_y,end_y);
    lv_anim_set_time(&ay, ANIM_TIME);
    lv_anim_set_exec_cb(&ay, anim_exec_pos_y_cb);
    lv_anim_set_path_cb(&ay, ANIM_POS_PATH);

    uint32_t playtime=lv_anim_timeline_get_playtime(st_anim_ctrl.animline);
    lv_anim_timeline_add(st_anim_ctrl.animline,playtime,&ax);
    lv_anim_timeline_add(st_anim_ctrl.animline,playtime,&ay);
}
static void anim_exec_push_pos_y_cb(void *ctx, int32_t y)
{

    lv_obj_set_y(st_anim_ctrl.driving,y);
    int16_t start=(int16_t)ctx;
    lv_obj_set_y(st_anim_ctrl.passive,y-start);
}
void pm_anim_push_pos_y(void* driving,void* passive,int16_t start_y,int16_t end_y)
{
    st_anim_ctrl.driving=(lv_obj_t*)driving;
    st_anim_ctrl.passive=(lv_obj_t*)passive;
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, (void*)((int16_t)start_y));
    lv_anim_set_values(&a,start_y,end_y);
    lv_anim_set_time(&a, ANIM_TIME);
    lv_anim_set_exec_cb(&a, anim_exec_push_pos_y_cb);
    lv_anim_set_path_cb(&a, ANIM_POS_PATH);
    uint32_t playtime=lv_anim_timeline_get_playtime(st_anim_ctrl.animline);
    lv_anim_timeline_add(st_anim_ctrl.animline,playtime,&a);
}

static void anim_exec_push_pos_x_cb(void *ctx, int32_t x)
{
    lv_obj_set_x(st_anim_ctrl.driving,x);
    int16_t start=(int16_t)ctx;
    lv_obj_set_x(st_anim_ctrl.passive,x-start);
}
void pm_anim_push_pos_x(void* driving,void* passive,int16_t start_x,int16_t end_x)
{
    st_anim_ctrl.driving=(lv_obj_t*)driving;
    st_anim_ctrl.passive=(lv_obj_t*)passive;
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, (void*)((int16_t)start_x));
    lv_anim_set_values(&a,start_x,end_x);
    lv_anim_set_time(&a, ANIM_TIME);
    lv_anim_set_exec_cb(&a, anim_exec_push_pos_x_cb);
    lv_anim_set_path_cb(&a, ANIM_POS_PATH);
    uint32_t playtime=lv_anim_timeline_get_playtime(st_anim_ctrl.animline);
    lv_anim_timeline_add(st_anim_ctrl.animline,playtime,&a);
}
void pm_anim_push_pos_xy(void* driving,void* passive,int16_t start_x,int16_t end_x,int16_t start_y,int16_t end_y)
{
    st_anim_ctrl.driving=(lv_obj_t*)driving;
    st_anim_ctrl.passive=(lv_obj_t*)passive;
    lv_anim_t ax;
    lv_anim_init(&ax);
    lv_anim_set_var(&ax, (void*)((int16_t)start_x));
    lv_anim_set_values(&ax,start_x,end_x);
    lv_anim_set_time(&ax, ANIM_TIME);
    lv_anim_set_exec_cb(&ax, anim_exec_push_pos_x_cb);
    lv_anim_set_path_cb(&ax, ANIM_POS_PATH);

    lv_anim_t ay;
    lv_anim_init(&ay);
    lv_anim_set_var(&ay, (void*)((int16_t)start_y));
    lv_anim_set_values(&ay,start_y,end_y);
    lv_anim_set_time(&ay, ANIM_TIME);
    lv_anim_set_exec_cb(&ay, anim_exec_push_pos_y_cb);
    lv_anim_set_path_cb(&ay, ANIM_POS_PATH);

    uint32_t playtime=lv_anim_timeline_get_playtime(st_anim_ctrl.animline);
    lv_anim_timeline_add(st_anim_ctrl.animline,playtime,&ax);
    lv_anim_timeline_add(st_anim_ctrl.animline,playtime,&ay);
}


static void anim_exec_size_height(void *obj, int32_t height)
{
    lv_obj_set_height(obj,height);
}
void pm_anim_size_height(void* page,int16_t start,int16_t end)
{
    lv_obj_t* obj=(lv_obj_t*)page;

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, start, end);
    lv_anim_set_exec_cb(&a, anim_exec_size_height);
    lv_anim_set_path_cb(&a,ANIM_SIZE_PATH);
    lv_anim_set_time(&a, ANIM_TIME);

    uint32_t playtime=lv_anim_timeline_get_playtime(st_anim_ctrl.animline);
    lv_anim_timeline_add(st_anim_ctrl.animline,playtime,&a);
}
static void anim_exec_size_width(void *ctx, int32_t width)
{
    lv_obj_set_width(ctx,width);
}
void pm_anim_size_width(void* page,int16_t start,int16_t end)
{
    lv_obj_t* obj=(lv_obj_t*)page;

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, start, end);
    lv_anim_set_exec_cb(&a, anim_exec_size_width);
    lv_anim_set_path_cb(&a,ANIM_SIZE_PATH);
    lv_anim_set_time(&a, ANIM_TIME);

    uint32_t playtime=lv_anim_timeline_get_playtime(st_anim_ctrl.animline);
    lv_anim_timeline_add(st_anim_ctrl.animline,playtime,&a);
}
void pm_anim_size(void* page,int16_t start_w,int16_t end_w,int16_t start_h,int16_t end_h)
{
    lv_obj_t* obj=(lv_obj_t*)page;

    lv_anim_t aw;
    lv_anim_init(&aw);
    lv_anim_set_var(&aw, obj);
    lv_anim_set_values(&aw, start_w, end_w);
    lv_anim_set_exec_cb(&aw, anim_exec_size_width);
    lv_anim_set_path_cb(&aw,ANIM_SIZE_PATH);
    lv_anim_set_time(&aw, ANIM_TIME);

    lv_anim_t ah;
    lv_anim_init(&ah);
    lv_anim_set_var(&ah, obj);
    lv_anim_set_values(&ah, start_h, end_h);
    lv_anim_set_exec_cb(&ah, anim_exec_size_height);
    lv_anim_set_path_cb(&ah,ANIM_SIZE_PATH);
    lv_anim_set_time(&ah, ANIM_TIME);

    uint32_t playtime=lv_anim_timeline_get_playtime(st_anim_ctrl.animline);
    lv_anim_timeline_add(st_anim_ctrl.animline,playtime,&aw);
    lv_anim_timeline_add(st_anim_ctrl.animline,playtime,&ah);
}
static void anim_exec_fade(void *ctx, int32_t opa)
{
    lv_obj_set_style_opa(ctx,opa,0);
}
void pm_anim_fade_in(void* page)
{
    lv_obj_t* obj=(lv_obj_t*)page;

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, LV_OPA_TRANSP, lv_obj_get_style_opa(obj, 0));
    lv_anim_set_exec_cb(&a, anim_exec_fade);
    lv_anim_set_time(&a, ANIM_TIME);

    uint32_t playtime=lv_anim_timeline_get_playtime(st_anim_ctrl.animline);
    lv_anim_timeline_add(st_anim_ctrl.animline,playtime,&a);
}
void pm_anim_fade_out(void* page)
{
    lv_obj_t* obj=(lv_obj_t*)page;

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, lv_obj_get_style_opa(obj, 0), LV_OPA_TRANSP);
    lv_anim_set_exec_cb(&a, anim_exec_fade);
    lv_anim_set_time(&a, ANIM_TIME*2);

    uint32_t playtime=lv_anim_timeline_get_playtime(st_anim_ctrl.animline);
    lv_anim_timeline_add(st_anim_ctrl.animline,playtime,&a);
}