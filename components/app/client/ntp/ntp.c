/* LwIP SNTP example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "ntp.h"
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_sleep.h"
#include "esp_sntp.h"
#include "wifi/wifi.h"
static const char *TAG = "sntp";

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 48
#endif

static bool obtain_time(void);
static void initialize_sntp(void);
static bool ntp_is_success=false;

#ifdef CONFIG_SNTP_TIME_SYNC_METHOD_CUSTOM
void sntp_sync_time(struct timeval *tv)
{
    settimeofday(tv, NULL);
    ESP_LOGI(TAG, "Time is synchronized from custom code");
    sntp_set_sync_status(SNTP_SYNC_STATUS_COMPLETED);
}
#endif

void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Notification of a time synchronization event");
    ntp_is_success=true;
}
void ntp_thread_func(void* p)
{
    while (!obtain_time())
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    time_t now;
    // update 'now' variable with current time
    time(&now);
    char strftime_buf[64];
    // Set timezone to China Standard Time
    setenv("TZ", "CST-8", 1);
    tzset();
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "The current date/time in Shanghai is: %s", strftime_buf);
    if (sntp_get_sync_mode() == SNTP_SYNC_MODE_SMOOTH)
    {
        struct timeval outdelta;
        while (sntp_get_sync_status() == SNTP_SYNC_STATUS_IN_PROGRESS)
        {
            adjtime(NULL, &outdelta);
            ESP_LOGI(TAG, "Waiting for adjusting time ... outdelta = %jd sec: %li ms: %li us",
                     (intmax_t)outdelta.tv_sec,
                     outdelta.tv_usec / 1000,
                     outdelta.tv_usec % 1000);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
    }
    vTaskDelete(NULL);
}
void ntp_init(void)
{
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    xTaskCreate(ntp_thread_func,"ntp",1024*4,NULL,8,NULL);
    ESP_LOGI(TAG, "Time is not set yet. Connecting to WiFi and getting time over NTP.");
}
bool ntp_sync_is_success()
{
    return ntp_is_success;
}
static bool obtain_time(void)
{
    if (wifi_is_connected())
    {
        initialize_sntp();
        // wait for time to be set
        int retry_count=0;
        while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET)
        {
            ESP_LOGI(TAG, "Waiting for system time to be set... (%d)",retry_count++);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
        return true;
    }
    else
    {
        return false;
    }
}

static void initialize_sntp(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");     // set the secondary NTP server (will be used only if SNTP_MAX_SERVERS > 1)
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    sntp_init();

    ESP_LOGI(TAG, "List of configured NTP servers:");

    for (uint8_t i = 0; i < SNTP_MAX_SERVERS; ++i)
    {
        if (sntp_getservername(i))
        {
            ESP_LOGI(TAG, "server %d: %s", i, sntp_getservername(i));
        }
        else
        {
            // we have either IPv4 or IPv6 address, let's print it
            char buff[INET6_ADDRSTRLEN];
            ip_addr_t const *ip = sntp_getserver(i);
            if (ipaddr_ntoa_r(ip, buff, INET6_ADDRSTRLEN) != NULL)
                ESP_LOGI(TAG, "server %d: %s", i, buff);
        }
    }
}
