#ifndef PAGE_NODE_H
#define PAGE_NODE_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "node.h"
typedef struct _page_node
{
    node_item_t node;                              
    const char* name;                           
    void *ctx;                                               // contex
    void *obj;                                               // 页面对象
    void (*onCreate)(struct _page_node *page);               // 页面创建
    void (*onAppearing)(struct _page_node *page);            // 页面出现的过度动画
    void (*onDisappearing)(struct _page_node *page);         // 页面消失的过度动画
    void (*onRelease)(struct _page_node *page);              // 释放页面
    void (*eventHandler)(const void *event);                 // lvgl 事件处理回调
    void(*timerHandler)(const void *usrCtx);                 // 需要定时更新数据
} page_node_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
