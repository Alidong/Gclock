#ifndef PM_ANIM_H
#define PM_ANIM_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include "page_node.h"

void pm_anim_set_timeline(void* timeline);
void pm_anim_reset(void);

void pm_anim_over_pos_xy(void* page,int16_t start_x,int16_t end_x,int16_t start_y,int16_t end_y);
void pm_anim_over_pos_x(void* page,int16_t start_x,int16_t end_x);
void pm_anim_over_pos_y(void* page,int16_t start_y,int16_t end_y);

void pm_anim_push_pos_xy(void* driving,void* passive,int16_t start_x,int16_t end_x,int16_t start_y,int16_t end_y);
void pm_anim_push_pos_x(void* driving,void* passive,int16_t start_x,int16_t end_x);
void pm_anim_push_pos_y(void* driving,void* passive,int16_t start_y,int16_t end_y);

void pm_anim_size(void* page,int16_t start_w,int16_t end_w,int16_t start_h,int16_t end_h);
void pm_anim_size_width(void* page,int16_t start,int16_t end);
void pm_anim_size_height(void* page,int16_t start,int16_t end);

void pm_anim_fade_in(void* page);
void pm_anim_fade_out(void* page);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
