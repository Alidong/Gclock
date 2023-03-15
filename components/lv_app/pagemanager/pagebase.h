#ifndef PAGEBASE_H
#define PAGEBASE_H
#ifdef __cplusplus
extern "C"
{
#endif
#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "../lvgl/lvgl.h"
#endif
typedef enum
{
    PAGE_NOERR = 0,
    PAGE_ERR = -1,
    PAGE_REACH_BOTTOM = -2,
} page_err_t;
typedef struct _page_node
{
    struct _page_node *prePage;                              //父页面节点
    struct _page_node *nextPage;                             //子页面节点
    void *ctx;                                               // 用户对象
    void (*onCreate)(struct _page_node *page);               // 页面创建
    void (*onAppearing)(struct _page_node *page);            // 页面出现的过度动画
    void (*onDisappearing)(struct _page_node *page);      // 页面消失的过度动画
    void (*onRelease)(struct _page_node *page);              // 释放页面
    void (*hidePage)(struct _page_node *page, bool en);      //隐藏页面
    void (*eventHandler)(lv_event_t *event);                          //lvgl 事件处理回调
    void(*updatePage)(const void *Pagedata);                        //需要定时更新数据
    bool isReleased;
} page_node_t;

page_err_t page_stack_init();  //初始化页面栈
page_node_t *page_stack_top(void);                     //获取栈顶页面
page_node_t *page_stack_root(void);                  //获取栈底页面
page_err_t page_push(page_node_t *page);       //推送页面
page_err_t page_pop(page_node_t *page);        //页面出栈销毁
page_err_t page_pop_all(void);                  //回到主页面，销毁除root以外的所有页面
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
