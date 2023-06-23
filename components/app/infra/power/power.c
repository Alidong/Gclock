#include "pal.h"
#include "power.h"
// #include "button/button.h"
// #include "bat/battery.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "power";
typedef struct
{
    TaskHandle_t *pThread;
    bool sleep; //sleep cmd
} power_ctrl_t;
static power_ctrl_t st_powerCtrl;
static void power_manager_func(void)
{
    // button_wait_event(KEY_MASK_LONG_PRESS(KEY_1) | KEY_MASK_LONG_PRESS(KEY_2), portMAX_DELAY);
    // if (!st_powerCtrl.sleep)
    // {
    //     st_powerCtrl.sleep = true;
    //     fcntl(DRV->lcdHandle, LCD_SET_LIGHT, 0);
    // }
    // else
    // {
    //     st_powerCtrl.sleep = false;
    //     fcntl(DRV->lcdHandle, LCD_SET_LIGHT, 100);
    // }
    vTaskDelay(pdMS_TO_TICKS(1000));
}
static void power_thread(void *p)
{
    while (1)
    {
        power_manager_func();
        vTaskDelay(30);
    }
}
void power_init(void)
{
    xTaskCreate(power_thread, "powerThrad", 1024 * 2, NULL, 7, st_powerCtrl.pThread);
}
void power_light_sleep(void)
{
    st_powerCtrl.sleep = true;
}
