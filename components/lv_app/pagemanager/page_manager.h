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
    PM_ERR_FAIL = -1,
    PM_ERR_NOT_FOUND,
    PM_ERR_DOUBLE_PAGE,
    PM_ERR_NO_PAGE,
    PM_ERR_BUSY,
} page_err_t;

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
    PM_ANIM_SIZE_HEIGHT,
    PM_ANIM_SIZE_WIDTH,
    PM_ANIM_FADE_IN,
    PM_ANIM_FADE_OUT,
    PM_ANIM_NONE,
} pm_anim_style_t;

page_err_t pm_init(void);                                                       //init pm
page_err_t pm_register_page(page_node_t* page);                                 //register page in page_pool       
page_err_t pm_stack_pop_page(const char* name,pm_anim_style_t animType);                         //release top page in stack
page_err_t pm_stack_push_page(const char* name, pm_anim_style_t animType);      //push new page
page_err_t pm_stack_replace_page(const char* name,pm_anim_style_t animType);    //new page will replce top page in stack 
page_err_t pm_stack_back_home_page(pm_anim_style_t animType);                   //back to buttom page in stack
void pm_run(void);                                                              //pm handler 
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
