#ifndef PAGE_NODE_H
#define PAGE_NODE_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "node.h"
#define PAGE_NAME_LEN 32
enum
{
    PAGE_DEAD=0,
    PAGE_ALIVE,
    PAGE_SUSPEND,
    PAGE_BACKSTAGE
};
#pragma pack(4)
typedef struct _page_node
{
    node_item_t node;
    uint8_t status;                            
    char name[PAGE_NAME_LEN+1];                           
    void *usrCtx;                                            // contex
    void *obj;                                               // 页面对象
    void (*onCreate)(struct _page_node *page);               // 页面创建
    void (*onAppearing)(struct _page_node *page,uint32_t delay,void* animLine);            // 页面出现的过度动画
    void (*onDisappearing)(struct _page_node *page,uint32_t delay,void* animLine);         // 页面消失的过度动画
    void (*onRelease)(struct _page_node *page);                 // 释放页面
    // void (*eventHandler)(const void *event);                 // lvgl 事件处理回调
    //TO DO:
    // msg subscribe and send
} page_node_t;
#pragma pack()
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
