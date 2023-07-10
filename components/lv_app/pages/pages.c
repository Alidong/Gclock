#include "pageManager/page_manager.h"
#include "lvgl.h"
extern void page_main_init();
extern void page2_init();
extern void page3_init();
static void timer_cb(lv_timer_t* timer)
{
    static uint8_t count=0;
    uint8_t idx=count%2;
    // char name[12];
    // lv_snprintf(name,sizeof(name),"page%d",idx);
    // pm_stack_replace_page(name,PM_ANIM_PUSH_LEFT_TO_RIGHT);
    if (idx)
    {
        pm_stack_page_backstage(NULL,PM_ANIM_OVER_BOTTOM_TO_TOP);
    }
    else
    {
        pm_stack_push_page("page1",PM_ANIM_OVER_TOP_TO_BOTTOM);
    }
    count++;
}
void pages_init(void)
{
    page_main_init();
    page2_init();
    page3_init();
    pm_stack_push_page("page1",PM_ANIM_OVER_TOP_TO_BOTTOM);
    lv_timer_create(timer_cb,5000,NULL);
}