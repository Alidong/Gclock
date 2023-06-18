#include "page_node/page_node.h"
#include "scroll_page.h"
#include "lvgl.h"
enum{
    PAGE_TIME,
    PAGE_ENV,
    PAGE_SOUND,
    PAGE_MAX,
    // PAGE_WHEATHER,
    // PAGE_MONITOR,
    // PAGE_IMAGE,
    
};
page_node_t page_node_main;
extern lv_obj_t* page_env_init(lv_obj_t* parent);
extern lv_obj_t* page_time_init(lv_obj_t* parent);
extern lv_obj_t* page_sound_init(lv_obj_t* parent);
static void onCreate(struct _page_node *page)            // 页面创建
{
    lv_obj_t* objPool[PAGE_MAX];
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
    objPool[PAGE_TIME]=page_time_init(lv_scr_act());
    objPool[PAGE_ENV]=page_env_init(lv_scr_act());
    objPool[PAGE_SOUND]=page_sound_init(lv_scr_act());
    page_scroll_init(objPool,PAGE_MAX);
}
void page_node_main_init()
{
    page_node_main.isReleased = true;
    page_node_main.onCreate = onCreate;
    page_node_main.ctx = lv_scr_act();
    page_node_stack_init(&page_node_main);
}