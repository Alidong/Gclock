#ifndef PM_ANIM_H
#define PM_ANIM_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include "lvgl.h"
void pm_anim_over_top_to_buttom(lv_obj_t* obj,void* cb);
void pm_anim_over_buttom_to_top(lv_obj_t* obj,void* cb);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
