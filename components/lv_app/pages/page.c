#include "page.h"
#include "lvgl.h"
extern void page_main_init();
extern void page_bar_init();
void pages_init(void)
{
    page_stack_init();
    page_main_init();
    page_bar_init();
    page_push(&page_main);
    page_push(&page_bar);
}