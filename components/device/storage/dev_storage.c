#include "dev_storage.h"
#include "device.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_vfs.h"
#include "esp_vfs_dev.h"
#include "esp_partition.h"
#include "esp_vfs_fat.h"
#include "nvs_flash.h"
#include "wear_levelling.h"
#include "esp_err.h"
#include "esp_log.h"
static const char *TAG = "dev storage";
static TimerHandle_t timer;
 static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;
static void sd_timer(TimerHandle_t xTimer)
{
    if(!DEV->isSdOk)
    {
        ESP_LOGI(TAG,"try mount sd!");
        if(sd_mount()==ESP_OK)
        {
            ESP_LOGI(TAG,"mount sd ok!");
            DEV->isSdOk=true;
        }
    }
    else
    {
        if (!sd_detect())
        {
            ESP_LOGI(TAG,"unmount sd!");
            DEV->isSdOk=false;
            sd_unmount();
        }
    }
}
esp_err_t dev_storage_init(void) 
{
    // Initialize NVS 
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
    //mount sd
    timer=xTimerCreate("sd_mount",pdMS_TO_TICKS(2000),true,(void*)1,sd_timer);
    err = sd_init();
    if (err==ESP_OK)
    {
        DEV->isSdOk=true;
    }
    else
    {
        DEV->isSdOk=false;
    }
    xTimerStart(timer,pdMS_TO_TICKS(1000));

    //mount internal flash file system
    ESP_LOGI(TAG, "Mounting FAT filesystem");
    const esp_vfs_fat_mount_config_t mount_config = {
            .max_files = 4,
            .format_if_mount_failed = true,
            .allocation_unit_size = CONFIG_WL_SECTOR_SIZE
    };
    err = esp_vfs_fat_spiflash_mount(FLASH_MOUNT_PATH, "storage", &mount_config, &s_wl_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
        return err;
    }
     ESP_LOGI(TAG, "FAT filesystem mounted!");
    // Print FAT FS size information
    // uint64_t bytes_total, bytes_free;
    // esp_vfs_fat_info(FLASH_MOUNT_PATH, &bytes_total, &bytes_free);
    // ESP_LOGI(TAG, "FAT FS: %lld kB total, %lld kB free", bytes_total / 1024, bytes_free / 1024);
    return err;
}
