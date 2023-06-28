#include "pages.h"
#include "lvgl.h"
extern void page_main_init();
extern void page2_init();
extern uint32_t dispHZ;
static lv_obj_t* lableHz;
static void timerhz(lv_timer_t* timer)
{
    lv_label_set_text_fmt(lableHz,"%02dHZ",dispHZ);
}
void pages_init(void)
{
    lableHz=lv_label_create(lv_layer_top());
    lv_obj_set_size(lableHz,LV_SIZE_CONTENT,LV_SIZE_CONTENT);
    lv_obj_align(lableHz,LV_ALIGN_LEFT_MID,0,0);
    lv_timer_create(timerhz,100,NULL);
    page_main_init();
    page2_init();
}