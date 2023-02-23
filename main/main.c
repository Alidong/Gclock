/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_vfs.h"
#include "device/device.h"
#include "lv_app/lv_app.h"
#include "server/server.h"
#include "client/client.h"
#include "device/device.h"
void app_main(void)
{
    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), WiFi%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        printf("Get flash size failed");
        return;
    }

    printf("%luMB %s flash\n", (uint32_t)(flash_size / (1024 * 1024)),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %ld bytes\n", esp_get_minimum_free_heap_size());
    //device
    device_init();
    vTaskDelay(pdMS_TO_TICKS(1000));
    server_init();
    client_init();
    //lvgl
    lv_app_init();
    // while (1)
    // {
    //     vTaskDelay(pdMS_TO_TICKS(1000));
    //     aht10_data_t data;
    //     read(DEV->aht10Handle,&data,sizeof(aht10_data_t));
    //     printf("AHT10 Temp=%ld.%01ldC Humi=%ld.%01ld%%\r\n",data.temp/100,data.temp%100,data.humi/10,data.humi%10);
    // }
}
