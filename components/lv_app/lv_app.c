
#include "lv_app.h"
#include "lvgl.h"
#include "pages/pages.h"
#include "lvgl/demos/lv_demos.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
static void gui_thread(void *pvParameter)
{
    (void)pvParameter;
    lv_init();
    lv_port_init();
    pm_init();
    pages_init();
    // lv_demo_widgets();
    while (1)
    {
        lv_task_handler();
        pm_run();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    // Never returns
}
void lv_app_init(void)
{
    xTaskCreate(gui_thread, "lvgl", 1024 * 8, NULL, 3, NULL);
}