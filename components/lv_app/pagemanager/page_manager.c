
#include "page_manager.h"
#include "pm_anim.h"
#include "lvgl.h"
#include <stdlib.h>
#include "stdint.h"
#include "stdio.h"
// #include "esp_err.h"
// #include "esp_log.h"
#define TAG "PM:"

typedef struct page_manager_ctrl
{
    uint8_t status;
    pm_anim_style_t animType;
    uint8_t language;
    node_list_t pageStack;
    node_item_t* pageCache[3];
    node_list_t pagePool;
    page_node_t* pageWillRelease;
}page_manager_ctrl_t;
typedef page_err_t (*pmChangeCB)(page_manager_ctrl_t* PM);
typedef page_err_t (*pmStatusCB)(page_manager_ctrl_t* PM);

static page_err_t pm_change_status_to_anim_push_cb(page_manager_ctrl_t* PM);
static page_err_t pm_change_status_to_anim_pop_cb(page_manager_ctrl_t* PM);
static const pmChangeCB PM_CHANGE_STATUS_CB[PM_STATUS_MAX]=
{
    NULL,
    NULL,
    pm_change_status_to_anim_push_cb,
    NULL,
    pm_change_status_to_anim_pop_cb,
    NULL,
};
static page_err_t pm_status_anim_push_done_cb(page_manager_ctrl_t* PM);
static page_err_t pm_status_anim_pop_done_cb(page_manager_ctrl_t* PM);
static const pmStatusCB PM_RUN_STATUS_CB[PM_STATUS_MAX]=
{
    NULL,
    NULL,
    NULL,
    pm_status_anim_push_done_cb,
    NULL,
    pm_status_anim_pop_done_cb,
};
static page_manager_ctrl_t pm_ctrl;

static page_err_t pm_change_status_to(pm_status_t status)
{
    pm_ctrl.status=status;
    if (PM_CHANGE_STATUS_CB[status])
    {
        return PM_CHANGE_STATUS_CB[status](&pm_ctrl);
    }
    return PM_ERR_OK;
}
void pm_anim_push_done_cb(lv_anim_t * pAnim)
{
    pm_change_status_to(PM_STATUS_ANIMATION_PUSH_DONE);
}
void pm_anim_pop_done_cb(lv_anim_t * pAnim)
{
    pm_change_status_to(PM_STATUS_ANIMATION_POP_DONE);
}

static page_err_t pm_change_status_to_anim_push_cb(page_manager_ctrl_t* PM)
{
    page_err_t err=PM_ERR_OK;
    page_node_t* page=NULL;
    if (pm_ctrl.pageStack.len)
    {
        node_item_t* pNode=node_list_get_tail(&(pm_ctrl.pageStack));
        page=node_entry(pNode,page_node_t,node);
        // if (page->onAppearing)
        // {
        //     page->onAppearing(page);
        // } 
    }
    switch (PM->animType)
    {
    case PM_ANIM_OVER_LEFT_TO_RIGHT:
        /* code */
        break;
    case PM_ANIM_OVER_RIGHT_TO_LEFT:
        /* code */
        break;
    case PM_ANIM_OVER_TOP_TO_BOTTOM:
        pm_anim_over_top_to_buttom(page->obj,pm_anim_push_done_cb);
        break;
    case PM_ANIM_OVER_BOTTOM_TO_TOP:
        pm_anim_over_buttom_to_top(page->obj,pm_anim_push_done_cb);
        break;
    default:
        break;
    }
    return err;
}
static page_err_t pm_change_status_to_anim_pop_cb(page_manager_ctrl_t* PM)
{
    page_err_t err=PM_ERR_OK;
    switch (PM->animType)
    {
    case PM_ANIM_OVER_LEFT_TO_RIGHT:
        /* code */
        break;
    case PM_ANIM_OVER_RIGHT_TO_LEFT:
        /* code */
        break;
    case PM_ANIM_OVER_TOP_TO_BOTTOM:
        pm_anim_over_top_to_buttom(PM->pageWillRelease->obj,pm_anim_pop_done_cb);
        break;
    case PM_ANIM_OVER_BOTTOM_TO_TOP:
        pm_anim_over_buttom_to_top(PM->pageWillRelease->obj,pm_anim_pop_done_cb);
        break;
    default:
        break;
    }
    return err;
}
static page_err_t pm_status_anim_push_done_cb(page_manager_ctrl_t* PM)
{
    if (pm_ctrl.pageStack.len)
    {
        node_item_t* pNode=node_list_get_tail(&(pm_ctrl.pageStack));
        page_node_t* page=node_entry(pNode,page_node_t,node);
        // lv_obj_clear_flag(page->obj,LV_OBJ_FLAG_HIDDEN);
        if (page->onAppearing)
        {
            page->onAppearing(page);
        } 
    }
    pm_change_status_to(PM_STATUS_IDLE);
}
static page_err_t pm_status_anim_pop_done_cb(page_manager_ctrl_t* PM)
{
    if (pm_ctrl.pageWillRelease->onRelease)
    {   
        pm_ctrl.pageWillRelease->onRelease(pm_ctrl.pageWillRelease);
    }
    pm_change_status_to(PM_STATUS_IDLE);
}
page_err_t pm_init(void)
{
    memset(&pm_ctrl,0x00,sizeof(pm_ctrl));
    node_list_init(&(pm_ctrl.pagePool));
    node_list_init(&(pm_ctrl.pageStack));
    pm_ctrl.status=PM_STATUS_IDLE;
}

