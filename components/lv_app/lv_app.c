
#include "lv_app.h"
#include "lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pages/page.h"
static void gui_thread(void *pvParameter)
{
	(void)pvParameter;
	while (1)
	{
		lv_task_handler();
		vTaskDelay(pdMS_TO_TICKS(10));
	}
	// Never returns
}
void lv_app_init(void)
{
    lv_init();
    lv_port_init();
    lv_demo_benchmark();
	//lv_demo_music();
	//lv_demo_widgets();
	//lv_demo_stress();
	// pages_init();
	// page_push(&page_main);
	xTaskCreate(gui_thread,"lvgl", 1024 * 8, NULL, 12, NULL);
    // xTaskCreatePinnedToCore(gui_thread, "lvgl", 1024 * 8, NULL, 12, NULL, 1);
}