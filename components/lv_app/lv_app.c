
#include "lv_app.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
static void gui_thread(void *pvParameter)
{
    (void)pvParameter;
    lv_init();
    lv_port_init();
    pm_init();
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