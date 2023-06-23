#ifndef PM_ANIM_H
#define PM_ANIM_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include "page_node.h"
typedef void(*anim_done_cb_t)(void* ctx);
void pm_anim_over_pos_x(page_node_t* page,anim_done_cb_t cb,void* ctx,int32_t start,int32_t end);
void pm_anim_over_pos_y(page_node_t* page,anim_done_cb_t cb,void* ctx,int32_t start,int32_t end);
void pm_anim_over_top_to_buttom(page_node_t* page,anim_done_cb_t cb,void* ctx);
void pm_anim_over_buttom_to_top(page_node_t* page,anim_done_cb_t cb,void* ctx);
void pm_anim_over_left_to_right(page_node_t* page,anim_done_cb_t cb,void* ctx);
void pm_anim_over_right_to_left(page_node_t* page,anim_done_cb_t cb,void* ctx);

void pm_anim_push_pos_x(page_node_t* origin,page_node_t* new,void* cb,void* ctx,int32_t start,int32_t end);
void pm_anim_push_pos_y(page_node_t* origin,page_node_t* new,void* cb,void* ctx,int32_t start,int32_t end);
void pm_anim_push_top_to_buttom(page_node_t* origin,page_node_t* new,void* cb,void* ctx);
void pm_anim_push_buttom_to_top(page_node_t* origin,page_node_t* new,void* cb,void* ctx);
void pm_anim_push_left_to_right(page_node_t* origin,page_node_t* new,void* cb,void* ctx);
void pm_anim_push_right_to_left(page_node_t* origin,page_node_t* new,void* cb,void* ctx);

void pm_anim_size_width(page_node_t* page,void* cb,void* ctx,int32_t start,int32_t end);
void pm_anim_size_height(page_node_t* page,void* cb,void* ctx,int32_t start,int32_t end);

void pm_anim_fade_in(page_node_t* page,void* cb,void* ctx);
void pm_anim_fade_out(page_node_t* page,void* cb,void* ctx);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
