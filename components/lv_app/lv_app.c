
#include "lv_app.h"
#include "lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lvgl/examples/lv_examples.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pages/page.h"
static void gui_thread(void *pvParameter)
{
    (void)pvParameter;
    lv_init();
    lv_port_init();
    pages_init();
    // page_push(&page_bar);
    // lv_example_anim_2();
    while (1)
    {
        lv_task_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    // Never returns
}
void lv_app_init(void)
{
    xTaskCreate(gui_thread, "lvgl", 1024 * 8, NULL, 3, NULL);
}