page_node_t* pm_find_page_in_pool(const char* name)
{
    page_node_t* page=NULL;
    node_item_t* pos;
    __list_for_each(pos,&(pm_ctrl.pagePool.root))
    {
        page=node_entry(pos,page_node_t,node);
        if(strcmp(name,page->name)==0)
        {
            break; 
        }
        else
        {
            page=NULL;
        }
    }
    if (!page)
    {
        printf(TAG,"can not find %s page in pool!",name);
    }
    return page;
}
page_node_t* pm_find_page_in_stack(const char* name)
{
    page_node_t* page=NULL;
    node_item_t* pos;
    __list_for_each(pos,&(pm_ctrl.pageStack.root))
    {
        page=node_entry(pos,page_node_t,node);
        if(strcmp(name,page->name)==0)
        {
            break; 
        }
        else
        {
            page=NULL;
        }
    }
    if (!page)
    {
        printf(TAG,"can not find %s page in stack!",name);
    }
    return page;
}
page_err_t pm_register_page(page_node_t* page)
{
    node_list_t* pList=&(pm_ctrl.pagePool);
    node_list_add_tail(pList,&(page->node));
}
page_err_t pm_stack_push_page(const char* name,pm_anim_style_t animType)
{
    if (pm_ctrl.status!=PM_STATUS_IDLE)
    {
       return PM_ERR_BUSY;
    }
    if (pm_find_page_in_stack(name)!=NULL)
    {
        return PM_ERR_DOUBLE_PAGE;
    }
    page_node_t* page=pm_find_page_in_pool(name);
    if (!page)
    {
        return PM_ERR_NOT_FOUND;
    }
    node_list_add_tail(&(pm_ctrl.pageStack),&(page->node));
    if (page->onCreate)
    {
        page->onCreate(page);
    }
    // lv_obj_add_flag(page->obj,LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(page->obj);
    pm_ctrl.animType = animType;
    pm_change_status_to(PM_STATUS_ANIMATION_PUSH);
}
page_err_t pm_stack_pop_page(pm_anim_style_t animType)
{
    if (pm_ctrl.status!=PM_STATUS_IDLE)
    {
       return PM_ERR_BUSY;
    }
    if (!pm_ctrl.pageStack.len)
    {
        return PM_ERR_NO_PAGE;
    }
    node_item_t* pNode=node_list_take_tail(&(pm_ctrl.pageStack));
    page_node_t* page = node_entry(pNode,page_node_t,node);
    if (page->onDisappearing)
    {
        page->onDisappearing(page);
    }
    pm_ctrl.pageWillRelease=page;
    pm_ctrl.animType = animType;
    pm_change_status_to(PM_STATUS_ANIMATION_POP);
}
void pm_run(void)
{
    if (PM_RUN_STATUS_CB[pm_ctrl.status])
    {
        PM_RUN_STATUS_CB[pm_ctrl.status](&pm_ctrl);
    }
}