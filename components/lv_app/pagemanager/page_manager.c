
#include "page_manager.h"
#include "pm_anim.h"
#include "lvgl.h"
#include <stdlib.h>
#include "stdint.h"
#include "stdio.h"
#include "esp_log.h"

#define TAG "PM:"
typedef enum
{
    PM_STATUS_NOT_INIT = 0,
    PM_STATUS_IDLE = 1,
    PM_STATUS_ANIMATION_START,
    PM_STATUS_ANIMATION_RUNNING,
    PM_STATUS_ANIMATION_DONE,
    PM_STATUS_MAX,
} pm_status_t;

typedef struct page_manager_ctrl
{
    uint8_t status;
    lv_point_t animStart;
    lv_point_t animEnd;
    pm_anim_style_t animType;
    lv_anim_timeline_t* timeline;
    uint32_t tick;
    node_list_t pageStack;
    node_list_t pagePool;
    page_node_t* pageActive;
    page_node_t* pageWillRelease;
}page_manager_ctrl_t;
typedef page_err_t (*pmChangeCB)(page_manager_ctrl_t* PM);
typedef page_err_t (*pmStatusCB)(page_manager_ctrl_t* PM);

static page_err_t pm_change_status_to_idle_cb(page_manager_ctrl_t* PM);
static page_err_t pm_change_status_to_anim_start_cb(page_manager_ctrl_t* PM);
static page_err_t pm_change_status_to_anim_done_cb(page_manager_ctrl_t* PM);
static const pmChangeCB PM_CHANGE_STATUS_CB[PM_STATUS_MAX]=
{
    NULL,
    pm_change_status_to_idle_cb,
    pm_change_status_to_anim_start_cb,
    NULL,
    pm_change_status_to_anim_done_cb,
};
static page_err_t pm_status_anim_running_cb(page_manager_ctrl_t* PM);
static const pmStatusCB PM_RUN_STATUS_CB[PM_STATUS_MAX]=
{
    NULL,
    NULL,
    NULL,
    pm_status_anim_running_cb,
    NULL,
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
static page_err_t pm_change_status_to_idle_cb(page_manager_ctrl_t* PM)
{
    PM->animType=PM_ANIM_NONE;
    if (PM->pageWillRelease)
    {
        PM->pageWillRelease->obj=NULL;
        PM->pageWillRelease=NULL;
    }
    if (PM->timeline)
    {
        lv_anim_timeline_del(PM->timeline);
        PM->tick=0;
        PM->timeline=NULL;
    }
    if (PM->pageStack.len)
    {
        node_item_t* pNode= node_list_get_tail(&(PM->pageStack));
        page_node_t* pageActive=node_entry(pNode,page_node_t,node);
        PM->pageActive=pageActive;
    }
    PM->animStart.x=0;
    PM->animStart.y=0;
    PM->animEnd.x=0;
    PM->animEnd.y=0;
    pm_anim_reset();
    return PM_ERR_OK;
}

static page_err_t pm_change_status_to_anim_start_cb(page_manager_ctrl_t* PM)
{
    page_err_t err=PM_ERR_OK;
    PM->timeline=lv_anim_timeline_create();
    pm_anim_set_timeline(PM->timeline);
    lv_obj_t* objRelease=NULL;
    if (PM->pageWillRelease)
    {   
        if ( PM->pageWillRelease->onDisappearing)
        {
            PM->pageWillRelease->onDisappearing(PM->pageWillRelease,0,PM->timeline);
        }
        objRelease=PM->pageWillRelease->obj;
    }
    lv_obj_t* objActive=NULL;
    if (PM->pageActive)
    {
        objActive=PM->pageActive->obj;
    }
    switch (PM->animType)
    {
    case PM_ANIM_OVER_LEFT_TO_RIGHT:
    case PM_ANIM_OVER_RIGHT_TO_LEFT:
        if (objRelease)
        {
            pm_anim_over_pos_x(objRelease,PM->animStart.x,PM->animEnd.x);
        }
        else
        {
            pm_anim_over_pos_x(objActive,PM->animStart.x,PM->animEnd.x);
        }
        break;
    case PM_ANIM_OVER_TOP_TO_BOTTOM:
    case PM_ANIM_OVER_BOTTOM_TO_TOP:
        if (objRelease)
        {
            pm_anim_over_pos_y(objRelease,PM->animStart.y,PM->animEnd.y);
        }
        else
        {
            pm_anim_over_pos_y(objActive,PM->animStart.y,PM->animEnd.y);
        }
        break;
    case PM_ANIM_PUSH_LEFT_TO_RIGHT:
    case PM_ANIM_PUSH_RIGHT_TO_LEFT:
        if(objActive && objRelease)
        {
            pm_anim_push_pos_x(objActive,objRelease,PM->animStart.x,PM->animEnd.x);
        }
        break;  
    case PM_ANIM_PUSH_TOP_TO_BOTTOM:
    case PM_ANIM_PUSH_BOTTOM_TO_TOP:
        if(objActive && objRelease)
        {
            pm_anim_push_pos_y(objActive,objRelease,PM->animStart.y,PM->animEnd.y);
        }   
        break;   
    case PM_ANIM_SIZE_HEIGHT:
        if (objRelease)
        {
            pm_anim_size_height(objRelease,PM->animStart.y,PM->animEnd.y);
        }
        else
        {
            pm_anim_size_height(objActive,PM->animStart.y,PM->animEnd.y);
        }
        break; 
    case PM_ANIM_SIZE_WIDTH:
        if (objRelease)
        {
            pm_anim_size_width(objRelease,PM->animStart.x,PM->animEnd.x);
        }
        else
        {
            pm_anim_size_width(objActive,PM->animStart.x,PM->animEnd.x);
        }
        break;      
    case PM_ANIM_FADE_IN:
        if (objActive)
        {
            pm_anim_fade_in(objActive);
        }
        break;  
    case PM_ANIM_FADE_OUT:
        if (objRelease)
        {
            pm_anim_fade_out(objRelease);
        }
        break;                  
    default:
        //not support
        pm_change_status_to(PM_STATUS_ANIMATION_DONE);
        break;
    }
    if (PM->pageActive->onAppearing)
    {
        uint32_t delay= lv_anim_timeline_get_playtime(PM->timeline);
        PM->pageActive->onAppearing(PM->pageActive,delay,PM->timeline);
    }
    PM->tick=lv_anim_timeline_get_playtime(PM->timeline)+lv_tick_get();
    lv_anim_timeline_start(PM->timeline);
    pm_change_status_to(PM_STATUS_ANIMATION_RUNNING);
    return err;
}

static page_err_t pm_status_anim_running_cb(page_manager_ctrl_t* PM)
{   
    if (PM->timeline)
    {
        if (lv_tick_get()>(PM->tick))
        {
            return pm_change_status_to(PM_STATUS_ANIMATION_DONE);
        }
    }
    return PM_ERR_OK;
}
static page_err_t pm_change_status_to_anim_done_cb(page_manager_ctrl_t* PM)
{
    if (PM->pageWillRelease && PM->pageWillRelease->onRelease)
    {   
        PM->pageWillRelease->onRelease(PM->pageWillRelease);
        ESP_LOGI(TAG,"release %s page!",PM->pageWillRelease->name);
    } 
    return pm_change_status_to(PM_STATUS_IDLE);
}
page_err_t pm_init(void)
{
    memset(&pm_ctrl,0x00,sizeof(pm_ctrl));
    node_list_init(&(pm_ctrl.pagePool));
    node_list_init(&(pm_ctrl.pageStack));
    pm_ctrl.status=PM_STATUS_IDLE;
    return PM_ERR_OK;
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
        ESP_LOGI(TAG,"can not find %s page in stack!",name);
    }
    return page;
}
page_err_t pm_register_page(page_node_t* page)
{
    node_list_t* pList=&(pm_ctrl.pagePool);
    node_list_add_tail(pList,&(page->node));
    return PM_ERR_OK;
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
        ESP_LOGE(TAG,"can not find %s page in pool!",name);
        return PM_ERR_NOT_FOUND;
    }
    node_list_delete_item(&(pm_ctrl.pagePool),&(page->node));
    node_list_add_tail(&(pm_ctrl.pageStack),&(page->node));
    pm_ctrl.pageActive=page;
    page->onCreate(page);
    lv_obj_move_foreground(page->obj);
    lv_obj_update_layout(page->obj);
    pm_ctrl.animType = animType;
    bool needAnim=true;
    switch (animType)
    {
    case PM_ANIM_OVER_LEFT_TO_RIGHT:
        pm_ctrl.animStart.x=-lv_disp_get_hor_res(NULL);
        pm_ctrl.animEnd.x= 0;
        break;
    case PM_ANIM_OVER_RIGHT_TO_LEFT:
        pm_ctrl.animStart.x=lv_disp_get_hor_res(NULL);
        pm_ctrl.animEnd.x= 0;
        break;
    case PM_ANIM_OVER_TOP_TO_BOTTOM:
        pm_ctrl.animStart.y=-lv_disp_get_ver_res(NULL);
        pm_ctrl.animEnd.y= 0;
        break;
    case PM_ANIM_OVER_BOTTOM_TO_TOP:
        pm_ctrl.animStart.y=0;
        pm_ctrl.animEnd.y= lv_disp_get_ver_res(NULL);
        break;
    case PM_ANIM_SIZE_HEIGHT:
        pm_ctrl.animStart.y=lv_obj_get_height(page->obj)/3;
        pm_ctrl.animEnd.y= lv_obj_get_height(page->obj);
        break; 
    case PM_ANIM_SIZE_WIDTH:
        pm_ctrl.animStart.x=lv_obj_get_width(page->obj)/3;
        pm_ctrl.animEnd.x= lv_obj_get_width(page->obj);
        break;       
    case PM_ANIM_FADE_IN:
        break;      
    default:
        needAnim=false;
        break;
    }
    if (page->onAppearing || needAnim)
    {
        return pm_change_status_to(PM_STATUS_ANIMATION_START);
    }
    else
    {
        return pm_change_status_to(PM_STATUS_ANIMATION_DONE);
    }
}
page_err_t pm_stack_pop_page(const char* name,pm_anim_style_t animType)
{
    if (pm_ctrl.status!=PM_STATUS_IDLE)
    {
       return PM_ERR_BUSY;
    }
    if (!pm_ctrl.pageStack.len)
    {
        return PM_ERR_NO_PAGE;
    }
    if (!name)
    {
        // node_item_t* pNode=node_list_take_tail(&(pm_ctrl.pageStack));
        // page_node_t* page =node_entry(pNode,page_node_t,node);
        page_node_t* page = pm_ctrl.pageActive;
        node_list_delete_item(&(pm_ctrl.pageStack),&(page->node));
        node_list_add_tail(&(pm_ctrl.pagePool),&(page->node));
        pm_ctrl.pageWillRelease=page;
        pm_ctrl.animType = animType;
        pm_ctrl.pageActive=NULL;
    }
    else
    {
        page_node_t* page =pm_find_page_in_stack(name);
        if (!page)
        {
            return PM_ERR_NO_PAGE;
        }
        node_list_delete_item(&(pm_ctrl.pageStack),&(page->node));
        node_list_add_tail(&(pm_ctrl.pagePool),&(page->node));
        pm_ctrl.pageWillRelease=page;
        if (page==pm_ctrl.pageActive)
        {   
            pm_ctrl.animType = animType;
            pm_ctrl.pageActive=NULL;
        }
        else
        {
            pm_ctrl.animType = PM_ANIM_NONE;
        }
    }
    page_node_t* page = pm_ctrl.pageWillRelease;
    bool needAnim=true;
    switch (animType)
    {
    case PM_ANIM_OVER_LEFT_TO_RIGHT:
        pm_ctrl.animStart.x=0;
        pm_ctrl.animEnd.x= lv_disp_get_hor_res(NULL);;
        break;
    case PM_ANIM_OVER_RIGHT_TO_LEFT:
        pm_ctrl.animStart.x=0;
        pm_ctrl.animEnd.x= -lv_disp_get_hor_res(NULL);
        break;
    case PM_ANIM_OVER_TOP_TO_BOTTOM:
        pm_ctrl.animStart.y=0;
        pm_ctrl.animEnd.y= lv_disp_get_ver_res(NULL);
        break;
    case PM_ANIM_OVER_BOTTOM_TO_TOP:
        pm_ctrl.animStart.y=0;
        pm_ctrl.animEnd.y= -lv_disp_get_ver_res(NULL);
        break;
    case PM_ANIM_SIZE_HEIGHT:
        pm_ctrl.animStart.y=lv_obj_get_height(page->obj);
        pm_ctrl.animEnd.y= lv_obj_get_height( page->obj)/3;
        break; 
    case PM_ANIM_SIZE_WIDTH:
        pm_ctrl.animStart.x=lv_obj_get_width(page->obj);
        pm_ctrl.animEnd.x= lv_obj_get_width(page->obj)/3;
        break;       
    case PM_ANIM_FADE_OUT:
        break;      
    default:
        needAnim=false;
        break;
    }
    if (page->onAppearing || needAnim)
    {
        return pm_change_status_to(PM_STATUS_ANIMATION_START);
    }
    else
    {
        return pm_change_status_to(PM_STATUS_ANIMATION_DONE);
    }
}
page_err_t pm_stack_replace_page(const char* name,pm_anim_style_t animType)
{
    if (pm_ctrl.status!=PM_STATUS_IDLE)
    {
       return PM_ERR_BUSY;
    }
    page_node_t* page=NULL;
    if (name)
    {
        //new page in pool replace top page in stack
        if (pm_find_page_in_stack(name)!=NULL)
        {
            return PM_ERR_DOUBLE_PAGE;
        }
        page=pm_find_page_in_pool(name);
        if (!page)
        {
            ESP_LOGE(TAG,"can not find %s page in pool!",name);
            return PM_ERR_NOT_FOUND;
        }
        if (pm_ctrl.pageStack.len)
        {
            node_list_delete_item(&(pm_ctrl.pageStack),&(pm_ctrl.pageActive->node));
            node_list_add_tail(&(pm_ctrl.pagePool),&(pm_ctrl.pageActive->node));
            pm_ctrl.pageWillRelease = pm_ctrl.pageActive;
            pm_ctrl.pageActive=NULL;
        }
        else
        {
            pm_ctrl.pageWillRelease=NULL;
        }
        node_list_delete_item(&(pm_ctrl.pagePool),&(page->node));
        node_list_add_tail(&(pm_ctrl.pageStack),&(page->node));
        pm_ctrl.pageActive=page;
        if (page->onCreate)
        {
            page->onCreate(page);
        }
    }
    else 
    {
        return PM_ERR_FAIL;
    }

    lv_obj_move_foreground(page->obj);    
    lv_obj_update_layout(page->obj);

    pm_ctrl.animType = animType;
    bool needAnim=true;
    switch (animType)
    {
    case PM_ANIM_OVER_LEFT_TO_RIGHT:
    case PM_ANIM_PUSH_LEFT_TO_RIGHT:
        pm_ctrl.animStart.x=-lv_disp_get_hor_res(NULL);
        pm_ctrl.animEnd.x= 0;
        break;
    case PM_ANIM_OVER_RIGHT_TO_LEFT:
    case PM_ANIM_PUSH_RIGHT_TO_LEFT:
        pm_ctrl.animStart.x=lv_disp_get_hor_res(NULL);
        pm_ctrl.animEnd.x= 0;
        break;
    case PM_ANIM_OVER_TOP_TO_BOTTOM:
    case PM_ANIM_PUSH_TOP_TO_BOTTOM:
        pm_ctrl.animStart.y=-lv_disp_get_ver_res(NULL);
        pm_ctrl.animEnd.y= 0;
        break;
    case PM_ANIM_OVER_BOTTOM_TO_TOP:
    case PM_ANIM_PUSH_BOTTOM_TO_TOP:
        pm_ctrl.animStart.y=lv_disp_get_ver_res(NULL);
        pm_ctrl.animEnd.y= 0;
        break;
    case PM_ANIM_SIZE_HEIGHT:
        pm_ctrl.animStart.y=lv_obj_get_height(page->obj)/3;
        pm_ctrl.animEnd.y= lv_obj_get_height( page->obj);
        break; 
    case PM_ANIM_SIZE_WIDTH:
        pm_ctrl.animStart.x=lv_obj_get_width(page->obj)/3;
        pm_ctrl.animEnd.x= lv_obj_get_width(page->obj);
        break;       
    case PM_ANIM_FADE_IN:
        break;      
    default:
        needAnim=false;
        break;
    }
    if (page->onAppearing || needAnim)
    {
        return pm_change_status_to(PM_STATUS_ANIMATION_START);
    }
    else
    {
        return pm_change_status_to(PM_STATUS_ANIMATION_DONE);
    }
}
page_err_t pm_stack_back_home_page(pm_anim_style_t animType)
{
    if (pm_ctrl.pageStack.len>=2 && pm_ctrl.status==PM_STATUS_IDLE)
    {
        node_item_t* pNode= node_list_get_head(&(pm_ctrl.pageStack));
        page_node_t* home=node_entry(pNode,page_node_t,node);

        page_node_t* topPage=pm_ctrl.pageActive;

        page_node_t* page;
        for (pNode = (&(pm_ctrl.pageStack.root))->next; pNode != (&(pm_ctrl.pageStack.root));)
        {
            if(pNode==&(topPage->node) || pNode==&(home->node))
            {
                pNode=pNode->next;
                continue;
            }
            else
            {
                node_item_t* next=pNode->next;
                page=node_entry(pNode,page_node_t,node);
                node_list_delete_item(&(pm_ctrl.pageStack),pNode);
                node_list_add_tail(&(pm_ctrl.pagePool),pNode);
                page->onRelease(page);
                page->obj=NULL;
                pNode=next;
            }
        }
        return pm_stack_pop_page(NULL,animType);
    }
    return PM_ERR_FAIL;
}
void pm_run(void)
{
    if (PM_RUN_STATUS_CB[pm_ctrl.status])
    {
        PM_RUN_STATUS_CB[pm_ctrl.status](&pm_ctrl);
    }
}