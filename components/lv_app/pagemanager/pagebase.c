
#include "pagebase.h"
#include <stdlib.h>
#include "stdio.h"
#define TAG "pagemanager:"

static page_node_t root_node;
static bool search_page_pool(page_node_t *page);
page_err_t page_pop(page_node_t *page)
{
    page_node_t *pagePop = page;
    if (pagePop != NULL)
    {
        /*遍历内存池查看页面是否存在*/
        if (search_page_pool(pagePop) != true)
            return PAGE_NOERR;
        pagePop->prePage->nextPage = pagePop->nextPage;
        if (pagePop->nextPage != NULL)
        {
            pagePop->nextPage->prePage = pagePop->prePage;
        }
    }
    else
    {
        pagePop = page_stack_top();
        if (pagePop == &root_node)
        {
            return PAGE_REACH_BOTTOM;
        }
        pagePop->prePage->nextPage = NULL;
        pagePop->nextPage = NULL;
        pagePop->prePage = NULL;
    }
    /*释放资源*/
    if (pagePop->onDisappearing != NULL)
    {
        //pagePop->is_Quit = true;              //是否退出,释放资源
        pagePop->onDisappearing(pagePop);       //消失动画
    }
    // if(pagePop->onRelease!= NULL)
    // {
    //     pagePop->onRelease(pagePop);         //直接释放obj
    //     pagePop->isReleased=true;
    // }
    return PAGE_NOERR;
}
page_err_t page_push(page_node_t *page)
{
    /*遍历内存池查看推送的页面是否重复添加*/
    if (search_page_pool(page))
        return PAGE_ERR;
    page->prePage = page_stack_top();
    page->prePage->nextPage = page;
    page->nextPage = NULL;
    //page->prePage->is_Quit = false;
    if (page->isReleased)
    {
        page->onCreate(page);
        page->isReleased = false;
    }
    if (page->onAppearing != NULL)
    {
        page->onAppearing(page);
    }
    return PAGE_NOERR;
}
page_err_t page_stack_init()
{
    memset(&root_node, 0, sizeof(page_node_t));
    //lv_timer_t* timer = lv_timer_create(updatePage_cb, UPDATE_HZ(CONF_UPDATE_HZ), update_Ptr); //更新页面数据
    return PAGE_NOERR;
}
page_node_t *page_stack_top(void)
{
    // ESP_LOGI(TAG,"root=%d", (uint32_t)root);
    page_node_t *page = &root_node;
    while (page->nextPage != NULL)
    {
        page = page->nextPage;
    }
    return page;
}
page_node_t *page_stack_root(void)
{
    return &root_node;
}
page_err_t page_pop_all(void)
{
    page_node_t *root = &root_node;
    page_node_t *page = page_stack_top();
    if (page == root)
    {
        return PAGE_REACH_BOTTOM;
    }
    while (page != root)
    {
        page->prePage->nextPage = NULL;
        page->prePage = NULL;
        page->nextPage = NULL;
        //page->is_Quit = true;
        page->onRelease(page);
        page = page_stack_top();
    }
    page->onAppearing(page); //出现动画
    return PAGE_NOERR;
}
static bool search_page_pool(page_node_t *page)
{
    /*遍历内存池查看推送的页面是否重复添加*/
    page_node_t *page_pool = page_stack_top(); //从栈顶开始遍历
    while (page_pool->prePage != NULL)
    {
        if (page == page_pool)
            return true;
        page_pool = page_pool->prePage;
    }
    return false;
}
