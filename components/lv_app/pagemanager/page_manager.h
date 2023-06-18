#ifndef PAGE_MANAGER_H
#define PAGE_MANAGER_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "page_node.h"
typedef enum
{
    PM_ERR_OK = 0,
    PM_ERR_NOT_FOUND = -1,
    PM_ERR_DOUBLE_PAGE,
    PM_ERR_NO_PAGE,
    PM_ERR_BUSY,
} page_err_t;

typedef enum
{
    PM_STATUS_NOT_INIT = 0,
    PM_STATUS_IDLE = 1,
    PM_STATUS_ANIMATION_PUSH,
    PM_STATUS_ANIMATION_PUSH_DONE,
    PM_STATUS_ANIMATION_POP,
    PM_STATUS_ANIMATION_POP_DONE,
    PM_STATUS_MAX,
} pm_status_t;

typedef enum
{
    PM_ANIM_OVER_LEFT_TO_RIGHT = 0,
    PM_ANIM_OVER_RIGHT_TO_LEFT = 1,
    PM_ANIM_OVER_TOP_TO_BOTTOM,
    PM_ANIM_OVER_BOTTOM_TO_TOP,
    PM_ANIM_PUSH_LEFT_TO_RIGHT,
    PM_ANIM_PUSH_RIGHT_TO_LEFT,
    PM_ANIM_PUSH_TOP_TO_BOTTOM,
    PM_ANIM_PUSH_BOTTOM_TO_TOP,
    PM_ANIM_FADE_IN,
    PM_ANIM_FADE_OUT,
    PM_ANIM_NONE,
} pm_anim_style_t;

page_err_t pm_init(void);           //初始化页面栈
page_err_t pm_register_page(page_node_t* page);  //注册页面到页面池中
page_node_t *page_node_stack_top(void);                    
page_node_t *page_node_stack_root(void);                    
page_err_t pm_stack_pop_page(pm_anim_style_t animType);
page_err_t pm_stack_push_page(const char* name, pm_anim_style_t animType);
                  
void pm_run(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
