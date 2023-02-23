#include "page.h"
#include "lvgl.h"
#include "lvgl/examples/lv_examples.h"
page_node_t page_main;
static void onCreate(struct _page_node *page)            // 页面创建
{
    // lv_obj_t * parent = lv_obj_create(lv_scr_act());   /*Create a parent object on the current screen*/
    // lv_obj_set_size(parent, 480, 480);	                 /*Set the size of the parent*/

    // lv_obj_t * obj1 = lv_obj_create(parent);	         /*Create an object on the previously created parent object*/
    // lv_obj_set_pos(obj1, 100, 100);	                     /*Set the position of the new object*/
    // page->obj=obj1;
    lv_example_menu_1();
}
void page_main_init()
{
    page_main.onCreate=onCreate;
}